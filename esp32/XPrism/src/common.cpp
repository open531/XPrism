#include "common.h"
#include "network.h"

Buttons m_btn;
SdCard m_tf;
Network m_network;  // 网络连接
FlashFS m_flashCfg; // flash中的文件系统（替代原先的Preferences）
Display m_screen;   // 屏幕对象
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
