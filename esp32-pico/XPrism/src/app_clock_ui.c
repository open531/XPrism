#include "app_clock_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *stopwatchScr = NULL;

static lv_obj_t *clockStopwatchLabel = NULL;

void appClockUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&textStyle, &lv_font_wqy_microhei_24);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numBigStyle, &lv_font_ubuntu_b_108);
}

void appClockUiDisplayStopwatchInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == stopwatchScr)
    {
        return;
    }

    appClockUiDelete();
    lv_obj_clean(actObj);

    stopwatchScr = lv_obj_create(NULL);
    lv_obj_add_style(stopwatchScr, &defaultStyle, LV_STATE_DEFAULT);

    clockStopwatchLabel = lv_label_create(stopwatchScr);
    lv_obj_add_style(clockStopwatchLabel, &numSmallStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockStopwatchLabel, true);
    lv_label_set_text(clockStopwatchLabel, "00:00.00");

    lv_obj_align(clockStopwatchLabel, LV_ALIGN_CENTER, 0, 0);
}

void appClockUiDisplayStopwatch(unsigned long stopwatchInfo, lv_scr_load_anim_t animType)
{
    appClockUiDisplayStopwatchInit(animType);

    lv_label_set_text_fmt(clockStopwatchLabel, "%02d:%02d.%02d",
                          stopwatchInfo / 60000,
                          (stopwatchInfo % 60000) / 1000,
                          (stopwatchInfo % 1000) / 10);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(stopwatchScr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(stopwatchScr);
    }
}

void appClockUiDelete()
{
    if (stopwatchScr != NULL)
    {
        lv_obj_clean(stopwatchScr);
        stopwatchScr = NULL;
    }
}
