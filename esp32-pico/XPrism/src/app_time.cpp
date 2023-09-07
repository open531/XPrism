#include "app_time.h"
#include "app_time_ui.h"
#include "common.h"
#include "app_center.h"
#include "icons.h"
#include "ArduinoJson.h"
#include "ESP32Time.h"

#define APP_TIME_NAME "时间"
#define APP_TIME_INFO "显示时间信息"
#define TIME_API "http://api.m.taobao.com/rest/api3.do?api=mtop.common.gettimestamp"

#define TIME_CONFIG_PATH "/time.cfg"
#define TIMEZERO_OFFSIZE (28800000)
#define GET_SYS_MILLIS xTaskGetTickCount // 获取系统毫秒数

struct TimeCfg
{
    unsigned long updateInterval; // 时间更新的时间间隔(s)
};

struct TimeRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    int currPage;

    // BaseType_t xReturned_task_task_update;
    // TaskHandle_t xHandle_task_task_update;
    Time timInfo;

    long long preNetTimestamp;
    long long errorNetTimestamp; // 网络到显示过程中的时间误差
    long long preLocalTimestamp; // 上一次的本地机器时间戳

    ESP32Time g_rtc;
};

static void writeTimeCfg(TimeCfg *cfg)
{
    char tmp[16];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    memset(tmp, 0, 16);
    snprintf(tmp, 16, "%lu\n", cfg->updateInterval);
    w_data += tmp;
    m_flashCfg.writeFile(TIME_CONFIG_PATH, w_data.c_str());
}

static void readTimeCfg(TimeCfg *cfg)
{
    // char info[128] = {0};
    // uint16_t size = m_flashCfg.readFile(TIME_CONFIG_PATH, (uint8_t *)info);
    // info[size] = 0;
    // if (size == 0)
    // {
    // 默认值
    cfg->updateInterval = 900000; // 时间更新的时间间隔900000(900s)
    writeTimeCfg(cfg);
    // }
    // else
    // {
    //     // 解析数据
    //     char *param[1] = {0};
    //     analyseParam(info, 1, param);
    //     cfg->updateInterval = atol(param[0]);
    // }
}

static TimeCfg timeCfg;
static TimeRunData *timeRunData = NULL;

static long long getTimestamp()
{
    timeRunData->preNetTimestamp = timeRunData->preNetTimestamp + (GET_SYS_MILLIS() - timeRunData->preLocalTimestamp);
    timeRunData->preLocalTimestamp = GET_SYS_MILLIS();
    return timeRunData->preNetTimestamp;
}

static long long getTimestamp(String url)
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

    m_timestamp = timeRunData->preNetTimestamp;

    return timeRunData->preNetTimestamp;
}

static void UpdateTime_RTC(long long timestamp)
{
    struct Time t;
    timeRunData->g_rtc.setTime(timestamp / 1000);
    t.year = timeRunData->g_rtc.getYear();
    t.month = timeRunData->g_rtc.getMonth() + 1;
    t.day = timeRunData->g_rtc.getDay();
    t.hour = timeRunData->g_rtc.getHour(true);
    t.minute = timeRunData->g_rtc.getMinute();
    t.second = timeRunData->g_rtc.getSecond();
    t.weekday = timeRunData->g_rtc.getDayofWeek();
    // Serial.printf("time : %d-%d-%d\n",t.hour, t.minute, t.second);
    appTimeUiDisplay(t, LV_SCR_LOAD_ANIM_NONE);
}

static int timeInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appTimeUiInit();
    readTimeCfg(&timeCfg);
    timeRunData = (TimeRunData *)calloc(1, sizeof(TimeRunData));
    timeRunData->lastUpdate = 0;
    timeRunData->forceUpdate = 1;
    timeRunData->currPage = 0;
    timeRunData->preNetTimestamp = 1690848000000;
    timeRunData->errorNetTimestamp = 0;
    timeRunData->preLocalTimestamp = GET_SYS_MILLIS();
    return 0;
}

static void timeRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->action == ACT_BACK)
    {
        appCenter->exitApp();
        return;
    }

    // appTimeUiDisplay(timeRunData->timInfo, animType);

    if (timeRunData->forceUpdate || doDelayMillisTime(timeCfg.updateInterval, &timeRunData->lastUpdate, false))
    {
        appCenter->sendMsgTo(APP_TIME_NAME, CTRL_NAME, APP_MESSAGE_WIFI_CONN, NULL, NULL);
    }
    else if (GET_SYS_MILLIS() - timeRunData->preLocalTimestamp > 400)
    {
        UpdateTime_RTC(getTimestamp());
    }

    timeRunData->forceUpdate = 0;

    delay(30);
}

static void timeBackground(AppCenter *appCenter, const Action *action)
{
}

static int timeExit(void *param)
{
    appTimeUiDelete();

    // if (timeRunData->xReturned_task_task_update == pdPASS)
    // {
    //     vTaskDelete(timeRunData->xHandle_task_task_update);
    // }

    if (timeRunData != NULL)
    {
        free(timeRunData);
        timeRunData = NULL;
    }
    return 0;
}

static void timeOnMessage(const char *from, const char *to,
                          AppMsgType type, void *msg, void *info)
{
    if (type == APP_MESSAGE_WIFI_CONN)
    {
        long long timestamp = getTimestamp(TIME_API);
        UpdateTime_RTC(timestamp);
    }
    else if (type == APP_MESSAGE_GET_PARAM)
    {
        if (strcmp((char *)info, "updateInterval") == 0)
        {
            snprintf((char *)info, 32, "%lu", timeCfg.updateInterval);
        }
        else
        {
            snprintf((char *)info, 32, "%s", "NULL");
        }
    }
    else if (type == APP_MESSAGE_SET_PARAM)
    {
        char *paramValue = (char *)info;
        if (strcmp((char *)msg, "updateInterval") == 0)
        {
            timeCfg.updateInterval = atol(paramValue);
        }
    }
    else if (type == APP_MESSAGE_READ_CFG)
    {
        readTimeCfg(&timeCfg);
    }
    else if (type == APP_MESSAGE_WRITE_CFG)
    {
        writeTimeCfg(&timeCfg);
    }
}

App timeApp = {
    APP_TIME_NAME,
    &icon_time,
    APP_TIME_INFO,
    timeInit,
    timeRoutine,
    timeBackground,
    timeExit,
    timeOnMessage};