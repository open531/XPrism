#include "app_info_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *scr;

static lv_obj_t *ipLabel;
static lv_obj_t *gpsLabel;

void appInfoUiInit()
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

void appInfoUiDisplayInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == scr)
    {
        return;
    }

    appInfoUiDelete();
    lv_obj_clean(actObj);

    scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &defaultStyle, LV_STATE_DEFAULT);

    ipLabel = lv_label_create(scr);
    lv_obj_add_style(ipLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(ipLabel, "IP: ");

    gpsLabel = lv_label_create(scr);
    lv_obj_add_style(gpsLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(gpsLabel, "Lat: \nLon: \nAlt: \nSpd: \nSat:");

    lv_obj_align(ipLabel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(gpsLabel, LV_ALIGN_TOP_MID, 0, 100);
}

void appInfoUiDisplay(struct Info infInfo, lv_scr_load_anim_t animType)
{
    appInfoUiDisplayInit(animType);

    lv_label_set_text_fmt(ipLabel, "IP: %d.%d.%d.%d",
                          infInfo.ip[0], infInfo.ip[1], infInfo.ip[2], infInfo.ip[3]);
    lv_label_set_text_fmt(gpsLabel, "Lat: %f\nLon: %f\nAlt: %f\nSpd: %f\nSat: %d",
                          infInfo.gpsLat, infInfo.gpsLon, infInfo.gpsAlt,
                          infInfo.gpsSpd, infInfo.gpsSat);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr, animType, 0, 0, false);
    }
    else
    {
        lv_scr_load(scr);
    }
}

void appInfoUiDelete()
{
    if (scr != NULL)
    {
        lv_obj_clean(scr);
        scr = NULL;
    }
}