#include "pages_weather.h"
#include "pages_weather_ui.h"
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
    doc["lastUpdate"] = cfg.lastUpdate;
    doc["updateInterval"] = cfg.updateInterval;

    serializeJson(doc, file);
    file.close();
}

static void readCfg(WeatherCfg &cfg)
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

    cfg.appid = doc["appid"].as<String>();
    cfg.city = doc["city"].as<String>();
    cfg.lastUpdate = doc["lastUpdate"].as<unsigned long>();
    cfg.updateInterval = doc["updateInterval"].as<unsigned long>();

    file.close();
}

struct WeatherRunData
{
    unsigned long lastUpdate;              // 上次更新时间
    unsigned long updateInterval;          // 更新间隔
    unsigned int forceUpdate;              // 强制更新
    BaseType_t xReturned_task_task_update; // 更新数据的异步任务
    TaskHandle_t xHandle_task_task_update; // 更新数据的异步任务
    Weather wea;                           // 保存天气状况
};