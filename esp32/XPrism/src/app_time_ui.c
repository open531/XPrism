#include "app_time_ui.h"
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

static lv_obj_t *TimeLabel=NULL;
static lv_obj_t *DateLabel=NULL;

static const char weekDayCh[7][4] = {"日", "一", "二", "三", "四", "五", "六"};

void appTimeUiInit()
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

void appTimeUiDisplayBasicInit()
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

    TimeLabel=lv_label_create(basicScr);
    lv_obj_add_style(TimeLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(TimeLabel, "00:00:00    1月1日    周一");
    lv_obj_set_pos(TimeLabel, 0, 0);

}

void appTimeUiDisplayBasic(struct Time timInfo)
{

    lv_label_set_text_fmt(TimeLabel,"%2d:%2d:%2d    %2d月%2d日    周%s",timInfo.hour,timInfo.minute,timInfo.second, timInfo.month, timInfo.day,
                          weekDayCh[timInfo.weekday]);

}
void appTimeUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_del(basicScr);
        basicScr = NULL;
    }

}
