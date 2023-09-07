#ifndef APP_H
#define APP_H

enum AppMsgType
{
    APP_MESSAGE_WIFI_CONN = 0, // 开启连接
    APP_MESSAGE_WIFI_AP,       // 开启AP事件
    APP_MESSAGE_WIFI_ALIVE,    // wifi开关的心跳维持
    APP_MESSAGE_WIFI_DISCONN,  // 连接断开
    APP_MESSAGE_UPDATE_TIME,
    APP_MESSAGE_MQTT_DATA, // MQTT客户端收到消息
    APP_MESSAGE_GET_PARAM, // 获取参数
    APP_MESSAGE_SET_PARAM, // 设置参数
    APP_MESSAGE_READ_CFG,  // 向磁盘读取参数
    APP_MESSAGE_WRITE_CFG, // 向磁盘写入参数
    APP_MESSAGE_GPS_DATA,  // GPS数据

    APP_MESSAGE_NONE
};

enum AppType
{
    APP_TYPE_REAL_TIME = 0, // 实时应用
    APP_TYPE_BACKGROUND,    // 后台应用

    APP_TYPE_NONE
};

class AppCenter;
struct Action;

struct App
{
    const char *app_name;
    const void *app_image;
    const char *app_info;
    int (*app_init)(AppCenter *sys);
    void (*main_process)(AppCenter *sys,
                         const Action *action);
    void (*background_task)(AppCenter *sys,
                            const Action *action);
    int (*exit_callback)(void *param);
    void (*message_handle)(const char *from, const char *to,
                           AppMsgType type, void *message,
                           void *ext_info);
};

#endif