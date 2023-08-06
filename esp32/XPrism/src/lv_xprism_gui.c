#include "lv_xprism_color.h"
#include "lv_xprism_gui.h"
#include "display.h"
#include "icons.h"

void lv_xprism_gui(void)
{
	// set style
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_radius(&style, 5);
	lv_style_set_bg_opa(&style, LV_OPA_COVER);
	lv_style_set_bg_color(&style, LV_COLOR_BLACK);
}