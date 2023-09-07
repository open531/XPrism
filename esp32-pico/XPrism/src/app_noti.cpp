#include "app_noti.h"
#include "app_noti_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"
#include <ESP32Time.h>

#define APP_NOTI_NAME "通知"
#define APP_NOTI_INFO "通知信息"

#define NOTI_CONFIG_PATH "/noti.cfg"

extern WebServer server;

struct NotiCfg
{
};

static void writeNotiCfg(NotiCfg *cfg)
{
}

static void readNotiCfg(NotiCfg *cfg)
{
}

struct NotiAppRunData
{
    unsigned long lastUpdate;
    unsigned int forceUpdate;
    int currPage;
    int currNoti;
    int prevNoti;

    NotiStream notiStream;
};

static NotiCfg notiCfg;
static NotiAppRunData *notiAppRunData = NULL;

void appendNoti(Notification noti)
{
    if (notiAppRunData->notiStream.size == 0)
    {
        notiAppRunData->notiStream.notis = (Notification *)malloc(sizeof(Notification));
    }
    else
    {
        notiAppRunData->notiStream.notis = (Notification *)realloc(notiAppRunData->notiStream.notis, sizeof(Notification) * (notiAppRunData->notiStream.size + 1));
    }
    notiAppRunData->notiStream.notis[notiAppRunData->notiStream.size] = noti;
    notiAppRunData->notiStream.size++;
    notiAppRunData->prevNoti = notiAppRunData->currNoti;
    notiAppRunData->currNoti = notiAppRunData->notiStream.size - 1;
}

void removeNoti(int index)
{
    if (index < 0 || index >= notiAppRunData->notiStream.size)
    {
        return;
    }
    for (int i = index; i < notiAppRunData->notiStream.size - 1; i++)
    {
        notiAppRunData->notiStream.notis[i] = notiAppRunData->notiStream.notis[i + 1];
    }
    notiAppRunData->notiStream.size--;
    notiAppRunData->notiStream.notis = (Notification *)realloc(notiAppRunData->notiStream.notis, sizeof(Notification) * notiAppRunData->notiStream.size);
}

void removeNoti(Notification noti)
{
    for (int i = 0; i < notiAppRunData->notiStream.size; i++)
    {
        if (notiAppRunData->notiStream.notis[i].title == noti.title && notiAppRunData->notiStream.notis[i].text == noti.text && notiAppRunData->notiStream.notis[i].time == noti.time)
        {
            removeNoti(i);
            return;
        }
    }
}

void clearNoti()
{
    notiAppRunData->notiStream.size = 0;
    notiAppRunData->notiStream.notis = (Notification *)realloc(notiAppRunData->notiStream.notis, sizeof(Notification) * notiAppRunData->notiStream.size);
}

Notification getNoti(int index)
{
    if ((index < 0 || index >= notiAppRunData->notiStream.size) && index != 0)
    {
        Notification noti;
        strcpy(noti.title, NULL);
        strcpy(noti.text, NULL);
        noti.time = 0;
        return noti;
    }
    return notiAppRunData->notiStream.notis[index];
}

void handleNoti()
{
    String action = server.arg("action");
    if (action == "append")
    {
        Notification noti;
        strcpy(noti.title, (char *)server.arg("title").c_str());
        strcpy(noti.text, (char *)server.arg("text").c_str());
        noti.time = atoll(server.arg("time").c_str());
        ESP32Time rtc;
        rtc.setTime(noti.time / 1000);
        sprintf(noti.timeStr, "%02d-%02d %02d:%02d:%02d", rtc.getMonth() + 1, rtc.getDay(), rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
        appendNoti(noti);
    }
    else if (action == "remove")
    {
        Notification noti;
        strcpy(noti.title, (char *)server.arg("title").c_str());
        strcpy(noti.text, (char *)server.arg("text").c_str());
        noti.time = atoll(server.arg("time").c_str());
        removeNoti(noti);
    }
    else if (action == "clear")
    {
        clearNoti();
    }

    String message = "Noti\n\n";
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

static void serverNotiSetup()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.on("/noti", handleNoti);
    server.begin();
    Serial.println("HTTP server Noti started");
}

static void serverNotiLoop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    server.handleClient();
}

static void serverNotiStop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    server.stop();
}

static int notiInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appNotiUiInit();
    notiAppRunData = (NotiAppRunData *)malloc(sizeof(NotiAppRunData));
    notiAppRunData->lastUpdate = 0;
    notiAppRunData->forceUpdate = 0;
    notiAppRunData->currPage = 0;
    notiAppRunData->currNoti = 0;
    notiAppRunData->prevNoti = 0;
    notiAppRunData->notiStream.size = 0;
    notiAppRunData->notiStream.notis = NULL;
    readNotiCfg(&notiCfg);
    Notification tempNoti;
    strcpy(tempNoti.title, "");
    strcpy(tempNoti.text, "暂无更多通知");
    tempNoti.time = 0;
    strcpy(tempNoti.timeStr, "");
    appendNoti(tempNoti);
    serverNotiSetup();
    appNotiUiDisplay(notiAppRunData->notiStream.notis[notiAppRunData->currNoti],
                     LV_SCR_LOAD_ANIM_NONE, true);
    return 0;
}

static void notiRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->action == ACT_BACK)
    {
        appCenter->exitApp();
        return;
    }
    else if (action->action == ACT_RIGHT)
    {
        animType = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        notiAppRunData->prevNoti = notiAppRunData->currNoti;
        notiAppRunData->currNoti = (notiAppRunData->currNoti + 1) % notiAppRunData->notiStream.size;
    }
    else if (action->action == ACT_LEFT)
    {
        animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
        notiAppRunData->prevNoti = notiAppRunData->currNoti;
        notiAppRunData->currNoti = (notiAppRunData->currNoti - 1 + notiAppRunData->notiStream.size) % notiAppRunData->notiStream.size;
    }

    serverNotiLoop();

    appNotiUiDisplay(getNoti(notiAppRunData->currNoti), animType, false);
    delay(100);
}

static void notiBackground(AppCenter *appCenter, const Action *action)
{
}

static int notiExit(void *param)
{
    appNotiUiDelete();

    serverNotiStop();

    if (notiAppRunData != NULL)
    {
        free(notiAppRunData);
        notiAppRunData = NULL;
    }
    return 0;
}

static void notiOnMessage(const char *from, const char *to,
                          AppMsgType type, void *msg, void *info)
{
}

App notiApp = {
    APP_NOTI_NAME,
    &icon_noti,
    APP_NOTI_INFO,
    notiInit,
    notiRoutine,
    notiBackground,
    notiExit,
    notiOnMessage};