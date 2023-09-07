#include "app_clock.h"
#include "app_clock_ui.h"
#include "common.h"
#include "app_center.h"
#include "icons.h"
#include "ArduinoJson.h"
#include "ESP32Time.h"

#define APP_CLOCK_NAME "时钟"
#define APP_CLOCK_INFO "显示时钟信息"
#define APP_CLOCK_PAGE_SIZE 2

#define CLOCK_CFG_PATH "/clock.cfg"
#define TIMEZERO_OFFSIZE (28800000)
#define GET_SYS_MILLIS xTaskGetTickCount // 获取系统毫秒数

struct ClockCfg
{
};

struct ClockRunData
{
    unsigned int currPage;
    boolean stopwatchRunning;
    unsigned long stopwatchBegin;
    unsigned long stopwatchEnd;
    unsigned long stopwatchTemp;
    long long preLocalTimestamp; // 上一次的本地机器时间戳
};

static void writeClockCfg(ClockCfg *cfg)
{
}

static void readClockCfg(ClockCfg *cfg)
{
}

static ClockCfg clockCfg;
static ClockRunData *clockRunData = NULL;

static long long getTimestamp()
{
    clockRunData->preLocalTimestamp = GET_SYS_MILLIS();
    return clockRunData->preLocalTimestamp;
}

static void stopwatchStart()
{
    clockRunData->stopwatchBegin = getTimestamp();
    Serial.println("stopwatchStart");
    Serial.println(clockRunData->stopwatchTemp);
}

static void stopwatchStop()
{
    clockRunData->stopwatchEnd = getTimestamp();
    clockRunData->stopwatchTemp += clockRunData->stopwatchEnd - clockRunData->stopwatchBegin;
    Serial.println("stopwatchStop");
    Serial.println(clockRunData->stopwatchTemp);
}

static void updateStopwatch()
{
    double stopwatchInfo;
    if (clockRunData->stopwatchRunning)
    {
        stopwatchInfo = clockRunData->stopwatchTemp + getTimestamp() - clockRunData->stopwatchBegin;
    }
    else
    {
        stopwatchInfo = clockRunData->stopwatchTemp;
    }
    appClockUiDisplayStopwatch(stopwatchInfo, clockRunData->stopwatchTemp, LV_SCR_LOAD_ANIM_NONE);
}

static int clockInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appClockUiInit();
    readClockCfg(&clockCfg);
    clockRunData = (ClockRunData *)calloc(1, sizeof(ClockRunData));
    clockRunData->currPage = 0;
    clockRunData->stopwatchRunning = false;
    clockRunData->stopwatchBegin = 0;
    clockRunData->stopwatchEnd = 0;
    clockRunData->stopwatchTemp = 0;
    clockRunData->preLocalTimestamp = GET_SYS_MILLIS();
    Serial.println("clockInit");
    return 0;
}

static void clockRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;

    if (action->action == ACT_BACK)
    {
        appCenter->exitApp();
        return;
    }
    else if (action->action == ACT_FORWARD)
    {
        if (clockRunData->stopwatchRunning)
        {
            clockRunData->stopwatchRunning = false;
            stopwatchStop();
        }
        else
        {
            clockRunData->stopwatchRunning = true;
            stopwatchStart();
        }
    }

    if (GET_SYS_MILLIS() - clockRunData->preLocalTimestamp > 400)
    {
        updateStopwatch();
    }
}

static void clockBackground(AppCenter *appCenter, const Action *action)
{
}

static int clockExit(void *param)
{
    appClockUiDelete();

    if (clockRunData != NULL)
    {
        free(clockRunData);
        clockRunData = NULL;
    }
    return 0;
}

static void clockOnMessage(const char *from, const char *to,
                           AppMsgType type, void *msg, void *info)
{
    if (type == APP_MESSAGE_WIFI_CONN)
    {
    }
    else if (type == APP_MESSAGE_GET_PARAM)
    {
    }
    else if (type == APP_MESSAGE_SET_PARAM)
    {
    }
    else if (type == APP_MESSAGE_READ_CFG)
    {
    }
    else if (type == APP_MESSAGE_WRITE_CFG)
    {
    }
}

App clockApp = {
    APP_CLOCK_NAME,
    &icon_clock,
    APP_CLOCK_INFO,
    clockInit,
    clockRoutine,
    clockBackground,
    clockExit,
    clockOnMessage};