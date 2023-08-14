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

    long long preNetTimestamp= 1690848000000;;
    long long errorNetTimestamp = 0; // 网络到显示过程中的时间误差
    long long preLocalTimestamp= GET_SYS_MILLIS();
    ESP32Time g_rtc;
};
static ClockCfg clockCfg;
static ClockRunData *clockRunData = NULL;

unsigned long MyTestTimer;

long long preLocalTimestamp; // 上一次的本地机器时间戳

Clock *RTC,*RTR,*RTR0,*record;

static void initialdata()
{
    clockRunData->lastUpdate = 0;
    clockCfg.updateInterval = 100;
    clockRunData->forceUpdate = 0;
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

static long long get_timestamp()
{
    clockRunData->preNetTimestamp = clockRunData->preNetTimestamp + (GET_SYS_MILLIS() - clockRunData->preLocalTimestamp);
    clockRunData->preLocalTimestamp = GET_SYS_MILLIS();
    return clockRunData->preNetTimestamp;
}



static void UpdateClock(long long timestamp)
{
    struct Clock clo;
    clockRunData->g_rtc.setTime(timestamp / 1000);

    clo.hour = clockRunData->g_rtc.getHour(true);
    clo.minute = clockRunData->g_rtc.getMinute();
    clo.second = clockRunData->g_rtc.getSecond();

    // Serial.printf("time : %d-%d-%d\n",t.hour, t.minute, t.second);
    appClockUiDisplayBasic(clo, LV_SCR_LOAD_ANIM_NONE);
}

static void ClockClear(int runway)
{
    if(runway==1 && RTC!=NULL)
    {
        RTC=NULL;
    }
    else if(runway==-1 && RTR!=NULL)
    {
        RTR=NULL;
    }
}

// 需要补充：
// Clock initialRTR0(Clock *setClock) // 倒计时初始化
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


static void SoftClock(int runway)
{   if(runway==1)
    {
        clockRunData->g_rtc.setTime(get_timestamp() / 1000);
        RTC->hour = clockRunData->g_rtc.getHour(true)+record->hour;
        RTC->minute = clockRunData->g_rtc.getMinute()+record->minute;
        RTC->second = clockRunData->g_rtc.getSecond()+record->second;

    }

    else if(runway==-1)
    {
        clockRunData->g_rtc.setTime(get_timestamp() / 1000);
        RTR->hour = RTR0->hour-clockRunData->g_rtc.getHour(true)-record->hour;
        RTR->minute = RTR0->minute-clockRunData->g_rtc.getMinute()-record->minute;
        RTR->second = RTR0->second-clockRunData->g_rtc.getSecond()-record->second;
    }
}

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
    initialdata();
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }

    else if(action->active==BTN_FORWARD)
    {
        clockRunData->forceUpdate=1-clockRunData->forceUpdate;
        if(clockRunData->currPage==0)
        {
            if(clockRunData->forceUpdate==0)
            {
                record->hour=RTC->hour;
                record->minute=RTC->minute;
                record->second=RTC->second;
            }
            else if(clockRunData->forceUpdate==1)
            {
                clockRunData->preLocalTimestamp = GET_SYS_MILLIS();
            }
        }
        else if(clockRunData->currPage==1)
        {
            if(clockRunData->forceUpdate==0)
            {
                record->hour=RTR->hour;
                record->minute=RTR->minute;
                record->second=RTR->second;
            }
            else if(clockRunData->forceUpdate==1)
            {
                clockRunData->preLocalTimestamp = GET_SYS_MILLIS();
            }
        }
    }
    else if(action->active==BTN_LEFT)
    {
        clockRunData->currPage=1-clockRunData->currPage;
        ClockClear(1-clockRunData->currPage);
    }
    else if(action->active==BTN_RIGHT)
    {
        clockRunData->currPage=1-clockRunData->currPage;
        ClockClear(1-clockRunData->currPage);
    }

    if (clockRunData->currPage == 0)
    {
        while (1)
    {
        SoftClock(1);
        appClockUiDisplayBasic(*RTC,LV_SCR_LOAD_ANIM_NONE);
    }
    }
    else if (clockRunData->currPage == 1)
    {
        while(1)
        {
            SoftClock(-1);
            appClockUiDisplayBasic(*RTR,LV_SCR_LOAD_ANIM_NONE);
        }
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
    ClockClear(1);
    ClockClear(-1);
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