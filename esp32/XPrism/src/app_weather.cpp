#include "app_weather.h"
#include "app_weather_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"
#include "common.h"
#include "ArduinoJson.h"
#include "ESP32Time.h"
#include <esp32-hal-timer.h>
#include <map>

#define APP_WEATHER_NAME "天气"
#define APP_WEATHER_INFO "显示天气信息"
#define WEATHER_API_LATLON "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric&lang=zh_cn"
#define WEATHER_API_CITY "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric&lang=zh_cn"
#define APP_WEATHER_PAGE_SIZE 2

// 天气的持久化配置
#define WEATHER_CONFIG_PATH "/weather.cfg"
struct WeatherCfg
{
    String appid;
    String city;
    String lat;
    String lon;
    unsigned long updateInterval; // 天气更新的时间间隔(s)
};

static void writeWeatherCfg(WeatherCfg *cfg)
{
    char tmp[16];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    w_data = w_data + cfg->appid + "\n";
    w_data = w_data + cfg->city + "\n";
    w_data = w_data + cfg->lat + "\n";
    w_data = w_data + cfg->lon + "\n";
    memset(tmp, 0, 16);
    snprintf(tmp, 16, "%lu\n", cfg->updateInterval);
    w_data += tmp;
    m_flashCfg.writeFile(WEATHER_CONFIG_PATH, w_data.c_str());
}

static void readWeatherCfg(WeatherCfg *cfg)
{
    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    uint16_t size = m_flashCfg.readFile(WEATHER_CONFIG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        cfg->appid = "c66ce2b79b320564e343d0d77e688026";
        cfg->city = "Beijing";
        cfg->lat = "40.00";
        cfg->lon = "116.32";
        cfg->updateInterval = 900000; // 天气更新的时间间隔900000(900s)
        writeWeatherCfg(cfg);
    }
    else
    {
        // 解析数据
        char *param[5] = {0};
        analyseParam(info, 5, param);
        cfg->appid = param[0];
        cfg->city = param[1];
        cfg->lat = param[2];
        cfg->lon = param[3];
        cfg->updateInterval = atol(param[4]);
    }
    Serial.println("Weather config:");
    Serial.println(info);
    Serial.println(cfg->appid);
    Serial.println(cfg->city);
    Serial.println(cfg->lat);
    Serial.println(cfg->lon);
    Serial.println(cfg->updateInterval);
}

struct WeatherAppRunData
{
    unsigned long lastUpdate; // 上一回更新天气时的毫秒数
    unsigned int forceUpdate; // 强制更新标志
    int currPage;

    BaseType_t xReturned_task_task_update; // 更新数据的异步任务
    TaskHandle_t xHandle_task_task_update; // 更新数据的异步任务

    Weather weaInfo; // 保存天气状况
};

static WeatherCfg weatherCfg;
static WeatherAppRunData *weatherRunData = NULL;

static void getWeather()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    HTTPClient http;
    http.setTimeout(1000);
    char url[128] = {0};
    if (weatherCfg.lat.length() > 0 && weatherCfg.lon.length() > 0)
    {
        snprintf(url, 128, WEATHER_API_LATLON,
                 weatherCfg.lat.c_str(), weatherCfg.lon.c_str(),
                 weatherCfg.appid.c_str());
    }
    else
    {
        snprintf(url, 128, WEATHER_API_CITY,
                 weatherCfg.city.c_str(), weatherCfg.appid.c_str());
    }
    Serial.println(url);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
            String payload = http.getString();
            Serial.println(payload);
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload);
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }
            weatherRunData->weaInfo.weatherId = doc["weather"][0]["id"].as<int>();
            strcpy(weatherRunData->weaInfo.weatherMain, doc["weather"][0]["main"].as<char *>());
            strcpy(weatherRunData->weaInfo.weatherDescription, doc["weather"][0]["description"].as<char *>());
            strcpy(weatherRunData->weaInfo.weatherIcon, doc["weather"][0]["icon"].as<char *>());
            weatherRunData->weaInfo.mainTemp = doc["main"]["temp"].as<float>();
            weatherRunData->weaInfo.mainFeelsLike = doc["main"]["feels_like"].as<float>();
            weatherRunData->weaInfo.mainTempMin = doc["main"]["temp_min"].as<float>();
            weatherRunData->weaInfo.mainTempMax = doc["main"]["temp_max"].as<float>();
            weatherRunData->weaInfo.mainPressure = doc["main"]["pressure"].as<float>();
            weatherRunData->weaInfo.mainHumidity = doc["main"]["humidity"].as<float>();
            weatherRunData->weaInfo.mainSeaLevel = doc["main"]["sea_level"].as<float>();
            weatherRunData->weaInfo.mainGrndLevel = doc["main"]["grnd_level"].as<float>();
            weatherRunData->weaInfo.visibility = doc["visibility"].as<float>();
            weatherRunData->weaInfo.windSpeed = doc["wind"]["speed"].as<float>();
            weatherRunData->weaInfo.windDeg = doc["wind"]["deg"].as<float>();
            weatherRunData->weaInfo.cloudsAll = doc["clouds"]["all"].as<float>();
            weatherRunData->weaInfo.dt = doc["dt"].as<unsigned long>();
            weatherRunData->weaInfo.sysSunrise = doc["sys"]["sunrise"].as<unsigned long>();
            weatherRunData->weaInfo.sysSunset = doc["sys"]["sunset"].as<unsigned long>();
            weatherRunData->weaInfo.timezone = doc["timezone"].as<int>();
            weatherRunData->weaInfo.id = doc["id"].as<int>();
            strcpy(weatherRunData->weaInfo.name, doc["name"].as<char *>());
            weatherRunData->weaInfo.cod = doc["cod"].as<int>();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

static int weatherInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appWeatherUiInit();
    weatherRunData = (WeatherAppRunData *)malloc(sizeof(WeatherAppRunData));
    memset((char *)&weatherRunData->weaInfo, 0, sizeof(Weather));
    weatherRunData->currPage = 0;
    weatherRunData->lastUpdate = 0;
    weatherRunData->forceUpdate = 1;
    strcpy(weatherRunData->weaInfo.name, "北京");
    weatherRunData->weaInfo.weatherId = 800;
    strcpy(weatherRunData->weaInfo.weatherDescription, "晴");
    weatherRunData->weaInfo.mainTemp = 25.0;
    readWeatherCfg(&weatherCfg);
    getWeather();
    return 0;
}

static void weatherRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;

    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }
    else if (action->active == BTN_FORWARD)
    {
        weatherRunData->forceUpdate = 1;
        delay(500);
    }
    else if (action->active == BTN_LEFT)
    {
        animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
        weatherRunData->currPage = (weatherRunData->currPage + 1) % APP_WEATHER_PAGE_SIZE;
    }
    else if (action->active == BTN_RIGHT)
    {
        animType = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        weatherRunData->currPage = (weatherRunData->currPage + APP_WEATHER_PAGE_SIZE - 1) % APP_WEATHER_PAGE_SIZE;
    }

    if (weatherRunData->currPage == 0)
    {
        appWeatherUiDisplayBasic(weatherRunData->weaInfo, animType);
        delay(300);
    }
    else
    {
        appWeatherUiDisplayDetail(weatherRunData->weaInfo, animType);
        delay(300);
    }
}

static void weatherBackground(AppCenter *appCenter, const Action *action)
{
}

static int weatherExit(void *param)
{
    appWeatherUiDelete();
    if (weatherRunData->xReturned_task_task_update == pdPASS)
    {
        vTaskDelete(weatherRunData->xHandle_task_task_update);
    }
    if (weatherRunData != NULL)
    {
        free(weatherRunData);
        weatherRunData = NULL;
    }
    return 0;
}

static void weatherOnMessage(const char *from, const char *to,
                             AppMsgType type, void *msg, void *info)
{
    if (type == APP_MESSAGE_WIFI_CONN)
    {
        getWeather();
        if (weatherRunData->currPage == 0)
        {
            appWeatherUiDisplayBasic(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_NONE);
        }
        else
        {
            appWeatherUiDisplayDetail(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_NONE);
        }
    }
    else if (type == APP_MESSAGE_GET_PARAM)
    {
        if (strcmp((char *)msg, "appid") == 0)
        {
            snprintf((char *)info, 32, "%s", weatherCfg.appid);
        }
        else if (strcmp((char *)msg, "city") == 0)
        {
            snprintf((char *)info, 32, "%s", weatherCfg.city);
        }
        else if (strcmp((char *)msg, "lat") == 0)
        {
            snprintf((char *)info, 32, "%s", weatherCfg.lat);
        }
        else if (strcmp((char *)msg, "lon") == 0)
        {
            snprintf((char *)info, 32, "%s", weatherCfg.lon);
        }
        else if (strcmp((char *)msg, "updateInterval") == 0)
        {
            snprintf((char *)info, 32, "%lu", weatherCfg.updateInterval);
        }
        else
        {
            snprintf((char *)info, 32, "%s", "NULL");
        }
    }
    else if (type == APP_MESSAGE_SET_PARAM)
    {
        char *paramValue = (char *)info;
        if (strcmp((char *)msg, "appid") == 0)
        {
            weatherCfg.appid = paramValue;
        }
        else if (strcmp((char *)msg, "city") == 0)
        {
            weatherCfg.city = paramValue;
        }
        else if (strcmp((char *)msg, "lat") == 0)
        {
            weatherCfg.lat = paramValue;
        }
        else if (strcmp((char *)msg, "lon") == 0)
        {
            weatherCfg.lon = paramValue;
        }
        else if (strcmp((char *)msg, "updateInterval") == 0)
        {
            weatherCfg.updateInterval = atol(paramValue);
        }
    }
    else if (type == APP_MESSAGE_READ_CFG)
    {
        readWeatherCfg(&weatherCfg);
    }
    else if (type == APP_MESSAGE_WRITE_CFG)
    {
        writeWeatherCfg(&weatherCfg);
    }
}

App weatherApp = {
    APP_WEATHER_NAME,
    &icon_weather,
    APP_WEATHER_INFO,
    weatherInit,
    weatherRoutine,
    weatherBackground,
    weatherExit,
    weatherOnMessage};