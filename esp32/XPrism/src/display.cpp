#include "display.h"
#include <Arduino_GFX_Library.h>

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void Display::init()
{
#ifdef GFX_EXTRA_PRE_INIT
	GFX_EXTRA_PRE_INIT();
#endif

	// Init Display
	if (!gfx->begin())
	{
		Serial.println("gfx->begin() failed!");
	}
	gfx->fillScreen(BLACK);

#ifdef GFX_BL
	pinMode(GFX_BL, OUTPUT);
	digitalWrite(GFX_BL, HIGH);
#endif

	lv_init();

	screenWidth = gfx->width();
	screenHeight = gfx->height();

	disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

	if (!disp_draw_buf)
	{
		Serial.println("LVGL disp_draw_buf allocate failed!");
	}
	else
	{
		lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 40);

		// Initialize the display
		lv_disp_drv_init(&disp_drv);
		// Change the following line to your display resolution
		disp_drv.hor_res = screenWidth;
		disp_drv.ver_res = screenHeight;
		disp_drv.flush_cb = my_disp_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_drv_register(&disp_drv);

		// Initialize the (dummy) input device driver
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		lv_indev_drv_register(&indev_drv);

		// Create simple label
		lv_obj_t *label = lv_label_create(lv_scr_act());
		lv_label_set_text(label, "Hello Arduino! (V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

		Serial.println("Setup done");
	}
}
