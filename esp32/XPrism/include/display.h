#ifndef DISPLAY_H
#define DISPLAY_H

#include <lvgl.h>
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

Arduino_DataBus *bus = create_default_Arduino_DataBus();
Arduino_GFX *gfx = new Arduino_ST7789(bus,
									  1 /* RST */,
									  0 /* rotation */,
									  true /* IPS */,
									  240 /* width */,
									  240 /* height */,
									  35 /* col offset 1 */,
									  0 /* row offset 1 */,
									  35 /* col offset 2 */,
									  0 /* row offset 2 */);
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

class Display
{
public:
	void init();
};

#endif