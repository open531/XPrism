#ifndef DISPLAY_H
#define DISPLAY_H

#include <lvgl.h>

#define PIN_DISPLAY_BLK 32

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define LV_HOR_RES_MAX_LEN 80

// class Display
// {
// public:
void screenInit(uint8_t rotation, uint8_t backLight);
void screenRoutine();
void screenSetBackLight(float brightness);
// };

#endif
