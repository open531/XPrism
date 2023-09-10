#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "common.h"
#include "app_center.h"

#include "app_conf.h"

#include <SPIFFS.h>
#include <esp32-hal.h>
#include <esp32-hal-timer.h>

static bool isCheckAction = false;
// static bool isCheckGPS = false;

BTNAction *buttonAction; // 存放按键返回的数据
AppCenter *appCenter;    // APP控制器

TimerHandle_t xTimerAction = NULL;
void actionCheckHandle(TimerHandle_t xTimer)
{
    // 标志需要检测动作
    isCheckAction = true;
}
// TimerHandle_t xTimerGPS = NULL;
// void gpsCheckHandle(TimerHandle_t xTimer)
// {
//     // 标志需要检测动作
//     isCheckGPS = true;
// }

void setup()
{
    Serial.begin(115200);
    Serial.flush();
    Serial.print(F("ChipID(EfuseMac): "));
    Serial.println(ESP.getEfuseMac());

    appCenter = new AppCenter(); // APP控制器

    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    appCenter->readCfg(&appCenter->sysCfg);

    /*** Init screen ***/
    m_screen.init(4, 100);

    /*** Init micro SD-Card ***/
    m_tf.init();

    lv_fs_fatfs_init();

    appCenter->centerInit();

// 将APP"安装"到controller里
#if APP_TIME_USE
    appCenter->installApp(&timeApp);
#endif
#if APP_WEATHER_USE
    appCenter->installApp(&weatherApp);
#endif
#if APP_CLOCK_USE
    appCenter->installApp(&clockApp);
#endif
#if APP_NAVI_USE
    appCenter->installApp(&naviApp);
#endif
#if APP_INFO_USE
    appCenter->installApp(&infoApp);
#endif
#if APP_NOTI_USE
    appCenter->installApp(&notiApp);
#endif
#if APP_FILE_USE
    appCenter->installApp(&fileApp);
#endif
#if APP_SPORT_USE
    appCenter->installApp(&sportApp);
#endif
#if APP_CALENDAR_USE
    appCenter->installApp(&calendarApp);
#endif
#if APP_GPA_USE
    appCenter->installApp(&gpaApp);
#endif

    // 自启动APP
    appCenter->autoStart();

    // 优先显示屏幕 加快视觉上的开机时间
    appCenter->centerRoutine(&m_action);

    m_btn.init();
    m_imu.init(appCenter->sysCfg.mpu_order,
               appCenter->sysCfg.auto_calibration_mpu,
               &appCenter->imuCfg);
    m_gps.init();

    // 先初始化一次动作数据 防空指针
    buttonAction = m_btn.getAction();

    // 定义一个按键检测定时器
    xTimerAction = xTimerCreate("Action Check",
                                200 / portTICK_PERIOD_MS,
                                pdTRUE, (void *)0, actionCheckHandle);
    // // 定义一个GPS检测定时器
    // xTimerGPS = xTimerCreate("GPS Check",
    //                          2000 / portTICK_PERIOD_MS,
    //                          pdTRUE, (void *)0, gpsCheckHandle);
    xTimerStart(xTimerAction, 0);
    // xTimerStart(xTimerGPS, 0);
}

void loop()
{
    m_screen.routine();

    if (isCheckAction)
    {
        isCheckAction = false;
        buttonAction = m_btn.getAction();
        switch (buttonAction->btnAction)
        {
        case BTN_NONE:
            m_action.action = ACT_NONE;
            break;
        case BTN_BACK:
            m_action.action = ACT_BACK;
            break;
        case BTN_FORWARD:
            m_action.action = ACT_FORWARD;
            break;
        case BTN_LEFT:
            m_action.action = ACT_LEFT;
            break;
        case BTN_RIGHT:
            m_action.action = ACT_RIGHT;
            break;
        default:
            break;
        }
    }

    if (m_action.action == ACT_NONE)
    {
        switch (m_imu.update(200)->imuAction)
        {
        case IMU_NONE:
            m_action.action = ACT_NONE;
            break;
        case IMU_TURN_LEFT:
            m_action.action = ACT_LEFT;
            break;
        case IMU_TURN_RIGHT:
            m_action.action = ACT_RIGHT;
            break;
        case IMU_SHRUG_LEFT:
            m_action.action = ACT_BACK;
            break;
        case IMU_SHRUG_RIGHT:
            m_action.action = ACT_FORWARD;
            break;
        case IMU_UP:
            m_action.action = ACT_UP;
            break;
        case IMU_DOWN:
            m_action.action = ACT_DOWN;
            break;
        default:
            break;
        }
    }

    // if (isCheckGPS)
    // {
    //     isCheckGPS = false;
    m_gps.update();
    // }
    // m_imu.update(200);

    appCenter->centerRoutine(&m_action); // 运行当前进程
}