#include "app_noti.h"
#include "app_noti_ui.h"
#include "icons.h"
#include "app_center.h"
#include "network.h"

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

void handleNoti()
{
}
