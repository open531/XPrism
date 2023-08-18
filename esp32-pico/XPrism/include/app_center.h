#ifndef APP_CENTER_H
#define APP_CENTER_H

#include "Arduino.h"
#include "app.h"
#include "common.h"
#include <list>

#define CTRL_NAME "AppCenter"
#define APP_MAX_NUM 20           // 最大的可运行的APP数量
#define WIFI_LIFE_CYCLE 60000    // wifi的生命周期（60s）
#define MQTT_ALIVE_CYCLE 1000    // mqtt重连周期
#define EVENT_LIST_MAX_LENGTH 10 // 消息队列的容量
#define APP_CENTER_NAME_LEN 16   // app控制器的名字长度

struct EVENT_OBJ
{
    const App *from;       // 发送请求服务的APP
    AppMsgType type;     // app的事件类型
    void *info;                // 请求携带的信息
    uint8_t retryMaxNum;       // 重试次数
    uint8_t retryCount;        // 重试计数
    unsigned long nextRunTime; // 下次运行的时间戳
};

class AppCenter
{
public:
    AppCenter(const char *name = CTRL_NAME);
    ~AppCenter();
    void init(void);
    void Display(void); // 显示接口
    int app_auto_start();
    // 将APP注册到app_controller中
    int app_install(App *app,
                    AppType app_type = APP_TYPE_REAL_TIME);
    // 将APP从app_controller中卸载（删除）
    int app_uninstall(const App *app);
    // 将APP的后台任务从任务队列中移除(自能通过APP退出的时候，移除自身的后台任务)
    int remove_background_task(void);
    int main_process(Action *act_info);
    void app_exit(void); // 提供给app退出的系统调用
    // 消息发送
    int send_to(const char *from, const char *to,
                AppMsgType type, void *message,
                void *ext_info);
    void deal_config(AppMsgType type,
                     const char *key, char *value);
    // 事件处理
    int req_event_deal(void);
    bool wifi_event(AppMsgType type); // wifi事件的处理
    void read_config(SysUtilConfig *cfg);
    void write_config(SysUtilConfig *cfg);

private:
    App *getAppByName(const char *name);
    int getAppIdxByName(const char *name);
    int app_is_legal(const App *app_obj);

private:
    char name[APP_CENTER_NAME_LEN];    // app控制器的名字
    App *appList[APP_MAX_NUM];     // 预留APP_MAX_NUM个APP注册位
    AppType appTypeList[APP_MAX_NUM]; // 对应APP的运行类型
    // std::list<const App *> app_list; // APP注册位(为了C语言可移植，放弃使用链表)
    std::list<EVENT_OBJ> eventList;   // 用来储存事件
    boolean m_wifi_status;            // 表示是wifi状态 true开启 false关闭
    unsigned long m_preWifiReqMillis; // 保存上一回请求的时间戳
    unsigned int app_num;
    boolean app_exit_flag; // 表示是否退出APP应用
    int cur_app_index;     // 当前运行的APP下标
    int pre_app_index;     // 上一次运行的APP下标

    TimerHandle_t xTimerEventDeal; // 事件处理定时器

public:
    SysUtilConfig sys_cfg;
};

#endif