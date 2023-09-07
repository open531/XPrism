#include "app_calendar.h"
#include "app_calendar_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"
#include "ESP32Time.h"

#define APP_CALENDAR_NAME "日历"
#define APP_CALENDAR_INFO "日历"

#define CALENDAR_CONFIG_PATH "/calendar.cfg"

struct CalendarCfg
{
};

static void writeCalendarCfg(CalendarCfg *cfg)
{
}

static void readCalendarCfg(CalendarCfg *cfg)
{
}

struct CalendarAppRunData
{
    Date today;
    Date currDate;
    Date *highlight;
};

static CalendarCfg calendarCfg;
static CalendarAppRunData *calendarAppRunData = NULL;

static int calendarInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appCalendarUiInit();
    calendarAppRunData = (CalendarAppRunData *)malloc(sizeof(CalendarAppRunData));
    if (m_timestamp != 0)
    {
        ESP32Time g_rtc;
        g_rtc.setTime(m_timestamp / 1000);
        calendarAppRunData->today.year = g_rtc.getYear();
        calendarAppRunData->today.month = g_rtc.getMonth() + 1;
        calendarAppRunData->today.day = g_rtc.getDay();
    }
    else
    {
        calendarAppRunData->today.year = 2023;
        calendarAppRunData->today.month = 8;
        calendarAppRunData->today.day = 1;
    }
    calendarAppRunData->currDate.year = calendarAppRunData->today.year;
    calendarAppRunData->currDate.month = calendarAppRunData->today.month;
    calendarAppRunData->currDate.day = 1;
    calendarAppRunData->highlight = NULL;
    readCalendarCfg(&calendarCfg);
    return 0;
}

static void calendarRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
    if (action->action == ACT_BACK)
    {
        appCenter->exitApp();
        return;
    }
    else if (action->action == ACT_LEFT)
    {
        calendarAppRunData->currDate.month--;
        if (calendarAppRunData->currDate.month < 1)
        {
            calendarAppRunData->currDate.month = 12;
            calendarAppRunData->currDate.year--;
        }
    }
    else if (action->action == ACT_RIGHT)
    {
        calendarAppRunData->currDate.month++;
        if (calendarAppRunData->currDate.month > 12)
        {
            calendarAppRunData->currDate.month = 1;
            calendarAppRunData->currDate.year++;
        }
    }

    if (calendarAppRunData->currDate.year < 1900)
    {
        calendarAppRunData->currDate.year = 1900;
        calendarAppRunData->currDate.month = 1;
    }
    else if (calendarAppRunData->currDate.year > 2099)
    {
        calendarAppRunData->currDate.year = 2099;
        calendarAppRunData->currDate.month = 12;
    }

    appCalendarUiDisplay(calendarAppRunData->today,
                         calendarAppRunData->currDate,
                         calendarAppRunData->highlight, animType);
    delay(100);
}

static void calendarBackground(AppCenter *appCenter, const Action *action)
{
}

static int calendarExit(void *param)
{
    appCalendarUiDelete();

    if (calendarAppRunData != NULL)
    {
        free(calendarAppRunData);
        calendarAppRunData = NULL;
    }

    return 0;
}

static void calendarOnMessage(const char *from, const char *to,
                              AppMsgType type, void *msg, void *info)
{
}

App calendarApp = {
    APP_CALENDAR_NAME,
    &icon_calendar,
    APP_CALENDAR_INFO,
    calendarInit,
    calendarRoutine,
    calendarBackground,
    calendarExit,
    calendarOnMessage,
};