#include "app_info.h"
#include "app_info_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"

#define APP_INFO_NAME "信息"
#define APP_INFO_INFO "信息"

#define INFO_CONFIG_PATH "/info.cfg"

struct InfoCfg
{
};

static void writeInfoCfg(InfoCfg *cfg)
{
}

static void readInfoCfg(InfoCfg *cfg)
{
}

struct InfoAppRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    int currPage;

    Info infoInfo;
};

static InfoCfg infoCfg;
static InfoAppRunData *infoAppRunData = NULL;

void updateInfo()
{
    IPAddress ip = WiFi.localIP();
    infoAppRunData->infoInfo.ip[0] = ip[0];
    infoAppRunData->infoInfo.ip[1] = ip[1];
    infoAppRunData->infoInfo.ip[2] = ip[2];
    infoAppRunData->infoInfo.ip[3] = ip[3];
    infoAppRunData->infoInfo.gpsLat = m_gps.getLatitude();
    infoAppRunData->infoInfo.gpsLon = m_gps.getLongitude();
    infoAppRunData->infoInfo.gpsAlt = m_gps.getAltitude();
    infoAppRunData->infoInfo.gpsSpd = m_gps.getSpeed();
    infoAppRunData->infoInfo.gpsSat = m_gps.getSatellites();
}

static int infoInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appInfoUiInit();
    infoAppRunData = (InfoAppRunData *)malloc(sizeof(InfoAppRunData));
    infoAppRunData->currPage = 0;
    infoAppRunData->forceUpdate = 1;
    infoAppRunData->lastUpdate = 0;
    infoAppRunData->infoInfo.ip[0] = 0;
    infoAppRunData->infoInfo.ip[1] = 0;
    infoAppRunData->infoInfo.ip[2] = 0;
    infoAppRunData->infoInfo.ip[3] = 0;
    readInfoCfg(&infoCfg);
    return 0;
}

static void infoRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }
    else if (action->active == BTN_FORWARD)
    {
        infoAppRunData->forceUpdate = 1;
    }

    if (infoAppRunData->forceUpdate || millis() - infoAppRunData->lastUpdate > 3000)
    {
        infoAppRunData->lastUpdate = millis();
        updateInfo();
    }

    appInfoUiDisplay(infoAppRunData->infoInfo, animType);
    delay(100);
}

static void infoBackground(AppCenter *appCenter, const Action *action)
{
}

static int infoExit(void *param)
{
    appInfoUiDelete();

    if (infoAppRunData != NULL)
    {
        free(infoAppRunData);
        infoAppRunData = NULL;
    }

    return 0;
}

static void infoOnMessage(const char *from, const char *to,
                          AppMsgType type, void *msg, void *info)
{
}

App infoApp = {
    APP_INFO_NAME,
    &icon_about,
    APP_INFO_INFO,
    infoInit,
    infoRoutine,
    infoBackground,
    infoExit,
    infoOnMessage,
};