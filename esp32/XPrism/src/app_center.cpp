#include "app_center.h"
#include "app_center_ui.h"
#include "flash_fs.h"
#include <Arduino.h>

#define SYS_UTIL_CFG_FILE "/sys_util.cfg"

volatile static boolean isEventDeal = false;

void eventDeal(TimerHandle_t xTimer)
{
    isEventDeal = true;
}

AppCenter::AppCenter(const char *name)
{
    strncpy(this->name, name, 16);
    appNum = 0;
    wifiStatus = false;
    lastWifiTime = millis();
    inApp = false;
    currApp = 0;
    prevApp = 0;
    xTimerEventDeal = xTimerCreate("EventDeal", 300 / portTICK_PERIOD_MS,
                                   pdTRUE, (void *)0, eventDeal);
    xTimerStart(xTimerEventDeal, 0);
}

AppCenter::~AppCenter()
{
}

void AppCenter::init()
{
    if (this->sysUtilCfg.powerMode == 1)
    {
        setCpuFrequencyMhz(240);
    }
    else
    {
        setCpuFrequencyMhz(80);
    }

    Serial.print("CpuFrequencyMhz:");
    Serial.println(getCpuFrequencyMhz());

    appCenterUiInit();
    appList[0] = new App();
    appList[0]->icon = &icon_loading;
    appList[0]->name = "Loading...";
    appType[0] = APP_TYPE_FRONT;
    appCenterUiDisplay(appList[currApp]->icon, appList[currApp]->name,
                       LV_SCR_LOAD_ANIM_NONE, true);
}

void AppCenter::display()
{
    appCenterUiDisplay(appList[currApp]->icon, appList[currApp]->name,
                       LV_SCR_LOAD_ANIM_NONE, true);
}

void clickForward()
{
    appCenter->inApp = true;
    if (appCenter->appList[appCenter->currApp]->init != NULL)
    {
        (*(appCenter->appList[appCenter->currApp]->init))(appCenter);
    }
}

void clickLeft()
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
    appCenter->prevApp = appCenter->currApp;
    appCenter->currApp = (appCenter->currApp + appCenter->appNum - 1) % appCenter->appNum;
    Serial.print("currApp:");
    Serial.println(appCenter->appList[appCenter->currApp]->name);
    appCenterUiDisplay(appCenter->appList[appCenter->currApp]->icon,
                       appCenter->appList[appCenter->currApp]->name,
                       animType, false);
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

void clickRight()
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
    appCenter->prevApp = appCenter->currApp;
    appCenter->currApp = (appCenter->currApp + 1) % appCenter->appNum;
    Serial.print("currApp:");
    Serial.println(appCenter->appList[appCenter->currApp]->name);
    appCenterUiDisplay(appCenter->appList[appCenter->currApp]->icon,
                       appCenter->appList[appCenter->currApp]->name,
                       animType, false);
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

int AppCenter::routine()
{
    if (isEventDeal)
    {
        isEventDeal = false;
        dealEvent();
    }

    if (sysUtilCfg.powerMode == 0 && millis() - lastWifiTime > 60000)
    {
        sendMsg(APP_CENTER_NAME, APP_CENTER_NAME,
                APP_MSG_WIFI_DISCONN, NULL, NULL);
    }

    if (!inApp)
    {
        m_button.routine();
        m_button.attachClick(BTN_FORWARD, clickForward);
        m_button.attachClick(BTN_LEFT, clickLeft);
        m_button.attachClick(BTN_RIGHT, clickRight);
    }
    else
    {
        appCenterUiDisplay(appList[currApp]->icon, appList[currApp]->name,
                           LV_SCR_LOAD_ANIM_NONE, false);
        (*(appList[currApp]->routine))(this);
    }
    return 0;
}

int AppCenter::autoStart()
{
    int index = getAppIndex(sysUtilCfg.autoStart.c_str());
    if (index < 0)
    {
        return -1;
    }
    inApp = true;
    currApp = index;
    if (appList[currApp]->init != NULL)
    {
        (*(appList[currApp]->init))(this);
    }
    return 0;
}

int AppCenter::installApp(App *app, AppType type)
{
    if (appNum >= APP_CENTER_MAX_APP)
    {
        return -1;
    }
    if (isAppLegal(app) < 0)
    {
        return -2;
    }
    appList[appNum] = app;
    appType[appNum] = type;
    appNum++;
    return 0;
}

int AppCenter::uninstallApp(const App *app)
{
    int index = getAppIndex(app->name);
    if (index < 0)
    {
        return -1;
    }
    for (int i = index; i < appNum - 1; i++)
    {
        appList[i] = appList[i + 1];
        appType[i] = appType[i + 1];
    }
    appNum--;
    return 0;
}

void AppCenter::appExit()
{
    inApp = false;

    for (std::list<Event>::iterator it = eventList.begin(); it != eventList.end();)
    {
        if ((*it).from == appList[currApp])
        {
            it = eventList.erase(it);
        }
        else
        {
            it++;
        }
    }

    if (appList[currApp]->exit != NULL)
    {
        (*(appList[currApp]->exit))(this);
    }
    appCenterUiDisplay(appList[currApp]->icon, appList[currApp]->name,
                       LV_SCR_LOAD_ANIM_NONE, false);

    if (sysUtilCfg.powerMode == 1)
    {
        setCpuFrequencyMhz(240);
    }
    else
    {
        setCpuFrequencyMhz(80);
    }
    Serial.print("CpuFrequencyMhz:");
    Serial.println(getCpuFrequencyMhz());
}

int AppCenter::sendMsg(const char *from, const char *to,
                       AppMsg type, void *msg, void *info)
{
    App *fromApp = getApp(from);
    App *toApp = getApp(to);
    if (type <= APP_MSG_MQTT_DATA)
    {
        if (eventList.size() > 10)
        {
            return -1;
        }
        Event newEvent = {fromApp, type, info, 0, 3, millis()};
        eventList.push_back(newEvent);
        Serial.print("eventList.size():");
        Serial.println(eventList.size());
    }
    else
    {
        if (toApp != NULL)
        {
            if (toApp->onMessage != NULL)
            {
                (*(toApp->onMessage))(from, to, type, msg, info);
            }
        }
        else if (!strcmp(to, APP_CENTER_NAME))
        {
            dealConfig(type, (const char *)msg, (char *)info);
        }
    }
    return 0;
}

void AppCenter::dealConfig(AppMsg type, const char *key, char *info)
{
    if (type == APP_MSG_GET_PARAM)
    {
        if (!strcmp(key, "ssid"))
        {
            strncpy(info, sysUtilCfg.ssid.c_str(), 32);
        }
        else if (!strcmp(key, "password"))
        {
            strncpy(info, sysUtilCfg.password.c_str(), 32);
        }
        else if (!strcmp(key, "autoStart"))
        {
            strncpy(info, sysUtilCfg.autoStart.c_str(), 32);
        }
        else if (!strcmp(key, "powerMode"))
        {
            sprintf(info, "%d", sysUtilCfg.powerMode);
        }
        else if (!strcmp(key, "brightness"))
        {
            sprintf(info, "%d", sysUtilCfg.brightness);
        }
        else if (!strcmp(key, "volume"))
        {
            sprintf(info, "%d", sysUtilCfg.volume);
        }
        else if (!strcmp(key, "rotation"))
        {
            sprintf(info, "%d", sysUtilCfg.rotation);
        }
    }
    else if (type == APP_MSG_SET_PARAM)
    {
        if (!strcmp(key, "ssid"))
        {
            sysUtilCfg.ssid = info;
        }
        else if (!strcmp(key, "password"))
        {
            sysUtilCfg.password = info;
        }
        else if (!strcmp(key, "autoStart"))
        {
            sysUtilCfg.autoStart = info;
        }
        else if (!strcmp(key, "powerMode"))
        {
            sysUtilCfg.powerMode = atoi(info);
        }
        else if (!strcmp(key, "brightness"))
        {
            sysUtilCfg.brightness = atoi(info);
        }
        else if (!strcmp(key, "volume"))
        {
            sysUtilCfg.volume = atoi(info);
        }
        else if (!strcmp(key, "rotation"))
        {
            sysUtilCfg.rotation = atoi(info);
        }
        writeConfig(&sysUtilCfg);
    }
    else if (type == APP_MSG_READ_CFG)
    {
        readConfig(&sysUtilCfg);
    }
    else if (type == APP_MSG_WRITE_CFG)
    {
        writeConfig(&sysUtilCfg);
    }
}

int AppCenter::dealEvent()
{
    for (std::list<Event>::iterator it = eventList.begin(); it != eventList.end();)
    {
        if (millis() - (*it).lastTime > 3000)
        {
            if ((*it).retryNum < (*it).retryMax)
            {
                (*it).retryNum++;
                (*it).lastTime = millis();
                if ((*it).from->onMessage != NULL)
                {
                    (*(it->from->onMessage))(APP_CENTER_NAME, it->from->name,
                                             it->type, NULL, it->info);
                }
                it++;
            }
            else
            {
                it = eventList.erase(it);
            }
        }
        else
        {
            it++;
        }
    }
    return 0;
}

boolean AppCenter::wifiEvent(AppMsg type)
{
    if (type == APP_MSG_WIFI_CONN)
    {
        if (!wifiStatus)
        {
            m_network.connectNetwork(sysUtilCfg.ssid.c_str(),
                                     sysUtilCfg.password.c_str());
            wifiStatus = true;
        }
        lastWifiTime = millis();
        return true;
    }
    else if (type == APP_MSG_WIFI_DISCONN)
    {
        if (wifiStatus)
        {
            m_network.closeNetwork();
            wifiStatus = false;
        }
        wifiStatus = false;
        return true;
    }
    else if (type == APP_MSG_WIFI_AP)
    {
        // wifiStatus = true;
        // lastWifiTime = millis();
        return true;
    }
    else if (type == APP_MSG_WIFI_ALIVE)
    {
        wifiStatus = true;
        lastWifiTime = millis();
        return true;
    }
    return false;
}

void AppCenter::readConfig(SysUtilCfg *cfg)
{
    char info[128] = {0};
    uint16_t size = m_flashFS.readFile(SYS_UTIL_CFG_FILE, (uint8_t *)info);
    info[size] = '\0';
    if (size == 0)
    {
        cfg->ssid = "";
        cfg->password = "";
        cfg->autoStart = "";
        cfg->powerMode = 0;
        cfg->brightness = 80;
        cfg->volume = 20;
        cfg->rotation = 0;
        writeConfig(cfg);
    }
    else
    {
        char *param[7] = {0};
        analyseParam(info, 7, param);
        cfg->ssid = param[0];
        cfg->password = param[1];
        cfg->autoStart = param[2];
        cfg->powerMode = atoi(param[3]);
        cfg->brightness = atoi(param[4]);
        cfg->volume = atoi(param[5]);
        cfg->rotation = atoi(param[6]);
    }
}

void AppCenter::writeConfig(SysUtilCfg *cfg)
{
    char info[128] = {0};
    sprintf(info, "%s\n%s\n%s\n%d\n%d\n%d\n%d\n",
            cfg->ssid.c_str(), cfg->password.c_str(), cfg->autoStart.c_str(),
            cfg->powerMode, cfg->brightness, cfg->volume, cfg->rotation);
    m_flashFS.writeFile(SYS_UTIL_CFG_FILE, info);
}

App *AppCenter::getApp(const char *name)
{
    for (int i = 0; i < appNum; i++)
    {
        if (!strcmp(appList[i]->name, name))
        {
            return appList[i];
        }
    }
    return NULL;
}

int AppCenter::getAppIndex(const char *name)
{
    for (int i = 0; i < appNum; i++)
    {
        if (!strcmp(appList[i]->name, name))
        {
            return i;
        }
    }
    return -1;
}

int AppCenter::isAppLegal(const App *app)
{
    if (app->name == NULL || app->icon == NULL || app->info == NULL ||
        app->init == NULL || app->routine == NULL || app->background == NULL ||
        app->exit == NULL || app->onMessage == NULL)
    {
        return -1;
    }
    return 0;
}