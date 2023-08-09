#include "pages_weather.h"
#include "pages_weather_ui.h"
#include "icons_weather.h"
#include "system.h"
#include "pages_main.h"
#include "ArduinoJson.h"
#include <esp32-hal-timer.h>
#include <map>

#define WEATHER_APP_NAME "天气"
#define WEATHER_API_LATLON "http://api.openweathermap.org/data/2.5/weather?q=%s,%s&appid=%s&units=metric&lang=zh_cn"
#define WEATHER_API_CITY "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric&lang=zh_cn"
#define WEATHER_PAGE_SIZE 2

// 存储天气配置
#define WEATHER_CONFIG_PATH "/weather.json"
struct WeatherCfg
{
    String appid;                 // API KEY
    String city;                  // 城市
    String lat;                   // 纬度
    String lon;                   // 经度
    unsigned long lastUpdate;     // 上次更新时间
    unsigned long updateInterval; // 更新间隔
};

static void writeCfg(WeatherCfg &cfg)
{
    File file = SPIFFS.open(WEATHER_CONFIG_PATH, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open config file for writing");
        return;
    }

    DynamicJsonDocument doc(1024);
    doc["appid"] = cfg.appid;
    doc["city"] = cfg.city;
    doc["lat"] = cfg.lat;
    doc["lon"] = cfg.lon;
    doc["lastUpdate"] = cfg.lastUpdate;
    doc["updateInterval"] = cfg.updateInterval;

    serializeJson(doc, file);
    file.close();
}

static void readCfg(WeatherCfg *cfg)
{
    File file = SPIFFS.open(WEATHER_CONFIG_PATH, FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open config file for reading");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
    }

    cfg->appid = doc["appid"].as<String>();
    cfg->city = doc["city"].as<String>();
    cfg->lat = doc["lat"].as<String>();
    cfg->lon = doc["lon"].as<String>();
    cfg->lastUpdate = doc["lastUpdate"].as<unsigned long>();
    cfg->updateInterval = doc["updateInterval"].as<unsigned long>();

    file.close();
}

struct WeatherRunData
{
    unsigned long lastUpdate;              // 上次更新时间
    unsigned long updateInterval;          // 更新间隔
    unsigned int forceUpdate;              // 强制更新
    unsigned int currentPage;              // 当前页面
    BaseType_t xReturned_task_task_update; // 更新数据的异步任务
    TaskHandle_t xHandle_task_task_update; // 更新数据的异步任务
    Weather wea;                           // 保存天气状况
};

static WeatherCfg cfg_data;
static WeatherRunData *run_data = NULL;

std::map<int, lv_img_dsc_t> weatherMap = {
    // Group 2xx: Thunderstorm
    {200, weather_lightning_rainy}, // thunderstorm with light rain
    {201, weather_lightning_rainy}, // thunderstorm with rain
    {202, weather_lightning_rainy}, // thunderstorm with heavy rain
    {210, weather_lightning},       // light thunderstorm
    {211, weather_lightning},       // thunderstorm
    {212, weather_lightning},       // heavy thunderstorm
    {221, weather_lightning},       // ragged thunderstorm
    {230, weather_lightning_rainy}, // thunderstorm with light drizzle
    {231, weather_lightning_rainy}, // thunderstorm with drizzle
    {232, weather_lightning_rainy}, // thunderstorm with heavy drizzle
    // Group 3xx: Drizzle
    {300, weather_rainy}, // light intensity drizzle
    {301, weather_rainy}, // drizzle
    {302, weather_rainy}, // heavy intensity drizzle
    {310, weather_rainy}, // light intensity drizzle rain
    {311, weather_rainy}, // drizzle rain
    {312, weather_rainy}, // heavy intensity drizzle rain
    {313, weather_rainy}, // shower rain and drizzle
    {314, weather_rainy}, // heavy shower rain and drizzle
    {321, weather_rainy}, // shower drizzle
    // Group 5xx: Rain
    {500, weather_rainy},       // light rain
    {501, weather_rainy},       // moderate rain
    {502, weather_pouring},     // heavy intensity rain
    {503, weather_pouring},     // very heavy rain
    {504, weather_pouring},     // extreme rain
    {511, weather_snowy_rainy}, // freezing rain
    {520, weather_rainy},       // light intensity shower rain
    {521, weather_rainy},       // shower rain
    {522, weather_rainy},       // heavy intensity shower rain
    {531, weather_rainy},       // ragged shower rain
    // Group 6xx: Snow
    {600, weather_snowy},       // light snow
    {601, weather_snowy},       // snow
    {602, weather_snowy_heavy}, // heavy snow
    {611, weather_snowy_rainy}, // sleet
    {612, weather_snowy_rainy}, // light shower sleet
    {613, weather_snowy_rainy}, // shower sleet
    {615, weather_snowy_rainy}, // light rain and snow
    {616, weather_snowy_rainy}, // rain and snow
    {620, weather_snowy},       // light shower snow
    {621, weather_snowy},       // shower snow
    {622, weather_snowy_heavy}, // heavy shower snow
    // Group 7xx: Atmosphere
    {701, weather_fog},     // mist
    {711, weather_fog},     // smoke
    {721, weather_fog},     // haze
    {731, weather_dust},    // sand/dust whirls
    {741, weather_fog},     // fog
    {751, weather_dust},    // sand
    {761, weather_dust},    // dust
    {762, weather_dust},    // volcanic ash
    {771, weather_fog},     // squalls
    {781, weather_tornado}, // tornado
    // Group 800: Clear
    {800, weather_sunny}, // clear sky
    // Group 80x: Clouds
    {801, weather_partly_cloudy}, // few clouds: 11-25%
    {802, weather_partly_cloudy}, // scattered clouds: 25-50%
    {803, weather_cloudy},        // broken clouds: 51-84%
    {804, weather_cloudy},        // overcast clouds: 85-100%
};

static void task_update(void *pvParameters)
{
    WeatherRunData *data = (WeatherRunData *)pvParameters;
    while (1)
    {
        if (data->forceUpdate || (millis() - data->lastUpdate > data->updateInterval))
        {
            data->forceUpdate = 0;
            data->lastUpdate = millis();
            Serial.println("task_update");
            char url[128] = {0};
            if (cfg_data.city.length() == 0)
            {
                snprintf(url, 128, WEATHER_API_LATLON, String(cfg_data.lat).c_str(), String(cfg_data.lon).c_str(), String(cfg_data.appid).c_str());
            }
            else
            {
                snprintf(url, 128, WEATHER_API_CITY, String(cfg_data.city).c_str(), String(cfg_data.appid).c_str());
            }
            Serial.println(url);
            HTTPClient http;
            http.begin(url);
            int httpCode = http.GET();
            if (httpCode > 0)
            {
                String payload = http.getString();
                Serial.println(payload);
                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, payload);
                if (error)
                {
                    Serial.println("Failed to read file, using default configuration");
                }
                else
                {
                    data->wea.id = doc["weather"][0]["id"].as<int>();
                    strcpy(data->wea.name, doc["name"].as<char *>());
                    strcpy(data->wea.main, doc["weather"][0]["main"].as<char *>());
                    strcpy(data->wea.description, doc["weather"][0]["description"].as<char *>());
                    data->wea.temp = doc["main"]["temp"].as<double>();
                    data->wea.feels_like = doc["main"]["feels_like"].as<double>();
                    data->wea.pressure = doc["main"]["pressure"].as<int>();
                    data->wea.humidity = doc["main"]["humidity"].as<int>();
                    data->wea.wind_speed = doc["wind"]["speed"].as<double>();
                    data->wea.wind_deg = doc["wind"]["deg"].as<int>();
                    data->wea.clouds = doc["clouds"]["all"].as<int>();
                    data->wea.visibility = doc["visibility"].as<int>();
                    data->wea.dt = doc["dt"].as<long>();
                    data->wea.sunrise = doc["sys"]["sunrise"].as<long>();
                    data->wea.sunset = doc["sys"]["sunset"].as<long>();
                }
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
            http.end();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void get_weather(void)
{
    if (run_data == NULL)
    {
        run_data = new WeatherRunData;
        run_data->lastUpdate = 0;
        run_data->updateInterval = cfg_data.updateInterval;
        run_data->forceUpdate = 1;
        run_data->xReturned_task_task_update = xTaskCreate(
            task_update,                          /* Task function. */
            "task_update",                        /* String with name of task. */
            10000,                                /* Stack size in bytes. */
            run_data,                             /* Parameter passed as input of the task */
            1,                                    /* Priority of the task. */
            &run_data->xHandle_task_task_update); /* Task handle. */
    }
    else
    {
        run_data->forceUpdate = 1;
    }
}

static int weather_init(XPages *sys)
{
    tft->setSwapBytes(true);
    weather_gui_init();
    readCfg(&cfg_data);

    run_data = new WeatherRunData;
    memset((char *)&run_data->wea, 0, sizeof(Weather));
    run_data->lastUpdate = 0; // 1970-01-01 00:00:00
    run_data->updateInterval = cfg_data.updateInterval;
    run_data->forceUpdate = 1;
    run_data->currentPage = 0;
    run_data->xReturned_task_task_update = xTaskCreate(
        task_update,                          /* Task function. */
        "task_update",                        /* String with name of task. */
        8000,                                 /* Stack size in bytes. */
        run_data,                             /* Parameter passed as input of the task */
        1,                                    /* Priority of the task. */
        &run_data->xHandle_task_task_update); /* Task handle. */
    return 0;
}

static void click_BACK()
{
    xpages->app_exit();
}

static void click_FORWARD()
{
    run_data->forceUpdate = 1;
    delay(500);
}

static void click_LEFT()
{
    run_data->currentPage = (run_data->currentPage + WEATHER_PAGE_SIZE - 1) % WEATHER_PAGE_SIZE;
    if (run_data->currentPage == 0)
    {
        display_weather(run_data->wea, LV_SCR_LOAD_ANIM_MOVE_LEFT);
    }
    else
    {
        display_detail(run_data->wea, LV_SCR_LOAD_ANIM_MOVE_LEFT);
    }
}

static void click_RIGHT()
{
    run_data->currentPage = (run_data->currentPage + 1) % WEATHER_PAGE_SIZE;
    if (run_data->currentPage == 0)
    {
        display_weather(run_data->wea, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    }
    else
    {
        display_detail(run_data->wea, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    }
}

static void weather_process(XPages *sys,
                            Buttons *btns_info)
{
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    btns_info->tick();
    btns.attachClick(BACK, click_BACK);
    btns.attachClick(FORWARD, click_FORWARD);
    btns.attachClick(LEFT, click_LEFT);
    btns.attachClick(RIGHT, click_RIGHT);
}

static void weather_background_task(XPages *sys,
                                    Buttons *btns_info)
{
}

static int weather_exit_callback(void *param)
{
    weather_gui_del();
    if (run_data->xReturned_task_task_update == pdPASS)
    {
        vTaskDelete(run_data->xHandle_task_task_update);
    }
    if (run_data != NULL)
    {
        delete run_data;
        run_data = NULL;
    }
    return 0;
}

static void weather_message_handle(const char *from,
                                   const char *to,
                                   APP_MESSAGE_TYPE type,
                                   void *message,
                                   void *ext_info)
{
    switch (type)
    {
    case APP_MESSAGE_WIFI_CONN:
    {
        get_weather();
        if (run_data->currentPage == 0)
        {
            display_weather(run_data->wea, LV_SCR_LOAD_ANIM_NONE);
        }
        break;
    }
    case APP_MESSAGE_GET_PARAM:
    {
        char *param = (char *)message;
        if (strcmp(param, "appid") == 0)
        {
            strcpy((char *)ext_info, cfg_data.appid.c_str());
        }
        else if (strcmp(param, "city") == 0)
        {
            strcpy((char *)ext_info, cfg_data.city.c_str());
        }
        else if (strcmp(param, "lat") == 0)
        {
            strcpy((char *)ext_info, cfg_data.lat.c_str());
        }
        else if (strcmp(param, "lon") == 0)
        {
            strcpy((char *)ext_info, cfg_data.lon.c_str());
        }
        else if (strcmp(param, "updateInterval") == 0)
        {
            sprintf((char *)ext_info, "%lu", cfg_data.updateInterval);
        }
    }
    case APP_MESSAGE_SET_PARAM:
    {
        char *param_key = (char *)message;
        char *param_val = (char *)ext_info;
        if (strcmp(param_key, "appid") == 0)
        {
            cfg_data.appid = param_val;
            writeCfg(cfg_data);
        }
        else if (strcmp(param_key, "city") == 0)
        {
            cfg_data.city = param_val;
            writeCfg(cfg_data);
        }
        else if (strcmp(param_key, "lat") == 0)
        {
            cfg_data.lat = param_val;
            writeCfg(cfg_data);
        }
        else if (strcmp(param_key, "lon") == 0)
        {
            cfg_data.lon = param_val;
            writeCfg(cfg_data);
        }
        else if (strcmp(param_key, "updateInterval") == 0)
        {
            cfg_data.updateInterval = atol(param_val);
            writeCfg(cfg_data);
        }
        break;
    }
    case APP_MESSAGE_READ_CFG:
    {
        readCfg(&cfg_data);
        break;
    }
    case APP_MESSAGE_WRITE_CFG:
    {
        writeCfg(cfg_data);
        break;
    }
    default:
        break;
    }
}

APP_OBJ weather_app = {WEATHER_APP_NAME,
                       &app_weather,
                       "",
                       weather_init,
                       weather_process,
                       weather_background_task,
                       weather_exit_callback,
                       weather_message_handle};