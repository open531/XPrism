#include "app_time.h"
#include "app_time_ui.h"
#include "system.h"
#include "app_center.h"
#include "ArduinoJson.h"
#include "ESP32Time.h"

#define APP_TIME_NAME "时间"
#define APP_TIME_INFO "显示时间信息"
#define TIME_API "http://api.m.taobao.com/rest/api3.do?api=mtop.common.gettimestamp"

#define TIME_CFG_PATH "/time.cfg"
#define TIMEZERO_OFFSIZE (28800000)
#define GET_SYS_MILLIS xTaskGetTickCount // 获取系统毫秒数

struct TimeCfg
{
    char appid[50];
    unsigned long lastUpdate;
};

struct TimeRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    unsigned int currPage;
    BaseType_t xReturn;
    TaskHandle_t xHandle;
    Time timInfo;
    
    long long preNetTimestamp; 
    ESP32Time g_rtc;
    long long errorNetTimestamp;    // 网络到显示过程中的时间误差
    long long preLocalTimestamp;    // 上一次的本地机器时间戳
};

static void readTimeCfg(TimeCfg *cfg)
{
    FILE *fp = fopen(TIME_CFG_PATH, "rb");
    if (fp)
    {
        fread(cfg, sizeof(TimeCfg), 1, fp);
        fclose(fp);
    }
}

static void writeTimeCfg(TimeCfg *cfg)
{
    FILE *fp = fopen(TIME_CFG_PATH, "wb");
    if (fp)
    {
        fwrite(cfg, sizeof(TimeCfg), 1, fp);
        fclose(fp);
    }
}

static TimeCfg timeCfg;
static TimeRunData *timeRunData = NULL;



static void UpdateTime_RTC(long long timestamp)
{
    struct Time t;
    timeRunData->g_rtc.setTime(timestamp / 1000);
    t.month = timeRunData->g_rtc.getMonth() + 1;
    t.day = timeRunData->g_rtc.getDay();
    t.hour = timeRunData->g_rtc.getHour(true);
    t.minute = timeRunData->g_rtc.getMinute();
    t.second = timeRunData->g_rtc.getSecond();
    t.weekday = timeRunData->g_rtc.getDayofWeek();
    // Serial.printf("time : %d-%d-%d\n",t.hour, t.minute, t.second);
    appTimeUiDisplayBasic(t);
}

static long long get_timestamp(String url)
{
    if (WL_CONNECTED != WiFi.status())
        return 0;

    String time = "";
    HTTPClient http;
    http.setTimeout(1000);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println(payload);
            int time_index = (payload.indexOf("data")) + 12;
            time = payload.substring(time_index, payload.length() - 3);
            // 以网络时间戳为准
            timeRunData->preNetTimestamp = atoll(time.c_str()) + timeRunData->errorNetTimestamp + TIMEZERO_OFFSIZE;
            timeRunData->preLocalTimestamp = GET_SYS_MILLIS();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        // 得不到网络时间戳时
        timeRunData->preNetTimestamp = timeRunData->preNetTimestamp + (GET_SYS_MILLIS() - timeRunData->preLocalTimestamp);
        timeRunData->preLocalTimestamp = GET_SYS_MILLIS();
    }
    http.end();

    return timeRunData->preNetTimestamp;
}


static void TimeTask(void *pvParameters)
{
    TimeRunData *data = (TimeRunData *)pvParameters;
    while (1)
    {
        if (data->forceUpdate || (millis() - data->lastUpdate > 1000))
        {
            data->forceUpdate = 0;
            data->lastUpdate = millis();
            long long timestamp = get_timestamp(TIME_API); // nowapi时间API
            UpdateTime_RTC(timestamp);
            
           
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void getTime()
{
    if (timeRunData == NULL)
    {
        timeRunData = (TimeRunData *)malloc(sizeof(timeRunData));
        timeRunData->lastUpdate = 0;
        timeRunData->forceUpdate = 1;
        timeRunData->xReturn = xTaskCreate(TimeTask,
                                              "TimeTask",
                                              4096,
                                              timeRunData,
                                              1,
                                              &timeRunData->xHandle);
    }
    else
    {
        timeRunData->forceUpdate = 1;
    }
}

static int timeInit(AppCenter *appCenter)
{
    readTimeCfg(&timeCfg);
    getTime();
    return 0;
}

static void clickBack()
{
    appCenter->appExit();
}


static void timeRoutine(AppCenter *appCenter)
{
    m_button.routine();
    m_button.attachClick(BTN_BACK, clickBack);
    
}

static void timeBackground(AppCenter *appCenter)
{
}

static int timeExit(AppCenter *appCenter)
{
    appTimeUiDelete();
    if (timeRunData != NULL)
    {
        vTaskDelete(timeRunData->xHandle);
        free(timeRunData);
        timeRunData = NULL;
    }
    return 0;
}

static void timeOnMessage(const char *from, const char *to,
                             AppMsg type, void *msg, void *info)
{
    if (type == APP_MSG_WIFI_CONN)
    {
        getTime();
        appTimeUiDisplayBasic(timeRunData->timInfo);
    }
    else if (type == APP_MSG_GET_PARAM)
    {
        snprintf((char *)info, 50, "%s", timeCfg.appid);
        
    }
    else if (type == APP_MSG_SET_PARAM)
    {
        char *paramKey = (char *)msg;
        char *paramValue = (char *)info;
        if (strcmp((char *)msg, "appid") == 0)
        {
            snprintf(timeCfg.appid, 50, "%s", paramValue);
        }
        else if (strcmp((char *)msg, "save") == 0)
        {
            writeTimeCfg(&timeCfg);
        }
        else if (strcmp((char *)msg, "update") == 0)
        {
            getTime();
        }
    }
    else if (type == APP_MSG_READ_CFG)
    {
        readTimeCfg(&timeCfg);
    }
    else if (type == APP_MSG_WRITE_CFG)
    {
        writeTimeCfg(&timeCfg);
    }
}

App TimeApp = {
    APP_TIME_NAME,
    &icon_time,
    APP_TIME_INFO,
    timeInit,
    timeRoutine,
    timeBackground,
    timeExit,
    timeOnMessage};