#include "app_clock_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_20);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *basicScr = NULL;

static lv_obj_t *ClockLabel=NULL;

void appClockUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&textStyle, &lv_font_wqy_microhei_20);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numBigStyle, &lv_font_ubuntu_b_108);
}

void appClockUiDisplayBasicInit()
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

    ClockLabel=lv_label_create(basicScr);
    lv_obj_add_style(ClockLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(ClockLabel, "00:00:00");
    lv_obj_set_pos(ClockLabel, 0, 0);

}

void appClockUiDisplayBasic(struct Clock cloInfo)
{

    lv_label_set_text_fmt(ClockLabel,"%d:%d:%d",cloInfo.hour,cloInfo.minute,cloInfo.second);

}


void appClockUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_del(basicScr);
        basicScr = NULL;
    }

}
