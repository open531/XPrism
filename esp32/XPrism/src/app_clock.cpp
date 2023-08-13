#include "app_clock.h"
#include "app_clock_ui.h"
#include "system.h"
#include "app_center.h"
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
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    unsigned int currPage;
    BaseType_t xReturn;
    TaskHandle_t xHandle;
    Clock cloInfo;
    unsigned int updateInterval;
};

static ClockRunData *clockRunData = NULL;

unsigned long MyTestTimer;

static int RTC_Hour;
static int RTC_Minute;
static int RTC_Second;

static int RTR_Hour=1;
static int RTR_Minute=59;
static int RTR_Second=59;

static ClockRunData *run_data = NULL;

static ClockRunData *data;

static void initialdata()
{
    data->lastUpdate = 0;
    data->updateInterval = 100;
    data->forceUpdate=0;
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

    //初始化时间
}
//

static void ClockTask(void *pvParameters)
{
    ClockRunData *data = (ClockRunData *)pvParameters;
    Clock *second_clock,*resecond_clock;
    
    if(data->currPage==0)
    {
        task_update_second(second_clock);
    }
    else if(data->currPage==1)
    {
        task_update_resecond(resecond_clock);
    }

}


static int clockInit(AppCenter *appCenter)
{
    appClockUiInit();
    return 0;
}

static void clickBack()
{
    appCenter->appExit();
}


static void clickForward()
{
     if (data->currPage == 0)
        {
            if (data->forceUpdate == 0)
            {
                data->forceUpdate = 1;
                StopFinish = millis();
            }
            else if (data->forceUpdate == 1)
            {
                data->forceUpdate = 0;
                StopStart = millis();
            }
        }
        else if (data->currPage == 1)
        {
            if (data->forceUpdate == 0)
            {
                data->forceUpdate = 1;
                StopFinish = millis();
            }
            else if (data->forceUpdate == 1)
            {
                data->forceUpdate = 0;
                StopStart = millis();
            }
        }
        delay(100);
}

static void clickLeft()
{
    if (data->currPage == 0)
        {
            Second_Clear();
            data->forceUpdate == 0;
        }
        else if (data->currPage == 1)
        {
            ReSecond_Clear();
            data->forceUpdate == 0;
        }
    clockRunData->currPage = (clockRunData->currPage + APP_CLOCK_PAGE_SIZE - 1) % APP_CLOCK_PAGE_SIZE;

    
}

static void clickRight()
{
    if (data->currPage == 0)
        {
            Second_Clear();
            data->forceUpdate == 0;
        }
        else if (data->currPage == 1)
        {
            ReSecond_Clear();
            data->forceUpdate == 0;
        }
    clockRunData->currPage = (clockRunData->currPage + 1) % APP_CLOCK_PAGE_SIZE;
    
}

static void clockRoutine(AppCenter *appCenter)
{
    m_button.routine();
    m_button.attachClick(BTN_BACK, clickBack);
    
}

static void clockBackground(AppCenter *appCenter)
{
}

static int clockExit(AppCenter *appCenter)
{
    appClockUiDelete();
    if (clockRunData != NULL)
    {
        vTaskDelete(clockRunData->xHandle);
        free(clockRunData);
        clockRunData = NULL;
    }
    return 0;
}



App ClockApp = {
    APP_CLOCK_NAME,
    &icon_clock,
    APP_CLOCK_INFO,
    clockInit,
    clockRoutine,
    clockBackground,
    clockExit,
    };