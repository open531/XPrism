#include "app_sport.h"
#include "app_sport_ui.h"
#include "icons.h"
#include "app_center.h"

#define APP_SPORT_NAME "运动"
#define APP_SPORT_INFO "运动信息"

#define SPORT_CONFIG_PATH "/sport.cfg"

struct SportCfg
{
};

static void writeSportCfg(SportCfg *cfg)
{
}

static void readSportCfg(SportCfg *cfg)
{
}

struct SportAppRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    int currPage;

    Sport sportInfo;
};

static SportCfg sportCfg;
static SportAppRunData *sportAppRunData = NULL;

void updateSport()
{
    sportAppRunData->sportInfo.gpsLat = m_gps.getLatitude();
    sportAppRunData->sportInfo.gpsLon = m_gps.getLongitude();
    sportAppRunData->sportInfo.gpsAlt = m_gps.getAltitude();
    sportAppRunData->sportInfo.gpsSpd = m_gps.getSpeed();
    sportAppRunData->sportInfo.gpsSat = m_gps.getSatellites();
}

static int sportInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appSportUiInit();
    sportAppRunData = (SportAppRunData *)malloc(sizeof(SportAppRunData));
    sportAppRunData->currPage = 0;
    sportAppRunData->forceUpdate = 1;
    sportAppRunData->lastUpdate = 0;
    sportAppRunData->sportInfo.gpsLat = 0;
    sportAppRunData->sportInfo.gpsLon = 0;
    sportAppRunData->sportInfo.gpsAlt = 0;
    sportAppRunData->sportInfo.gpsSpd = 0;
    sportAppRunData->sportInfo.gpsSat = 0;
    readSportCfg(&sportCfg);
    return 0;
}

static void sportRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }
    else if (action->active == BTN_FORWARD)
    {
        sportAppRunData->forceUpdate = 1;
    }

    if (sportAppRunData->forceUpdate || millis() - sportAppRunData->lastUpdate > 2000)
    {
        sportAppRunData->lastUpdate = millis();
        updateSport();
    }

    appSportUiDisplay(sportAppRunData->sportInfo, animType);
    delay(100);
}

static void sportBackground(AppCenter *appCenter, const Action *action)
{
}

static int sportExit(void *param)
{
    appSportUiDelete();

    if (sportAppRunData != NULL)
    {
        free(sportAppRunData);
        sportAppRunData = NULL;
    }

    return 0;
}

static void sportOnMessage(const char *from, const char *to,
                           AppMsgType type, void *msg, void *info)
{
}

App sportApp = {
    APP_SPORT_NAME,
    &icon_sport,
    APP_SPORT_INFO,
    sportInit,
    sportRoutine,
    sportBackground,
    sportExit,
    sportOnMessage,
};