#include "common.h"

Action m_action;          // 原则上只提供给主程序调用
Buttons m_btn;            // 按键
SdCard m_tf;              // SD卡
Network m_network;        // 网络连接
FlashFS m_flashCfg;       // flash中的文件系统（替代原先的Preferences）
Display m_screen;         // 屏幕对象
GPS m_gps;                // GPS对象
IMU m_imu;                // IMU对象
uint64_t m_timestamp = 0; // 时间戳
TFT_eSPI *m_tft = new TFT_eSPI(SCREEN_HOR_RES, SCREEN_VER_RES);

// lvgl handle的锁
SemaphoreHandle_t lvgl_mutex = xSemaphoreCreateMutex();

boolean doDelayMillisTime(unsigned long interval, unsigned long *previousMillis, boolean state)
{
    unsigned long currentMillis = GET_SYS_MILLIS();
    if (currentMillis - *previousMillis >= interval)
    {
        *previousMillis = currentMillis;
        state = !state;
    }
    return state;
}
