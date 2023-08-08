#include "pages_main.h"
#include "pages_main_ui.h"
#include "system.h"
#include "interface.h"
#include "Arduino.h"

const char *app_event_type_info[] = {"APP_MESSAGE_WIFI_CONN", "APP_MESSAGE_WIFI_AP",
                                     "APP_MESSAGE_WIFI_ALIVE", "APP_MESSAGE_WIFI_DISCONN",
                                     "APP_MESSAGE_UPDATE_TIME", "APP_MESSAGE_MQTT_DATA",
                                     "APP_MESSAGE_GET_PARAM", "APP_MESSAGE_SET_PARAM",
                                     "APP_MESSAGE_READ_CFG", "APP_MESSAGE_WRITE_CFG",
                                     "APP_MESSAGE_NONE"};

volatile static bool isRunEventDeal = false;

// TickType_t mainFormRefreshLastTime;
// const TickType_t xDelay500ms = pdMS_TO_TICKS(500);
// mainFormRefreshLastTime = xTaskGetTickCount();
// vTaskDelayUntil(&mainFormRefreshLastTime, xDelay500ms);

void eventDealHandle(TimerHandle_t xTimer)
{
    isRunEventDeal = true;
}

XPages::XPages(const char *name)
{
    strncpy(this->name, name, APP_CONTROLLER_NAME_LEN);
    app_num = 0;
    app_exit_flag = 0;
    cur_app_index = 0;
    pre_app_index = 0;
    // appList = new APP_OBJ[APP_MAX_NUM];
    m_wifi_status = false;
    m_preWifiReqMillis = GET_SYS_MILLIS();

    // 定义一个事件处理定时器
    xTimerEventDeal = xTimerCreate("Event Deal",
                                   300 / portTICK_PERIOD_MS,
                                   pdTRUE, (void *)0, eventDealHandle);
    // 启动事件处理定时器
    xTimerStart(xTimerEventDeal, 0);
}

void XPages::init(void)
{
    // 设置CPU主频
    if (1 == this->sys_cfg.power_mode)
    {
        setCpuFrequencyMhz(240);
    }
    else
    {
        setCpuFrequencyMhz(80);
    }
    // uint32_t freq = getXtalFrequencyMhz(); // In MHz
    Serial.print(F("CpuFrequencyMhz: "));
    Serial.println(getCpuFrequencyMhz());

    app_control_gui_init();
    appList[0] = new APP_OBJ();
    appList[0]->app_image = &app_loading;
    appList[0]->app_name = "Loading...";
    appTypeList[0] = APP_TYPE_REAL_TIME;
    app_control_display_scr(appList[cur_app_index]->app_image,
                            appList[cur_app_index]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);
    // Display();
}

void XPages::Display()
{
    // appList[0].app_image = &app_loading;
    app_control_display_scr(appList[cur_app_index]->app_image,
                            appList[cur_app_index]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);
}

XPages::~XPages()
{
    led_stop();
}

int XPages::app_is_legal(const APP_OBJ *app_obj)
{
    // APP的合法性检测
    if (NULL == app_obj)
        return 1;
    if (APP_MAX_NUM <= app_num)
        return 2;
    return 0;
}

// 将APP安装到app_controller中
int XPages::app_install(APP_OBJ *app, APP_TYPE app_type)
{
    int ret_code = app_is_legal(app);
    if (0 != ret_code)
    {
        return ret_code;
    }

    appList[app_num] = app;
    appTypeList[app_num] = app_type;
    ++app_num;
    return 0; // 安装成功
}

// 将APP的后台任务从任务队列中移除(自能通过APP退出的时候，移除自身的后台任务)
int XPages::remove_backgroud_task(void)
{
    return 0; // 安装成功
}

// 将APP从app_controller中卸载（删除）
int XPages::app_uninstall(const APP_OBJ *app)
{
    // todo
    return 0;
}

int XPages::app_auto_start()
{
    // APP自启动
    int index = this->getAppIdxByName(sys_cfg.auto_start_app.c_str());
    if (index < 0)
    {
        // 没找到相关的APP
        return 0;
    }
    // 进入自启动的APP
    app_exit_flag = 1; // 进入app, 如果已经在
    cur_app_index = index;
    (*(appList[cur_app_index]->app_init))(this); // 执行APP初始化
    return 0;
}

int XPages::main_process(Action *act_info)
{
    if (ACTION_TYPE::UNKNOWN != act_info->type)
    {
        Serial.print(F("[Operate]\tact_info->type: "));
        Serial.println(action_type_info[act_info->type]);
    }

    if (isRunEventDeal)
    {
        isRunEventDeal = false;
        // 扫描事件
        this->req_event_deal();
    }

    // wifi自动关闭(在节能模式下)
    if (0 == sys_cfg.power_mode && true == m_wifi_status && doDelayMillisTime(WIFI_LIFE_CYCLE, &m_preWifiReqMillis, false))
    {
        send_to(XPAGES_NAME, XPAGES_NAME, APP_MESSAGE_WIFI_DISCONN, 0, NULL);
    }

    if (0 == app_exit_flag)
    {
        // 当前没有进入任何app
        lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;
        if (ACTION_TYPE::LEFT == act_info->type)
        {
            anim_type = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
            pre_app_index = cur_app_index;
            cur_app_index = (cur_app_index + 1) % app_num;
            Serial.println(String("Current App: ") + appList[cur_app_index]->app_name);
        }
        else if (ACTION_TYPE::RIGHT == act_info->type)
        {
            anim_type = LV_SCR_LOAD_ANIM_MOVE_LEFT;
            pre_app_index = cur_app_index;
            // 以下等效与 processId = (processId - 1 + APP_NUM) % 4;
            // +3为了不让数据溢出成负数，而导致取模逻辑错误
            cur_app_index = (cur_app_index - 1 + app_num) % app_num; // 此处的3与p_processList的长度一致
            Serial.println(String("Current App: ") + appList[cur_app_index]->app_name);
        }
        else if (ACTION_TYPE::FORWARD == act_info->type)
        {
            app_exit_flag = 1; // 进入app
            if (NULL != appList[cur_app_index]->app_init)
            {
                (*(appList[cur_app_index]->app_init))(this); // 执行APP初始化
            }
        }

        if (ACTION_TYPE::FORWARD != act_info->type) // && UNKNOWN != act_info->type
        {
            app_control_display_scr(appList[cur_app_index]->app_image,
                                    appList[cur_app_index]->app_name,
                                    anim_type, false);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        app_control_display_scr(appList[cur_app_index]->app_image,
                                appList[cur_app_index]->app_name,
                                LV_SCR_LOAD_ANIM_NONE, false);
        // 运行APP进程 等效于把控制权交给当前APP
        (*(appList[cur_app_index]->main_process))(this, act_info);
    }
    act_info->type = ACTION_TYPE::UNKNOWN;
    act_info->isValid = 0;
    return 0;
}

APP_OBJ *XPages::getAppByName(const char *name)
{
    for (int pos = 0; pos < app_num; ++pos)
    {
        if (!strcmp(name, appList[pos]->app_name))
        {
            return appList[pos];
        }
    }

    return NULL;
}

int XPages::getAppIdxByName(const char *name)
{
    for (int pos = 0; pos < app_num; ++pos)
    {
        if (!strcmp(name, appList[pos]->app_name))
        {
            return pos;
        }
    }

    return -1;
}

// 通信中心（消息转发）
int XPages::send_to(const char *from, const char *to,
                    APP_MESSAGE_TYPE type, void *message,
                    void *ext_info)
{
    APP_OBJ *fromApp = getAppByName(from); // 来自谁 有可能为空
    APP_OBJ *toApp = getAppByName(to);     // 发送给谁 有可能为空
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
        else if (!strcmp(to, XPAGES_NAME))
        {
            Serial.print("[Massage]\tFrom " + String(fromApp->app_name) + "\tTo " + XPAGES_NAME + "\n");
            deal_config(type, (const char *)message, (char *)ext_info);
        }
    }
    return 0;
}

int XPages::req_event_deal(void)
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
        bool ret = wifi_event((*event).type);
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
            (*((*event).from->message_handle))(XPAGES_NAME, (*event).from->app_name,
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
bool XPages::wifi_event(APP_MESSAGE_TYPE type)
{
    switch (type)
    {
    case APP_MESSAGE_WIFI_CONN:
    {
        // 更新请求
        // CONN_ERROR == g_network.end_conn_wifi() ||
        if (false == m_wifi_status)
        {
            g_network.start_conn_wifi(sys_cfg.ssid_0.c_str(), sys_cfg.password_0.c_str());
            m_wifi_status = true;
        }
        m_preWifiReqMillis = GET_SYS_MILLIS();
        if ((WiFi.getMode() & WIFI_MODE_STA) == WIFI_MODE_STA && CONN_SUCC != g_network.end_conn_wifi())
        {
            // 在STA模式下 并且还没连接上wifi
            return false;
        }
    }
    break;
    case APP_MESSAGE_WIFI_AP:
    {
        // 更新请求
        g_network.open_ap(AP_SSID);
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
        g_network.close_wifi();
        m_wifi_status = false; // 标志位
                               // m_preWifiReqMillis = GET_SYS_MILLIS() - WIFI_LIFE_CYCLE;
    }
    break;
    case APP_MESSAGE_UPDATE_TIME:
    {
    }
    break;
    case APP_MESSAGE_MQTT_DATA:
    {
        Serial.println("APP_MESSAGE_MQTT_DATA");
        if (app_exit_flag == 1 && cur_app_index != getAppIdxByName("Heartbeat")) // 在其他app中
        {
            app_exit_flag = 0;
            (*(appList[cur_app_index]->exit_callback))(NULL); // 退出当前app
        }
        if (app_exit_flag == 0)
        {
            app_exit_flag = 1; // 进入app, 如果已经在
            cur_app_index = getAppIdxByName("Heartbeat");
            (*(getAppByName("Heartbeat")->app_init))(this); // 执行APP初始化
        }
    }
    break;
    default:
        break;
    }

    return true;
}

void XPages::app_exit()
{
    app_exit_flag = 0; // 退出APP

    // 清空该对象的所有请求
    for (std::list<EVENT_OBJ>::iterator event = eventList.begin(); event != eventList.end();)
    {
        if (appList[cur_app_index] == (*event).from)
        {
            event = eventList.erase(event); // 删除该响应事件
        }
        else
        {
            ++event;
        }
    }

    if (NULL != appList[cur_app_index]->exit_callback)
    {
        // 执行APP退出回调
        (*(appList[cur_app_index]->exit_callback))(NULL);
    }
    app_control_display_scr(appList[cur_app_index]->app_image,
                            appList[cur_app_index]->app_name,
                            LV_SCR_LOAD_ANIM_NONE, true);

    // 恢复RGB灯  HSV色彩模式
    LEDConfig *cfg = &led_cfg;
    LEDParam led_setting = {LED_MODE_HSV,
                            cfg->min_value_0, cfg->min_value_1, cfg->min_value_2,
                            cfg->max_value_0, cfg->max_value_1, cfg->max_value_2,
                            cfg->step_0, cfg->step_1, cfg->step_2,
                            cfg->min_brightness, cfg->max_brightness,
                            cfg->brightness_step, cfg->time};
    set_led_and_run(&led_setting);

    // 设置CPU主频
    if (1 == this->sys_cfg.power_mode)
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

void XPages::read_config(SysUtilConfig *cfg)
{
    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    uint16_t size = g_flashCfg.readFile(APP_CTRL_CONFIG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        cfg->power_mode = 0;           // 功耗模式（0为节能模式 1为性能模式）
        cfg->backLight = 80;           // 屏幕亮度（1-100）
        cfg->rotation = 4;             // 屏幕旋转方向
        cfg->auto_calibration_mpu = 1; // 是否自动校准陀螺仪 0关闭自动校准 1打开自动校准
        cfg->mpu_order = 0;            // 操作方向
        cfg->auto_start_app = "None";  // 无指定开机自启APP
        this->write_config(cfg);
    }
    else
    {
        // 解析数据
        char *param[12] = {0};
        analyseParam(info, 12, param);
        cfg->ssid_0 = param[0];
        cfg->password_0 = param[1];
        cfg->ssid_1 = param[2];
        cfg->password_1 = param[3];
        cfg->ssid_2 = param[4];
        cfg->password_2 = param[5];
        cfg->power_mode = atol(param[6]);
        cfg->backLight = atol(param[7]);
        cfg->rotation = atol(param[8]);
        cfg->auto_calibration_mpu = atol(param[9]);
        cfg->mpu_order = atol(param[10]);
        cfg->auto_start_app = param[11]; // 开机自启APP的name
    }
}

void XPages::write_config(SysUtilConfig *cfg)
{
    char tmp[25];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    w_data = w_data + cfg->ssid_0 + "\n";
    w_data = w_data + cfg->password_0 + "\n";
    w_data = w_data + cfg->ssid_1 + "\n";
    w_data = w_data + cfg->password_1 + "\n";
    w_data = w_data + cfg->ssid_2 + "\n";
    w_data = w_data + cfg->password_2 + "\n";
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->power_mode);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->backLight);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->rotation);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->auto_calibration_mpu);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->mpu_order);
    w_data += tmp;

    w_data = w_data + cfg->auto_start_app + "\n";

    g_flashCfg.writeFile(APP_CTRL_CONFIG_PATH, w_data.c_str());

    // 立即生效相关配置
    screen.setBackLight(cfg->backLight / 100.0);
    tft->setRotation(cfg->rotation);
    mpu.setOrder(cfg->mpu_order);
}

void XPages::read_config(SysMpuConfig *cfg)
{
    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    uint16_t size = g_flashCfg.readFile(MPU_CONFIG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        cfg->x_gyro_offset = 0;
        cfg->y_gyro_offset = 0;
        cfg->z_gyro_offset = 0;
        cfg->x_accel_offset = 0;
        cfg->y_accel_offset = 0;
        cfg->z_accel_offset = 0;

        this->write_config(cfg);
    }
    else
    {
        // 解析数据
        char *param[6] = {0};
        analyseParam(info, 6, param);
        cfg->x_gyro_offset = atol(param[0]);
        cfg->y_gyro_offset = atol(param[1]);
        cfg->z_gyro_offset = atol(param[2]);
        cfg->x_accel_offset = atol(param[3]);
        cfg->y_accel_offset = atol(param[4]);
        cfg->z_accel_offset = atol(param[5]);
    }
}

void XPages::write_config(SysMpuConfig *cfg)
{
    char tmp[25];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->x_gyro_offset);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->y_gyro_offset);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->z_gyro_offset);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->x_accel_offset);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->y_accel_offset);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->z_accel_offset);
    w_data += tmp;
    g_flashCfg.writeFile(MPU_CONFIG_PATH, w_data.c_str());
}

void XPages::read_config(LEDConfig *cfg)
{
    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    uint16_t size = g_flashCfg.readFile(LED_CONFIG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        cfg->mode = 1;
        cfg->min_value_0 = 1;
        cfg->min_value_1 = 32;
        cfg->min_value_2 = 255;
        cfg->max_value_0 = 255;
        cfg->max_value_1 = 255;
        cfg->max_value_2 = 255;
        cfg->step_0 = 1;
        cfg->step_1 = 1;
        cfg->step_2 = 1;
        cfg->min_brightness = 0.15;
        cfg->max_brightness = 0.25;
        cfg->brightness_step = 0.001;
        cfg->time = 30;

        this->write_config(cfg);
    }
    else
    {
        // 解析数据
        char *param[14] = {0};
        analyseParam(info, 14, param);
        cfg->mode = atol(param[0]);
        cfg->min_value_0 = atol(param[1]);
        cfg->min_value_1 = atol(param[2]);
        cfg->min_value_2 = atol(param[3]);
        cfg->max_value_0 = atol(param[4]);
        cfg->max_value_1 = atol(param[5]);
        cfg->max_value_2 = atol(param[6]);
        cfg->step_0 = atol(param[7]);
        cfg->step_1 = atol(param[8]);
        cfg->step_2 = atol(param[9]);
        cfg->min_brightness = atof(param[10]);
        cfg->max_brightness = atof(param[11]);
        cfg->brightness_step = atof(param[12]);
        cfg->time = atol(param[13]);
    }
}

void XPages::write_config(LEDConfig *cfg)
{
    char tmp[25];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->mode);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->min_value_0);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->min_value_1);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->min_value_2);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->max_value_0);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->max_value_1);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->max_value_2);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->step_0);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->step_1);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->step_2);
    w_data += tmp;

    if (cfg->min_brightness < 0.01)
    {
        // 限制
        cfg->min_brightness = 0.01;
    }
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%f\n", cfg->min_brightness);
    w_data += tmp;

    if (cfg->max_brightness < 0.01)
    {
        // 限制
        cfg->max_brightness = 0.01;
    }
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%f\n", cfg->max_brightness);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%f\n", cfg->brightness_step);
    w_data += tmp;

    if (cfg->time < 10)
    {
        cfg->time = 10;
    }
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%d\n", cfg->time);
    w_data += tmp;

    g_flashCfg.writeFile(LED_CONFIG_PATH, w_data.c_str());

    // 初始化RGB灯 HSV色彩模式
    LEDParam led_setting = {LED_MODE_HSV,
                            led_cfg.min_value_0, led_cfg.min_value_1, led_cfg.min_value_2,
                            led_cfg.max_value_0, led_cfg.max_value_1, led_cfg.max_value_2,
                            led_cfg.step_0, led_cfg.step_1, led_cfg.step_2,
                            led_cfg.min_brightness, led_cfg.max_brightness,
                            led_cfg.brightness_step, led_cfg.time};
    // 初始化RGB任务
    set_led_and_run(&led_setting);
}

void XPages::deal_config(APP_MESSAGE_TYPE type,
                         const char *key, char *value)
{
    switch (type)
    {

    case APP_MESSAGE_GET_PARAM:
    {
        if (!strcmp(key, "ssid_0"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_0.c_str());
        }
        else if (!strcmp(key, "password_0"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_0.c_str());
        }
        else if (!strcmp(key, "ssid_1"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_1.c_str());
        }
        else if (!strcmp(key, "password_1"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_1.c_str());
        }
        if (!strcmp(key, "ssid_2"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_2.c_str());
        }
        else if (!strcmp(key, "password_2"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_2.c_str());
        }
        else if (!strcmp(key, "power_mode"))
        {
            snprintf(value, 32, "%u", sys_cfg.power_mode);
        }
        else if (!strcmp(key, "backLight"))
        {
            snprintf(value, 32, "%u", sys_cfg.backLight);
        }
        else if (!strcmp(key, "rotation"))
        {
            snprintf(value, 32, "%u", sys_cfg.rotation);
        }
        else if (!strcmp(key, "auto_calibration_mpu"))
        {
            snprintf(value, 32, "%u", sys_cfg.auto_calibration_mpu);
        }
        else if (!strcmp(key, "mpu_order"))
        {
            snprintf(value, 32, "%u", sys_cfg.mpu_order);
        }
        else if (!strcmp(key, "min_brightness"))
        {
            snprintf(value, 32, "%f", led_cfg.min_brightness);
        }
        else if (!strcmp(key, "max_brightness"))
        {
            snprintf(value, 32, "%f", led_cfg.max_brightness);
        }
        else if (!strcmp(key, "time"))
        {
            snprintf(value, 32, "%u", led_cfg.time);
        }
        else if (!strcmp(key, "auto_start_app"))
        {
            snprintf(value, 32, "%s", sys_cfg.auto_start_app.c_str());
        }
    }
    break;
    case APP_MESSAGE_SET_PARAM:
    {
        if (!strcmp(key, "ssid_0"))
        {
            sys_cfg.ssid_0 = value;
        }
        else if (!strcmp(key, "password_0"))
        {
            sys_cfg.password_0 = value;
        }
        else if (!strcmp(key, "ssid_1"))
        {
            sys_cfg.ssid_1 = value;
        }
        else if (!strcmp(key, "password_1"))
        {
            sys_cfg.password_1 = value;
        }
        else if (!strcmp(key, "ssid_2"))
        {
            sys_cfg.ssid_2 = value;
        }
        else if (!strcmp(key, "password_2"))
        {
            sys_cfg.password_2 = value;
        }
        else if (!strcmp(key, "power_mode"))
        {
            sys_cfg.power_mode = atol(value);
        }
        else if (!strcmp(key, "backLight"))
        {
            sys_cfg.backLight = atol(value);
        }
        else if (!strcmp(key, "rotation"))
        {
            sys_cfg.rotation = atol(value);
        }
        else if (!strcmp(key, "auto_calibration_mpu"))
        {
            sys_cfg.auto_calibration_mpu = atol(value);
            if (0 == sys_cfg.auto_calibration_mpu)
            {
                this->write_config(&this->mpu_cfg);
            }
        }
        else if (!strcmp(key, "mpu_order"))
        {
            sys_cfg.mpu_order = atol(value);
        }
        else if (!strcmp(key, "min_brightness"))
        {
            led_cfg.min_brightness = atof(value);
        }
        else if (!strcmp(key, "max_brightness"))
        {
            led_cfg.max_brightness = atof(value);
        }
        else if (!strcmp(key, "time"))
        {
            led_cfg.time = atol(value);
        }
        else if (!strcmp(key, "auto_start_app"))
        {
            sys_cfg.auto_start_app = value;
        }
    }
    break;
    case APP_MESSAGE_READ_CFG:
    {
        read_config(&sys_cfg);
        // read_config(&mpu_cfg);
        read_config(&led_cfg);
    }
    break;
    case APP_MESSAGE_WRITE_CFG:
    {
        write_config(&sys_cfg);
        // write_config(&mpu_cfg);  // 在取消自动校准的时候已经写过一次了
        write_config(&led_cfg);
    }
    break;
    default:
        break;
    }
}
