#include "app_time_ui.h"
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

static lv_obj_t *timeHourLabel = NULL;
static lv_obj_t *timeMinuteLabel = NULL;
static lv_obj_t *timeSecondLabel = NULL;
static lv_obj_t *dateLabel = NULL;

static const char weekDayCh[7][4] = {"日", "一", "二", "三", "四", "五", "六"};

void appTimeUiInit()
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

void appTimeUiDisplayInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == basicScr)
    {
        return;
    }

    appTimeUiDelete();
    lv_obj_clean(actObj);

    basicScr = lv_obj_create(NULL);
    lv_obj_add_style(basicScr, &defaultStyle, LV_STATE_DEFAULT);

    timeHourLabel = lv_label_create(basicScr);
    lv_obj_add_style(timeHourLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(timeHourLabel, true);
    lv_label_set_text(timeHourLabel, "00");

    timeMinuteLabel = lv_label_create(basicScr);
    lv_obj_add_style(timeMinuteLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(timeMinuteLabel, true);
    lv_label_set_text(timeMinuteLabel, "00");

    timeSecondLabel = lv_label_create(basicScr);
    lv_obj_add_style(timeSecondLabel, &numSmallStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(timeSecondLabel, true);
    lv_label_set_text(timeSecondLabel, "00");

    dateLabel = lv_label_create(basicScr);
    lv_obj_add_style(dateLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(dateLabel, "0000-00-00 星期一");

    lv_obj_align(timeHourLabel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align(timeMinuteLabel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_align(timeSecondLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(dateLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void appTimeUiDisplay(struct Time timInfo, lv_scr_load_anim_t animType)
{
    appTimeUiDisplayInit(animType);
    lv_label_set_text_fmt(timeHourLabel, "%02d", timInfo.hour);
    lv_label_set_text_fmt(timeMinuteLabel, "#ffa500 %02d#", timInfo.minute);
    lv_label_set_text_fmt(timeSecondLabel, "#ffd700 %02d#", timInfo.second);
    lv_label_set_text_fmt(dateLabel, "%04d-%02d-%02d 星期%s",
                          timInfo.year, timInfo.month, timInfo.day,
                          weekDayCh[timInfo.weekday]);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(basicScr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(basicScr);
    }
}

void appTimeUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_clean(basicScr);
        basicScr = NULL;
    }
}
