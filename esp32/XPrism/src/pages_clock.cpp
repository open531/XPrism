#include "pages_clock_ui.h"
#include "system.h"
#include "pages_main.h"
#include "ArduinoJson.h"
#include "actions.h"
#include <esp32-hal-timer.h>
#include <map>
#include <String>
#include "ESP32Time.h"

#include "lvgl.h"
#define CLOCK_APP_NAME "时钟"
#define TIME_CONFIG_PATH "/clock.json"

struct ClockRunData
{
    unsigned long forceUpdate;
    unsigned long lastUpdate;
    unsigned long updateInterval;
    unsigned int currentPage;
};

ESP32Time esp32time;

unsigned long MyTestTimer;

static int RTC_Hour;
static int RTC_Minute;
static int RTC_Second;

static int RTR_Hour;
static int RTR_Minute;
static int RTR_Second;

static ClockRunData *run_data = NULL;

static ClockRunData *data;

void initialdata()
{
    data->lastUpdate = 0;
    data->updateInterval = 100;
}

unsigned long StopStart = 0, StopFinish = 0;

// easy to use helper-function for non-blocking timing
boolean TimePeriodIsOver(unsigned long &periodStartTime, unsigned long TimePeriod)
{

    unsigned long currentMillis = millis();
    if (data->forceUpdate == 0)
    {
        return false;
    }
    if (data->forceUpdate == 1)
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

void SoftRTC()
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

void Second_Clear()
{
    RTC_Hour = 0;
    RTC_Minute = 0;
    RTC_Second = 0;
}

void ReSecond_Clear()
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
        second_clock->Hour = RTC_Hour;
        second_clock->Minute = RTC_Minute;
        second_clock->Second = RTC_Second;
        display_clock(*second_clock);
    }
}

void second_clock() // 秒表
{
    Clock *second_clock;
    task_update_second(second_clock);
}

// 需要补充：
Clock initialRTR() // 倒计时初始化
{
}

unsigned long RTR_Timer;

void SoftRTR()
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

static void task_update_resecond(Clock *resecond_clock, Clock *set_clock)
{
    initialdata();
    while (1)
    {
        SoftRTR();
        resecond_clock->Hour = RTR_Hour;
        resecond_clock->Minute = RTR_Minute;
        resecond_clock->Second = RTR_Second;
        display_clock(*resecond_clock);
    }
}

void resecond_clock() // 计时器
{
    Clock *resecond_clock, *set_clock;
    task_update_resecond(resecond_clock, set_clock);

    set_clock->initialClock();
}

static void ClockProcess(XPages *sys, const Action *action)
{
    data->forceUpdate = 0;
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    if (action->type == BACK)
    {
        sys->app_exit();
        return;
    }
    else if (action->type == FORWARD)
    {
        if (data->currentPage == 0)
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
        else if (data->currentPage == 1)
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
        delay(500);
    }
    else if (action->type == LEFT)
    {
        if (data->currentPage == 0)
        {
            Second_Clear();
            data->forceUpdate == 0;
        }
        else if (data->currentPage == 1)
        {
            ReSecond_Clear();
            data->forceUpdate == 0;
        }
        data->currentPage = (data->currentPage + 1) % 2;
    }
    else if (action->type == RIGHT)
    {
        if (data->currentPage == 0)
        {
            Second_Clear();
            data->forceUpdate == 0;
        }
        else if (data->currentPage == 1)
        {
            ReSecond_Clear();
            data->forceUpdate == 0;
        }
        data->currentPage = (data->currentPage + 1) % 2;
    }
    else
    {
        return;
    }

    if (data->currentPage == 0)
    {
        second_clock();
    }
    else
    {
        resecond_clock();
    }
}