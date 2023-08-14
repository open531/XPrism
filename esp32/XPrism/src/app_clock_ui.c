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

static lv_obj_t *basicScr = NULL;

static lv_obj_t *clockHourLabel = NULL;
static lv_obj_t *clockMinuteLabel = NULL;
static lv_obj_t *clockSecondLabel = NULL;

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

void appClockUiDisplayBasicInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == basicScr)
    {
        return;
    }

    appClockUiDelete();
    lv_obj_clean(actObj);

    basicScr = lv_obj_create(NULL);
    lv_obj_add_style(basicScr, &defaultStyle, LV_STATE_DEFAULT);

    clockHourLabel = lv_label_create(basicScr);
    lv_obj_add_style(clockHourLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockHourLabel, true);
    lv_label_set_text(clockHourLabel, "00");

    clockMinuteLabel = lv_label_create(basicScr);
    lv_obj_add_style(clockMinuteLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockMinuteLabel, true);
    lv_label_set_text(clockMinuteLabel, "00");

    clockSecondLabel = lv_label_create(basicScr);
    lv_obj_add_style(clockSecondLabel, &numSmallStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockSecondLabel, true);
    lv_label_set_text(clockSecondLabel, "00");

    lv_obj_align(clockHourLabel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align(clockMinuteLabel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_align(clockSecondLabel, LV_ALIGN_CENTER, 0, 0);
}

void appClockUiDisplayBasic(struct Clock cloInfo, lv_scr_load_anim_t animType)
{
    appTimeUiDisplayInit(animType);
    lv_label_set_text_fmt(clockHourLabel, "%02d", cloInfo.hour);
    lv_label_set_text_fmt(clockMinuteLabel, "#ffa500 %02d#", cloInfo.minute);
    lv_label_set_text_fmt(clockSecondLabel, "#ffd700 %02d#", cloInfo.second);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(basicScr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(basicScr);
    }
}

void appClockUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_clean(basicScr);
        basicScr = NULL;
    }
}
