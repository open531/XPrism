#ifndef COMMON_H
#define COMMON_H

#define GET_SYS_MILLIS xTaskGetTickCount // 获取系统毫秒数

#include "Arduino.h"
#include "flash_fs.h"
#include "sd_card.h"
#include "display.h"
#include "btn.h"
#include "network.h"
#include "gps.h"
#include <TFT_eSPI.h>

// SD_Card
#define SD_SCK 14
#define SD_MISO 26
#define SD_MOSI 13
#define SD_SS 5

extern Buttons m_btn; // 原则上只提供给主程序调用
extern SdCard m_tf;
// extern Config g_cfg;       // 全局配置文件
extern Network m_network;  // 网络连接
extern FlashFS m_flashCfg; // flash中的文件系统（替代原先的Preferences）
extern Display m_screen;   // 屏幕对象
extern GPS m_gps;          // GPS对象

boolean doDelayMillisTime(unsigned long interval,
                          unsigned long *previousMillis,
                          boolean state);

// 屏幕尺寸
#define SCREEN_HOR_RES 240 // 水平
#define SCREEN_VER_RES 240 // 竖直

// TFT屏幕接口
#define LCD_BL_PIN 5

#define LCD_BL_PWM_CHANNEL 0

// lvgl 操作的锁
extern SemaphoreHandle_t lvgl_mutex;
// LVGL操作的安全宏（避免脏数据）
#define AIO_LVGL_OPERATE_LOCK(CODE)                          \
    if (pdTRUE == xSemaphoreTake(lvgl_mutex, portMAX_DELAY)) \
    {                                                        \
        CODE;                                                \
        xSemaphoreGive(lvgl_mutex);                          \
    }

struct SysUtilConfig
{
    String ssid_0;
    String password_0;
    String ssid_1;
    String password_1;
    String ssid_2;
    String password_2;
    String auto_start_app;        // 开机自启的APP名字
    uint8_t power_mode;           // 功耗模式（0为节能模式 1为性能模式）
    uint8_t backLight;            // 屏幕亮度（1-100）
    uint8_t rotation;             // 屏幕旋转方向
    uint8_t auto_calibration_mpu; // 是否自动校准陀螺仪 0关闭自动校准 1打开自动校准
    uint8_t mpu_order;            // 操作方向
};

#include <TFT_eSPI.h>
/*
TFT pins should be set in path/to/Arduino/libraries/TFT_eSPI/User_Setups/Setup24_ST7789.h
*/
extern TFT_eSPI *m_tft;

#endif