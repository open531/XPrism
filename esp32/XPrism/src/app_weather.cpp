#include "app_weather.h"
#include "app_weather_ui.h"
#include "icons_weather.h"
#include "system.h"
#include "app_center.h"
#include "ArduinoJson.h"

#define APP_WEATHER_NAME "天气"
#define APP_WEATHER_INFO "显示天气信息"
#define WEATHER_API_LATLON "http://api.openweathermap.org/data/2.5/weather?q=%s,%s&appid=%s&units=metric&lang=zh_cn"
#define WEATHER_API_CITY "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric&lang=zh_cn"
#define APP_WEATHER_PAGE_SIZE 2

#define WEATHER_CFG_PATH "/weather.cfg"

struct WeatherCfg
{
    char appid[50];
    char city[50];
    char lat[10];
    char lon[10];
    unsigned long lastUpdate;
};

struct WeatherRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    unsigned int currPage;
    BaseType_t xReturn;
    TaskHandle_t xHandle;
    Weather weaInfo;
};

static void readWeatherCfg(WeatherCfg *cfg)
{
    FILE *fp = fopen(WEATHER_CFG_PATH, "rb");
    if (fp)
    {
        fread(cfg, sizeof(WeatherCfg), 1, fp);
        fclose(fp);
    }
}

static void writeWeatherCfg(WeatherCfg *cfg)
{
    FILE *fp = fopen(WEATHER_CFG_PATH, "wb");
    if (fp)
    {
        fwrite(cfg, sizeof(WeatherCfg), 1, fp);
        fclose(fp);
    }
}

static WeatherCfg weatherCfg;
static WeatherRunData *weatherRunData = NULL;

static void weatherTask(void *pvParameters)
{
    WeatherRunData *data = (WeatherRunData *)pvParameters;
    while (1)
    {
        if (data->forceUpdate || (millis() - data->lastUpdate > 1000 * 60 * 60))
        {
            data->forceUpdate = 0;
            data->lastUpdate = millis();
            if (strlen(weatherCfg.lat) > 0 && strlen(weatherCfg.lon) > 0)
            {
                char url[200];
                sprintf(url, WEATHER_API_LATLON, weatherCfg.lat, weatherCfg.lon, weatherCfg.appid);
                HTTPClient http;
                http.begin(url);
                int httpCode = http.GET();
                if (httpCode > 0)
                {
                    String payload = http.getString();
                    DynamicJsonDocument doc(1024);
                    deserializeJson(doc, payload);
                    JsonObject root = doc.as<JsonObject>();
                    data->weaInfo.weatherId = root["weather"][0]["id"];
                    strcpy(data->weaInfo.weatherMain, root["weather"][0]["main"]);
                    strcpy(data->weaInfo.weatherDescription, root["weather"][0]["description"]);
                    strcpy(data->weaInfo.weatherIcon, root["weather"][0]["icon"]);
                    strcpy(data->weaInfo.base, root["base"]);
                    data->weaInfo.mainTemp = root["main"]["temp"];
                    data->weaInfo.mainFeelsLike = root["main"]["feels_like"];
                    data->weaInfo.mainTempMin = root["main"]["temp_min"];
                    data->weaInfo.mainTempMax = root["main"]["temp_max"];
                    data->weaInfo.mainPressure = root["main"]["pressure"];
                    data->weaInfo.mainHumidity = root["main"]["humidity"];
                    data->weaInfo.mainSeaLevel = root["main"]["sea_level"];
                    data->weaInfo.mainGrndLevel = root["main"]["grnd_level"];
                    data->weaInfo.visibility = root["visibility"];
                    data->weaInfo.windSpeed = root["wind"]["speed"];
                    data->weaInfo.windDeg = root["wind"]["deg"];
                    data->weaInfo.windGust = root["wind"]["gust"];
                    data->weaInfo.rain1h = root["rain"]["1h"];
                    data->weaInfo.rain3h = root["rain"]["3h"];
                    data->weaInfo.snow1h = root["snow"]["1h"];
                    data->weaInfo.snow3h = root["snow"]["3h"];
                    data->weaInfo.cloudsAll = root["clouds"]["all"];
                    data->weaInfo.dt = root["dt"];
                    strcpy(data->weaInfo.sysCountry, root["sys"]["country"]);
                    data->weaInfo.sysSunrise = root["sys"]["sunrise"];
                    data->weaInfo.sysSunset = root["sys"]["sunset"];
                    data->weaInfo.timezone = root["timezone"];
                    data->weaInfo.id = root["id"];
                    strcpy(data->weaInfo.name, root["name"]);
                    data->weaInfo.cod = root["cod"];
                }
                http.end();
            }
            else if (strlen(weatherCfg.city) > 0)
            {
                char url[200];
                sprintf(url, WEATHER_API_CITY, weatherCfg.city, weatherCfg.appid);
                HTTPClient http;
                http.begin(url);
                int httpCode = http.GET();
                if (httpCode > 0)
                {
                    String payload = http.getString();
                    DynamicJsonDocument doc(1024);
                    deserializeJson(doc, payload);
                    JsonObject root = doc.as<JsonObject>();
                    data->weaInfo.weatherId = root["weather"][0]["id"];
                    strcpy(data->weaInfo.weatherMain, root["weather"][0]["main"]);
                    strcpy(data->weaInfo.weatherDescription, root["weather"][0]["description"]);
                    strcpy(data->weaInfo.weatherIcon, root["weather"][0]["icon"]);
                    strcpy(data->weaInfo.base, root["base"]);
                    data->weaInfo.mainTemp = root["main"]["temp"];
                    data->weaInfo.mainFeelsLike = root["main"]["feels_like"];
                    data->weaInfo.mainTempMin = root["main"]["temp_min"];
                    data->weaInfo.mainTempMax = root["main"]["temp_max"];
                    data->weaInfo.mainPressure = root["main"]["pressure"];
                    data->weaInfo.mainHumidity = root["main"]["humidity"];
                    data->weaInfo.mainSeaLevel = root["main"]["sea_level"];
                    data->weaInfo.mainGrndLevel = root["main"]["grnd_level"];
                    data->weaInfo.visibility = root["visibility"];
                    data->weaInfo.windSpeed = root["wind"]["speed"];
                    data->weaInfo.windDeg = root["wind"]["deg"];
                    data->weaInfo.windGust = root["wind"]["gust"];
                    data->weaInfo.rain1h = root["rain"]["1h"];
                    data->weaInfo.rain3h = root["rain"]["3h"];
                    data->weaInfo.snow1h = root["snow"]["1h"];
                    data->weaInfo.snow3h = root["snow"]["3h"];
                    data->weaInfo.cloudsAll = root["clouds"]["all"];
                    data->weaInfo.dt = root["dt"];
                    strcpy(data->weaInfo.sysCountry, root["sys"]["country"]);
                    data->weaInfo.sysSunrise = root["sys"]["sunrise"];
                    data->weaInfo.sysSunset = root["sys"]["sunset"];
                    data->weaInfo.timezone = root["timezone"];
                    data->weaInfo.id = root["id"];
                    strcpy(data->weaInfo.name, root["name"]);
                    data->weaInfo.cod = root["cod"];
                }
                http.end();
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void getWeather()
{
    if (weatherRunData == NULL)
    {
        weatherRunData = (WeatherRunData *)malloc(sizeof(WeatherRunData));
        weatherRunData->lastUpdate = 0;
        weatherRunData->forceUpdate = 1;
        weatherRunData->xReturn = xTaskCreate(weatherTask,
                                              "weatherTask",
                                              4096,
                                              weatherRunData,
                                              1,
                                              &weatherRunData->xHandle);
    }
    else
    {
        weatherRunData->forceUpdate = 1;
    }
}

static int weatherInit(AppCenter *appCenter)
{
    readWeatherCfg(&weatherCfg);
    getWeather();
    return 0;
}

static void clickBack()
{
    appCenter->appExit();
}

static void clickForward()
{
    weatherRunData->forceUpdate = 1;
    delay(100);
}

static void clickLeft()
{
    weatherRunData->currPage = (weatherRunData->currPage + APP_WEATHER_PAGE_SIZE - 1) % APP_WEATHER_PAGE_SIZE;
    if (weatherRunData->currPage == 0)
    {
        appWeatherUiDisplayBasic(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_MOVE_LEFT);
    }
    else
    {
        appWeatherUiDisplayDetail(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_MOVE_LEFT);
    }
}

static void clickRight()
{
    weatherRunData->currPage = (weatherRunData->currPage + 1) % APP_WEATHER_PAGE_SIZE;
    if (weatherRunData->currPage == 0)
    {
        appWeatherUiDisplayBasic(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    }
    else
    {
        appWeatherUiDisplayDetail(weatherRunData->weaInfo, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    }
}

static void weatherRoutine(AppCenter *appCenter)
{
    m_button.routine();
    m_button.attachClick(BTN_BACK, clickBack);
    m_button.attachClick(BTN_FORWARD, clickForward);
    m_button.attachClick(BTN_LEFT, clickLeft);
    m_button.attachClick(BTN_RIGHT, clickRight);
}

static void weatherBackground(AppCenter *appCenter)
{
}

static int weatherExit(AppCenter *appCenter)
{
    appWeatherUiDelete();
    if (weatherRunData != NULL)
    {
        vTaskDelete(weatherRunData->xHandle);
        free(weatherRunData);
        weatherRunData = NULL;
    }
    return 0;
}

static void weatherOnMessage(const char *from, const char *to,
                             AppMsg type, void *msg, void *info)
{
    if (type == APP_MSG_WIFI_CONN)
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
    else if (type == APP_MSG_GET_PARAM)
    {
        if (strcmp((char *)msg, "appid") == 0)
        {
            snprintf((char *)info, 50, "%s", weatherCfg.appid);
        }
        else if (strcmp((char *)msg, "city") == 0)
        {
            snprintf((char *)info, 50, "%s", weatherCfg.city);
        }
        else if (strcmp((char *)msg, "lat") == 0)
        {
            snprintf((char *)info, 10, "%s", weatherCfg.lat);
        }
        else if (strcmp((char *)msg, "lon") == 0)
        {
            snprintf((char *)info, 10, "%s", weatherCfg.lon);
        }
    }
    else if (type == APP_MSG_SET_PARAM)
    {
        char *paramKey = (char *)msg;
        char *paramValue = (char *)info;
        if (strcmp((char *)msg, "appid") == 0)
        {
            snprintf(weatherCfg.appid, 50, "%s", paramValue);
        }
        else if (strcmp((char *)msg, "city") == 0)
        {
            snprintf(weatherCfg.city, 50, "%s", paramValue);
        }
        else if (strcmp((char *)msg, "lat") == 0)
        {
            snprintf(weatherCfg.lat, 10, "%s", paramValue);
        }
        else if (strcmp((char *)msg, "lon") == 0)
        {
            snprintf(weatherCfg.lon, 10, "%s", paramValue);
        }
        else if (strcmp((char *)msg, "save") == 0)
        {
            writeWeatherCfg(&weatherCfg);
        }
        else if (strcmp((char *)msg, "update") == 0)
        {
            getWeather();
        }
    }
    else if (type == APP_MSG_READ_CFG)
    {
        readWeatherCfg(&weatherCfg);
    }
    else if (type == APP_MSG_WRITE_CFG)
    {
        writeWeatherCfg(&weatherCfg);
    }
}

App weatherApp = {
    APP_WEATHER_NAME,
    &weatherAppIcon,
    APP_WEATHER_INFO,
    weatherInit,
    weatherRoutine,
    weatherBackground,
    weatherExit,
    weatherOnMessage};