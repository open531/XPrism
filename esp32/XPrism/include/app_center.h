#ifndef APP_CENTER_H
#define APP_CENTER_H

#include "app.h"
#include "system.h"
#include <list>

#define APP_CENTER_NAME "AppCenter"
#define APP_CENTER_MAX_APP 10

class AppCenter
{
    // private:
public:
    char *name;
    unsigned int appNum;
    App *appList[APP_CENTER_MAX_APP];
    AppType appType[APP_CENTER_MAX_APP];
    std::list<Event> eventList;
    boolean wifiStatus;
    unsigned long lastWifiTime;
    boolean inApp;
    unsigned int currApp;
    unsigned int prevApp;
    TimerHandle_t xTimerEventDeal;
    App *getApp(const char *name);
    int getAppIndex(const char *name);
    int isAppLegal(const App *app);

public:
    AppCenter(const char *name);
    ~AppCenter();
    void init();
    void display();
    int routine();
    int autoStart();
    int installApp(App *app, AppType type = APP_TYPE_FRONT);
    int uninstallApp(const App *app);
    void appExit();
    int sendMsg(const char *from, const char *to,
                AppMsg type, void *msg, void *info);
    void dealConfig(AppMsg type, const char *key, char *info);
    int dealEvent();
    boolean wifiEvent(AppMsg type);
    void readConfig(SysUtilCfg *cfg);
    void writeConfig(SysUtilCfg *cfg);
    SysUtilCfg sysUtilCfg;
};

extern AppCenter *appCenter;

#endif