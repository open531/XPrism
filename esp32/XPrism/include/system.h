#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "button.h"
#include "display.h"
#include "flash_fs.h"
#include "network.h"
#include "sd_card.h"

extern Buttons m_button;
// extern Display m_display;
extern FlashFS m_flashFS;
extern Network m_network;
extern SDCard m_sdCard;
extern TFT_eSPI *m_tft;

extern SemaphoreHandle_t lvgl_mutex;
#define LVGL_OPERATE_LOCK(CODE)                              \
    if (pdTRUE == xSemaphoreTake(lvgl_mutex, portMAX_DELAY)) \
    {                                                        \
        CODE;                                                \
        xSemaphoreGive(lvgl_mutex);                          \
    }

struct SysUtilCfg
{
    String ssid;
    String password;
    String autoStart;
    uint8_t brightness;
    uint8_t volume;
    uint8_t powerMode;
    uint8_t rotation;
};

#endif