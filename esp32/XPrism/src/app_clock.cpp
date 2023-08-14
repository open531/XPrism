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
    unsigned long updateInterval;
};

struct ClockRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    unsigned int currPage;
    BaseType_t xReturned_task_task_update;
    TaskHandle_t xHandle_task_task_update;
    Clock cloInfo;
    unsigned int updateInterval;
};
static ClockCfg clockCfg;
static ClockRunData *clockRunData = NULL;

unsigned long MyTestTimer;

static int RTC_Hour;
static int RTC_Minute;
static int RTC_Second;

static int RTR_Hour = 1;
static int RTR_Minute = 59;
static int RTR_Second = 59;

static ClockRunData *run_data = NULL;

static ClockRunData *data;

static void initialdata()
{
    data->lastUpdate = 0;
    clockCfg.updateInterval = 100;
    data->forceUpdate = 0;
}

static void writeClockCfg(ClockCfg *clockCfg)
{
    char tmp[16];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    memset(tmp, 0, 16);
    snprintf(tmp, 16, "%lu\n", clockCfg->updateInterval);
    w_data += tmp;
    m_flashCfg.writeFile(CLOCK_CFG_PATH, w_data.c_str());
}

static void readClockCfg(ClockCfg *clockCfg)
{
    char info[128] = {0};
    uint16_t size = m_flashCfg.readFile(CLOCK_CFG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        clockCfg->updateInterval = 900000; // 时间更新的时间间隔900000(900s)
        writeClockCfg(clockCfg);
    }
    else
    {
        // 解析数据
        char *param[1] = {0};
        analyseParam(info, 1, param);
        clockCfg->updateInterval = atol(param[0]);
    }
}

unsigned long StopStart = 0, StopFinish = 0;

// easy to use helper-function for non-blocking timing
boolean TimePeriodIsOver(unsigned long &periodStartTime,
                         unsigned long TimePeriod)
{

    unsigned long currentMillis = millis();
    if (data->forceUpdate == 0)
    {
        return false;
    }
    else
    {
        currentMillis = currentMillis - (StopFinish - StopStart);
        StopStart = StopFinish;
        if (currentMillis - periodStartTime >= TimePeriod)
        {
            periodStartTime = currentMillis; // set new expireTime
            return true;                     // more time than TimePeriod) has elapsed since last time if-condition was true
        }
        else
            return false; // not expired
    }
}

unsigned long RTC_Timer;

static void SoftRTC()
{ // 计时
    if (TimePeriodIsOver(RTC_Timer, 1000))
    {                 // once every 100 milliseconds
        RTC_Second++; // increase 1/10th-seconds counter

        if (RTC_Second == 60)
        {                   // if seconds counter reaches 60
            RTC_Minute++;   // increase minutes counter by 1
            RTC_Second = 0; // reset seconds counter to zero
        }

        if (RTC_Minute == 60)
        {                   // if minutes counter reaches 60
            RTC_Hour++;     // increase hour counter by 1
            RTC_Minute = 0; // reset minutes counter to zero
        }

        if (RTC_Hour == 24)
        {                 // if hours counter reaches 24
            RTC_Hour = 0; // reset hours counter to zero
        }
    }
}

static void Second_Clear()
{
    RTC_Hour = 0;
    RTC_Minute = 0;
    RTC_Second = 0;
}

static void ReSecond_Clear()
{
    RTR_Hour = 0;
    RTR_Minute = 0;
    RTR_Second = 0;
}

static void task_update_second(Clock *second_clock)
{
    initialdata();

    while (1)
    {
        SoftRTC();
        second_clock->hour = RTC_Hour;
        second_clock->minute = RTC_Minute;
        second_clock->second = RTC_Second;
        appClockUiDisplayBasic(*second_clock);
    }
}

static void second_clock() // 秒表
{
    Clock *second_clock;
    task_update_second(second_clock);
}

// 需要补充：
// Clock initialRTR(Clock *setClock) // 倒计时初始化
// {
//    if (clockRunData->currPage==0)
//    {
//       appClockUiInit();
//        appClockUiDisplayBasic(clockRunData->cloInfo);
//    }

//    else if (clockRunData->currPage==1)
//    {
//        RTR_Hour=setClock->hour;
//        RTR_Minute=setClock->minute;
//        RTR_Second=setClock->second;
//    }
// }

unsigned long RTR_Timer;

static void SoftRTR()
{ // 倒计时
    if (TimePeriodIsOver(RTR_Timer, 1000))
    {                 // once every 100 milliseconds
        RTR_Second--; // increase 1/10th-seconds counter

        if (RTR_Second <= 0 && RTR_Minute > 0 && RTR_Hour > 0)
        {                    // if seconds counter reaches 60
            RTR_Minute--;    // increase minutes counter by 1
            RTR_Second = 60; // reset seconds counter to zero
        }

        if (RTR_Minute <= 0 && RTR_Hour > 0)
        {                    // if minutes counter reaches 60
            RTR_Hour--;      // increase hour counter by 1
            RTR_Minute = 60; // reset minutes counter to zero
        }

        if (RTR_Hour <= 0)
        {                 // if hours counter reaches 24
            RTR_Hour = 0; // reset hours counter to zero
        }
    }
}

static void task_update_resecond(Clock *resecond_clock)
{
    initialdata();
    while (1)
    {
        SoftRTR();
        resecond_clock->hour = RTR_Hour;
        resecond_clock->minute = RTR_Minute;
        resecond_clock->second = RTR_Second;
        appClockUiDisplayBasic(*resecond_clock);
    }
}

static void resecond_clock() // 计时器
{
    Clock *resecond_clock;
    task_update_resecond(resecond_clock);

    // 需要初始化时间
}
//

static int clockInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appClockUiInit();
    readClockCfg(&clockCfg);
    clockRunData = (ClockRunData *)calloc(1, sizeof(ClockRunData));
    clockRunData->lastUpdate = 0;
    clockRunData->forceUpdate = 1;
    clockRunData->currPage = 0;

    return 0;
}

static void clockRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }

    Clock *clock;

    if (data->currPage == 0)
    {
        task_update_second(clock);
    }
    else if (data->currPage == 1)
    {
        task_update_resecond(clock);
    }
    else
    {
        appCenter->send_to(APP_CLOCK_NAME, CTRL_NAME, APP_MESSAGE_WIFI_CONN, NULL, NULL);
    }

    clockRunData->forceUpdate = 0;

    delay(30);
}

static void clockBackground(AppCenter *appCenter, const Action *action)
{
}

static int clockExit(void *param)
{
    appClockUiDelete();
    if (clockRunData->xReturned_task_task_update == pdPASS)
    {
        vTaskDelete(clockRunData->xHandle_task_task_update);
    }

    if (clockRunData != NULL)
    {
        free(clockRunData);
        clockRunData = NULL;
    }
    return 0;
}

App clockApp = {
    APP_CLOCK_NAME,
    &icon_clock,
    APP_CLOCK_INFO,
    clockInit,
    clockRoutine,
    clockBackground,
    clockExit,
};