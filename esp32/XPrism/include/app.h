#ifndef APP_H
#define APP_H

#include <Arduino.h>

enum AppMsg
{
    APP_MSG_WIFI_CONN,
    APP_MSG_WIFI_DISCONN,
    APP_MSG_WIFI_AP,
    APP_MSG_WIFI_ALIVE,
    APP_MSG_UPDATE_TIME,
    APP_MSG_MQTT_DATA,
    APP_MSG_GET_PARAM,
    APP_MSG_SET_PARAM,
    APP_MSG_READ_CFG,
    APP_MSG_WRITE_CFG,
    APP_MSG_NONE
};

enum AppType
{
    APP_TYPE_FRONT,
    APP_TYPE_BACKGROUND,
    APP_TYPE_NONE
};

class AppCenter;

struct App
{
    const char *name;
    const void *icon;
    const char *info;
    int (*init)(AppCenter *appCenter);
    void (*routine)(AppCenter *appCenter);
    void (*background)(AppCenter *appCenter);
    int (*exit)(AppCenter *appCenter);
    void (*onMessage)(const char *from, const char *to,
                      AppMsg type, void *msg, void *info);
};

struct Event
{
    const App *from;
    AppMsg type;
    void *info;
    uint8_t retryNum;
    uint8_t retryMax;
    unsigned long lastTime;
};

#endif