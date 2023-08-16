#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "common.h"
#include "app_center.h"

#include "app_conf.h"

#include <SPIFFS.h>
#include <esp32-hal.h>
#include <esp32-hal-timer.h>

static bool isCheckAction = false;

Action *act_info;     // 存放mpu6050返回的数据
AppCenter *appCenter; // APP控制器

TimerHandle_t xTimerAction = NULL;
void actionCheckHandle(TimerHandle_t xTimer)
{
    // 标志需要检测动作
    isCheckAction = true;
}

void my_print(const char *buf)
{
    Serial.printf("%s", buf);
    Serial.flush();
}

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

    appCenter->read_config(&appCenter->sys_cfg);

    /*** Init screen ***/
    m_screen.init(0, 100);

    /*** Init micro SD-Card ***/
    m_tf.init();

    lv_fs_fatfs_init();

    appCenter->init();

// 将APP"安装"到controller里
#if APP_TIME_USE
    appCenter->app_install(&timeApp);
#endif
#if APP_WEATHER_USE
    appCenter->app_install(&weatherApp);
#endif
#if APP_CLOCK_USE
    appCenter->app_install(&clockApp);
#endif
#if APP_NAVI_USE
    appCenter->app_install(&naviApp);
#endif

    m_btn.init();

    // 自启动APP
    appCenter->app_auto_start();

    // 优先显示屏幕 加快视觉上的开机时间
    appCenter->main_process(&m_btn.action_info);

    // 先初始化一次动作数据 防空指针
    act_info = m_btn.getAction();

    // 定义一个mpu6050的动作检测定时器
    xTimerAction = xTimerCreate("Action Check",
                                200 / portTICK_PERIOD_MS,
                                pdTRUE, (void *)0, actionCheckHandle);
    xTimerStart(xTimerAction, 0);
}

void loop()
{
    m_screen.routine();

    if (isCheckAction)
    {
        isCheckAction = false;
        act_info = m_btn.getAction();
    }
    appCenter->main_process(act_info); // 运行当前进程
}