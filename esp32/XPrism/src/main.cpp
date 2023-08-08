#include <Arduino.h>

#include "lv_port_indev.h"
#include "lv_port_fatfs.h"

#include "system.h"
#include "pages_main.h"
#include "pages_conf.h"

#include <SPIFFS.h>
#include <esp32-hal.h>
#include <esp32-hal-timer.h>

static bool isCheckAction = false;

/*** Component objects **7*/
Action *act_info; // 存放当前按钮输入的动作信息
XPages *xpages;   // APP控制器

TaskHandle_t handleTaskLvgl;

void TaskLvglUpdate(void *parameter)
{
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    for (;;)
    {
        AIO_LVGL_OPERATE_LOCK(lv_timer_handler();)
        vTaskDelay(5);
    }
}

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

    Serial.println(F("\n==XPrism==\n"));
    Serial.flush();
    // MAC ID可用作芯片唯一标识
    Serial.print(F("ChipID(EfuseMac): "));
    Serial.println(ESP.getEfuseMac());
    // flash运行模式
    // Serial.print(F("FlashChipMode: "));
    // Serial.println(ESP.getFlashChipMode());
    // Serial.println(F("FlashChipMode value: FM_QIO = 0, FM_QOUT = 1, FM_DIO = 2, FM_DOUT = 3, FM_FAST_READ = 4, FM_SLOW_READ = 5, FM_UNKNOWN = 255"));

    xpages = new XPages(); // APP控制器

    // 需要放在Setup里初始化
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // config_read(NULL, &g_cfg);   // 旧的配置文件读取方式
    xpages->read_config(&xpages->sys_cfg);
    xpages->read_config(&xpages->mpu_cfg);
    xpages->read_config(&xpages->led_cfg);

    /*** Init screen ***/
    screen.init(xpages->sys_cfg.rotation,
                xpages->sys_cfg.backLight);

    /*** Init on-board RGB ***/
    led.init();
    led.setBrightness(0.05).setRGB(0, 64, 64);

    /*** Init ambient-light sensor ***/
    // ambLight.init(ONE_TIME_H_RESOLUTION_MODE);

    /*** Init micro SD-Card ***/
    tf.init();

    lv_fs_fatfs_init();

    // Update display in parallel thread.
    // BaseType_t taskLvglReturned = xTaskCreate(
    //     TaskLvglUpdate,
    //     "LvglThread",
    //     8 * 1024,
    //     nullptr,
    //     TASK_LVGL_PRIORITY,
    //     &handleTaskLvgl);
    // if (taskLvglReturned != pdPASS)
    // {
    //     Serial.println("taskLvglReturned != pdPASS");
    // }
    // else
    // {
    //     Serial.println("taskLvglReturned == pdPASS");
    // }

#if LV_USE_LOG
    lv_log_register_print_cb(my_print);
#endif /*LV_USE_LOG*/

    xpages->init();

    // 将APP"安装"到controller里
#if INSTALL_PAGE_TIME
    xpages->app_install(&page_time);
#endif

#if INSTALL_PAGE_WEATHER
    xpages->app_install(&page_weather);
#endif

#if INSTALL_PAGE_FILE
    xpages->app_install(&page_file);
#endif

    // 自启动APP
    xpages->app_auto_start();

    // 优先显示屏幕 加快视觉上的开机时间
    xpages->main_process(btns.get_action());

    /*** Init IMU as input device ***/
    // lv_port_indev_init();

    mpu.init(xpages->sys_cfg.mpu_order,
             xpages->sys_cfg.auto_calibration_mpu,
             &xpages->mpu_cfg); // 初始化比较耗时

    /*** 以此作为MPU6050初始化完成的标志 ***/
    LEDConfig *led_cfg = &xpages->led_cfg;
    // 初始化RGB灯 HSV色彩模式
    LEDParam led_setting = {LED_MODE_HSV,
                            led_cfg->min_value_0, led_cfg->min_value_1, led_cfg->min_value_2,
                            led_cfg->max_value_0, led_cfg->max_value_1, led_cfg->max_value_2,
                            led_cfg->step_0, led_cfg->step_1, led_cfg->step_2,
                            led_cfg->min_brightness, led_cfg->max_brightness,
                            led_cfg->brightness_step, led_cfg->time};
    // 运行RGB任务
    set_led_and_run(&led_setting, RUN_MODE_TASK);

    // 先初始化一次动作数据 防空指针
    // act_info = mpu.getAction();
    // 定义一个mpu6050的动作检测定时器
    xTimerAction = xTimerCreate("Action Check", 200 / portTICK_PERIOD_MS, pdTRUE, (void *)0, actionCheckHandle);
    xTimerStart(xTimerAction, 0);
}

void loop()
{
    screen.routine();

#ifdef PEAK
    if (!mpu.Encoder_GetIsPush())
    {
        Serial.println("mpu.Encoder_GetIsPush()1");
        delay(1000);
        if (!mpu.Encoder_GetIsPush())
        {
            Serial.println("mpu.Encoder_GetIsPush()2");
            // 适配Peak的关机功能
            digitalWrite(CONFIG_POWER_EN_PIN, LOW);
        }
    }
#endif
    if (isCheckAction)
    {
        isCheckAction = false;
        act_info = btns.get_action();
    }
    xpages->main_process(act_info); // 运行当前进程
                                    // Serial.println(ambLight.getLux() / 50.0);
                                    // led.setBrightness(ambLight.getLux() / 500.0);
}