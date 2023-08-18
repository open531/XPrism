#include "app_navi.h"
#include "app_navi_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"

#define APP_NAVI_NAME "导航"
#define APP_NAVI_INFO "导航信息"

#define NAVI_CONFIG_PATH "/navi.cfg"

extern WebServer server;

struct NaviCfg
{
};

static void writeNaviCfg(NaviCfg *cfg)
{
}

static void readNaviCfg(NaviCfg *cfg)
{
}

struct NaviAppRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    int currPage;

    // BaseType_t xReturned_task_task_update; // 更新数据的异步任务
    // TaskHandle_t xHandle_task_task_update; // 更新数据的异步任务

    Navi navInfo;
    double currLat;
    double currLon;
};

static NaviCfg naviCfg;
static NaviAppRunData *naviAppRunData = NULL;

void decodePath(char *encoded)
{
    int len = strlen(encoded);
    int index = 0;
    double lat = 0;
    double lon = 0;
    int i = 0;

    while (index < len)
    {
        int b;
        int shift = 0;
        int result = 0;
        do
        {
            b = encoded[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);
        int dlat = ((result & 1) ? ~(result >> 1) : (result >> 1));
        lat += dlat;

        shift = 0;
        result = 0;
        do
        {
            b = encoded[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);
        int dlon = ((result & 1) ? ~(result >> 1) : (result >> 1));
        lon += dlon;

        naviAppRunData->navInfo.pointLat[i] = lat * 1e-5;
        naviAppRunData->navInfo.pointLon[i] = lon * 1e-5;
        i++;
    }
}

void handleNavi()
{
    naviAppRunData->navInfo.pointEncoded = (char *)malloc(server.arg("pointEncoded").length() + 1);
    strcpy(naviAppRunData->navInfo.pointEncoded, server.arg("pointEncoded").c_str());
    decodePath(naviAppRunData->navInfo.pointEncoded);
    naviAppRunData->navInfo.instructionLength = server.arg("instructionLength").toInt();
    naviAppRunData->navInfo.instructionText = (char **)malloc(naviAppRunData->navInfo.instructionLength * sizeof(char *));
    naviAppRunData->navInfo.instructionSign = (int *)malloc(naviAppRunData->navInfo.instructionLength * sizeof(int));
    naviAppRunData->navInfo.instructionIntervalBegin = (int *)malloc(naviAppRunData->navInfo.instructionLength * sizeof(int));
    naviAppRunData->navInfo.instructionIntervalEnd = (int *)malloc(naviAppRunData->navInfo.instructionLength * sizeof(int));
    String instructionText = server.arg("instructionText");
    String instructionSign = server.arg("instructionSign");
    String instructionIntervalBegin = server.arg("instructionIntervalBegin");
    String instructionIntervalEnd = server.arg("instructionIntervalEnd");

    int pos = instructionText.indexOf(',');
    int i = 0;
    while (pos != -1)
    {
        String text = instructionText.substring(0, pos);
        instructionText.remove(0, pos + 1);
        naviAppRunData->navInfo.instructionText[i] = (char *)malloc(text.length() + 1);
        strcpy(naviAppRunData->navInfo.instructionText[i], text.c_str());
        pos = instructionText.indexOf(',');
        i++;
    }
    strcpy(naviAppRunData->navInfo.instructionText[i], instructionText.c_str());

    pos = instructionSign.indexOf(',');
    i = 0;
    while (pos != -1)
    {
        String sign = instructionSign.substring(0, pos);
        instructionSign.remove(0, pos + 1);
        naviAppRunData->navInfo.instructionSign[i] = sign.toInt();
        pos = instructionSign.indexOf(',');
        i++;
    }
    naviAppRunData->navInfo.instructionSign[i] = instructionSign.toInt();

    pos = instructionIntervalBegin.indexOf(',');
    i = 0;
    while (pos != -1)
    {
        String begin = instructionIntervalBegin.substring(0, pos);
        instructionIntervalBegin.remove(0, pos + 1);
        naviAppRunData->navInfo.instructionIntervalBegin[i] = begin.toInt();
        pos = instructionIntervalBegin.indexOf(',');
        i++;
    }
    naviAppRunData->navInfo.instructionIntervalBegin[i] = instructionIntervalBegin.toInt();

    pos = instructionIntervalEnd.indexOf(',');
    i = 0;
    while (pos != -1)
    {
        String end = instructionIntervalEnd.substring(0, pos);
        instructionIntervalEnd.remove(0, pos + 1);
        naviAppRunData->navInfo.instructionIntervalEnd[i] = end.toInt();
        pos = instructionIntervalEnd.indexOf(',');
        i++;
    }
    naviAppRunData->navInfo.instructionIntervalEnd[i] = instructionIntervalEnd.toInt();

    String message = "Navi\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(200, "text/plain", message);
}

static void getRoute()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
    serverSetup();
}

static int naviInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appNaviUiInit();
    naviAppRunData = (NaviAppRunData *)malloc(sizeof(NaviAppRunData));
    memset((char *)&naviAppRunData->navInfo, 0, sizeof(Navi));
    naviAppRunData->currPage = 0;
    naviAppRunData->currLat = 0;
    naviAppRunData->currLon = 0;
    naviAppRunData->lastUpdate = 0;
    naviAppRunData->forceUpdate = 1;
    naviAppRunData->navInfo.currLat = 0;
    naviAppRunData->navInfo.currLon = 0;
    naviAppRunData->navInfo.currAlt = 0;
    naviAppRunData->navInfo.currSpeed = 0;
    naviAppRunData->navInfo.currSatellites = 0;
    readNaviCfg(&naviCfg);
    return 0;
}

static void naviRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }

    appNaviUiDisplay(naviAppRunData->navInfo, animType);
    delay(30);
}

static void naviBackground(AppCenter *appCenter, const Action *action)
{
}

static int naviExit(void *param)
{
    appNaviUiDelete();

    if (naviAppRunData != NULL)
    {
        free(naviAppRunData);
        naviAppRunData = NULL;
    }

    return 0;
}

static void naviOnMessage(const char *from, const char *to,
                          AppMsgType type, void *msg, void *info)
{
}

App naviApp = {
    APP_NAVI_NAME,
    &icon_map,
    APP_NAVI_INFO,
    naviInit,
    naviRoutine,
    naviBackground,
    naviExit,
    naviOnMessage};