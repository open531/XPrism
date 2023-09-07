#include "app_center.h"
#include "app_center_ui.h"
#include "common.h"
#include "app.h"
#include "Arduino.h"

String wifi_ssid = "Redmi K50";
String wifi_password = "5irmvnfnz5rb6xn";

const char *actionTypeInfo[] = {
    "ACT_NONE",
    "ACT_BACK",
    "ACT_FORWARD",
    "ACT_LEFT",
    "ACT_RIGHT",
    "ACT_UP",
    "ACT_DOWN",
};

const char *app_event_type_info[] = {"APP_MESSAGE_WIFI_CONN", "APP_MESSAGE_WIFI_AP",
                                     "APP_MESSAGE_WIFI_ALIVE", "APP_MESSAGE_WIFI_DISCONN",
                                     "APP_MESSAGE_UPDATE_TIME", "APP_MESSAGE_MQTT_DATA",
                                     "APP_MESSAGE_GET_PARAM", "APP_MESSAGE_SET_PARAM",
                                     "APP_MESSAGE_READ_CFG", "APP_MESSAGE_WRITE_CFG",
                                     "APP_MESSAGE_NONE"};

volatile static bool isRunEventDeal = false;

void eventDealHandle(TimerHandle_t xTimer)
{
    isRunEventDeal = true;
}

AppCenter::AppCenter(const char *name)
{
    strncpy(this->name, name, APP_CENTER_NAME_LEN);
    appNum = 0;
    appExitFlag = 0;
    currAppIndex = 0;
    prevAppIndex = 0;
    m_wifi_status = false;
    m_preWifiReqMillis = GET_SYS_MILLIS();

    // 定义一个事件处理定时器
    xTimerEventDeal = xTimerCreate("Event Deal",
                                   300 / portTICK_PERIOD_MS,
                                   pdTRUE, (void *)0, eventDealHandle);
    // 启动事件处理定时器
    xTimerStart(xTimerEventDeal, 0);
}

void AppCenter::centerInit(void)
{
    // 设置CPU主频
    if (1 == this->sysCfg.power_mode)
    {
        setCpuFrequencyMhz(240);
    }
    else
    {
        setCpuFrequencyMhz(80);
    }
    Serial.print(F("CpuFrequencyMhz: "));
    Serial.println(getCpuFrequencyMhz());

    app_control_gui_init();
    appList[0] = new App();
    appList[0]->app_image = &app_loading;
    appList[0]->app_name = "Loading...";
    appTypeList[0] = APP_TYPE_REAL_TIME;
    app_control_display_scr(appList[currAppIndex]->app_image,
                            appList[currAppIndex]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);
    sendMsgTo(CTRL_NAME, CTRL_NAME, APP_MESSAGE_WIFI_CONN, NULL, NULL);
}

void AppCenter::Display()
{
    app_control_display_scr(appList[currAppIndex]->app_image,
                            appList[currAppIndex]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);
}

AppCenter::~AppCenter()
{
}

int AppCenter::isAppLegal(const App *app_obj)
{
    // APP的合法性检测
    if (NULL == app_obj)
        return 1;
    if (APP_MAX_NUM <= appNum)
        return 2;
    return 0;
}

int AppCenter::installApp(App *app, AppType app_type)
{
    int ret_code = isAppLegal(app);
    if (0 != ret_code)
    {
        return ret_code;
    }

    appList[appNum] = app;
    appTypeList[appNum] = app_type;
    ++appNum;
    return 0; // 安装成功
}

int AppCenter::removeBackgroundTask(void)
{
    return 0;
}

int AppCenter::uninstallApp(const App *app)
{
    // todo
    return 0;
}

int AppCenter::autoStart()
{
    // APP自启动
    int index = this->getAppIdxByName(sysCfg.auto_start_app.c_str());
    if (index < 0)
    {
        // 没找到相关的APP
        return 0;
    }
    // 进入自启动的APP
    appExitFlag = 1;
    currAppIndex = index;
    (*(appList[currAppIndex]->app_init))(this); // 执行APP初始化
    return 0;
}

int AppCenter::centerRoutine(Action *action)
{
    if (action->action != ACT_NONE)
    {
        Serial.print(F("[Operate]\taction->active: "));
        Serial.println(actionTypeInfo[action->action]);
    }

    if (isRunEventDeal)
    {
        isRunEventDeal = false;
        // 扫描事件
        this->dealEvent();
    }

    if (0 == appExitFlag)
    {
        // 当前没有进入任何app
        lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;
        if (action->action == ACT_RIGHT)
        {
            anim_type = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
            prevAppIndex = currAppIndex;
            currAppIndex = (currAppIndex + 1) % appNum;
            Serial.println(String("Current App: ") + appList[currAppIndex]->app_name);
        }
        else if (action->action == ACT_LEFT)
        {
            anim_type = LV_SCR_LOAD_ANIM_MOVE_LEFT;
            prevAppIndex = currAppIndex;
            currAppIndex = (currAppIndex - 1 + appNum) % appNum;
            Serial.println(String("Current App: ") + appList[currAppIndex]->app_name);
        }
        else if (action->action == ACT_FORWARD)
        {
            appExitFlag = 1; // 进入app
            if (NULL != appList[currAppIndex]->app_init)
            {
                (*(appList[currAppIndex]->app_init))(this); // 执行APP初始化
            }
        }

        if (action->action != ACT_FORWARD)
        {
            app_control_display_scr(appList[currAppIndex]->app_image,
                                    appList[currAppIndex]->app_name,
                                    anim_type, false);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        app_control_display_scr(appList[currAppIndex]->app_image,
                                appList[currAppIndex]->app_name,
                                LV_SCR_LOAD_ANIM_NONE, false);
        // 运行APP进程 等效于把控制权交给当前APP
        (*(appList[currAppIndex]->main_process))(this, action);
    }
    action->action = ACT_NONE;
    action->isValid = 0;
    return 0;
}

App *AppCenter::getAppByName(const char *name)
{
    for (int pos = 0; pos < appNum; ++pos)
    {
        if (!strcmp(name, appList[pos]->app_name))
        {
            return appList[pos];
        }
    }

    return NULL;
}

int AppCenter::getAppIdxByName(const char *name)
{
    for (int pos = 0; pos < appNum; ++pos)
    {
        if (!strcmp(name, appList[pos]->app_name))
        {
            return pos;
        }
    }

    return -1;
}

// 通信中心（消息转发）
int AppCenter::sendMsgTo(const char *from, const char *to,
                         AppMsgType type, void *message,
                         void *ext_info)
{
    App *fromApp = getAppByName(from); // 来自谁 有可能为空
    App *toApp = getAppByName(to);     // 发送给谁 有可能为空
    if (type <= APP_MESSAGE_MQTT_DATA)
    {
        // 更新事件的请求者
        if (eventList.size() > EVENT_LIST_MAX_LENGTH)
        {
            return 1;
        }
        // 发给控制器的消息(目前都是wifi事件)
        EVENT_OBJ new_event = {fromApp, type, message, 3, 0, 0};
        eventList.push_back(new_event);
        Serial.print("[EVENT]\tAdd -> " + String(app_event_type_info[type]));
        Serial.print(F("\tEventList Size: "));
        Serial.println(eventList.size());
    }
    else
    {
        // 各个APP之间通信的消息
        if (NULL != toApp)
        {
            Serial.print("[Massage]\tFrom " + String(fromApp->app_name) + "\tTo " + String(toApp->app_name) + "\n");
            if (NULL != toApp->message_handle)
            {
                toApp->message_handle(from, to, type, message, ext_info);
            }
        }
        else if (!strcmp(to, CTRL_NAME))
        {
            Serial.print("[Massage]\tFrom " + String(fromApp->app_name) + "\tTo " + CTRL_NAME + "\n");
            dealCfg(type, (const char *)message, (char *)ext_info);
        }
    }
    return 0;
}

int AppCenter::dealEvent(void)
{
    // 请求事件的处理
    for (std::list<EVENT_OBJ>::iterator event = eventList.begin(); event != eventList.end();)
    {
        if ((*event).nextRunTime > GET_SYS_MILLIS())
        {
            ++event;
            continue;
        }
        // 后期可以拓展其他事件的处理
        bool ret = wifiEvent((*event).type);
        if (false == ret)
        {
            // 本事件没处理完成
            (*event).retryCount += 1;
            if ((*event).retryCount >= (*event).retryMaxNum)
            {
                // 多次重试失败
                Serial.print("[EVENT]\tDelete -> " + String(app_event_type_info[(*event).type]));
                event = eventList.erase(event); // 删除该响应事件
                Serial.print(F("\tEventList Size: "));
                Serial.println(eventList.size());
            }
            else
            {
                // 下次重试
                (*event).nextRunTime = GET_SYS_MILLIS() + 4000;
                ++event;
            }
            continue;
        }

        // 事件回调
        if (NULL != (*event).from && NULL != (*event).from->message_handle)
        {
            (*((*event).from->message_handle))(CTRL_NAME, (*event).from->app_name,
                                               (*event).type, (*event).info, NULL);
        }
        Serial.print("[EVENT]\tDelete -> " + String(app_event_type_info[(*event).type]));
        event = eventList.erase(event); // 删除该响应完成的事件
        Serial.print(F("\tEventList Size: "));
        Serial.println(eventList.size());
    }
    return 0;
}

/**
 *  wifi事件的处理
 *  事件处理成功返回true 否则false
 * */
bool AppCenter::wifiEvent(AppMsgType type)
{
    switch (type)
    {
    case APP_MESSAGE_WIFI_CONN:
    {
        // 更新请求
        if (false == m_wifi_status)
        {
            m_network.start_conn_wifi(wifi_ssid.c_str(), wifi_password.c_str());
            m_wifi_status = true;
        }
        m_preWifiReqMillis = GET_SYS_MILLIS();
        if ((WiFi.getMode() & WIFI_MODE_STA) == WIFI_MODE_STA &&
            CONN_SUCC != m_network.end_conn_wifi())
        {
            return false;
        }
    }
    break;
    case APP_MESSAGE_WIFI_AP:
    {
        // 更新请求
        m_network.open_ap(AP_SSID);
        m_wifi_status = true;
        m_preWifiReqMillis = GET_SYS_MILLIS();
    }
    break;
    case APP_MESSAGE_WIFI_ALIVE:
    {
        // wifi开关的心跳 持续收到心跳 wifi才不会被关闭
        m_wifi_status = true;
        // 更新请求
        m_preWifiReqMillis = GET_SYS_MILLIS();
    }
    break;
    case APP_MESSAGE_WIFI_DISCONN:
    {
        m_network.close_wifi();
        m_wifi_status = false;
    }
    break;
    case APP_MESSAGE_UPDATE_TIME:
    {
    }
    break;
    case APP_MESSAGE_MQTT_DATA:
    {
        Serial.println("APP_MESSAGE_MQTT_DATA");
        if (appExitFlag == 1)
        {
            appExitFlag = 0;
            (*(appList[currAppIndex]->exit_callback))(NULL); // 退出当前app
        }
        if (appExitFlag == 0)
        {
            appExitFlag = 1; // 进入app, 如果已经在
            currAppIndex = getAppIdxByName("Heartbeat");
            (*(getAppByName("Heartbeat")->app_init))(this); // 执行APP初始化
        }
    }
    break;
    default:
        break;
    }

    return true;
}

void AppCenter::exitApp()
{
    appExitFlag = 0; // 退出APP

    // 清空该对象的所有请求
    for (std::list<EVENT_OBJ>::iterator event = eventList.begin(); event != eventList.end();)
    {
        if (appList[currAppIndex] == (*event).from)
        {
            event = eventList.erase(event); // 删除该响应事件
        }
        else
        {
            ++event;
        }
    }

    if (NULL != appList[currAppIndex]->exit_callback)
    {
        // 执行APP退出回调
        (*(appList[currAppIndex]->exit_callback))(NULL);
    }
    app_control_display_scr(appList[currAppIndex]->app_image,
                            appList[currAppIndex]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);

    // 设置CPU主频
    if (1 == this->sysCfg.power_mode)
    {
        setCpuFrequencyMhz(240);
    }
    else
    {
        setCpuFrequencyMhz(80);
    }
    Serial.print(F("CpuFrequencyMhz: "));
    Serial.println(getCpuFrequencyMhz());
}