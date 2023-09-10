#include "app_sport_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_source_han_sans_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *scr;

static lv_obj_t *latLabel;
static lv_obj_t *lonLabel;
static lv_obj_t *spdLabel;
static lv_obj_t *kmphLabel;
static lv_obj_t *satLabel;

static int getDegree(double latlon)
{
    return (int)latlon;
}

static int getMinute(double latlon)
{
    return (int)((latlon - getDegree(latlon)) * 60);
}

static int getSecond(double latlon)
{
    return (int)((((latlon - getDegree(latlon)) * 60) - getMinute(latlon)) * 60);
}

static char getLatNS(double lat)
{
    return lat > 0 ? 'N' : 'S';
}

static char getLonEW(double lon)
{
    return lon > 0 ? 'E' : 'W';
}

static int getDistance(double lat1, double lon1, double lat2, double lon2)
{
    double PI = 3.1415926535898;
    double EARTH_RADIUS = 6378.137;

    double radLat1 = lat1 * PI / 180.0;
    double radLat2 = lat2 * PI / 180.0;
    double a = radLat1 - radLat2;
    double b = lon1 * PI / 180.0 - lon2 * PI / 180.0;
    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +
                             cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
    s = s * EARTH_RADIUS;
    s = round(s * 10000) / 10000;
    return (int)(s * 1000);
}

void appSportUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&textStyle, &lv_font_source_han_sans_24);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numBigStyle, &lv_font_ubuntu_b_108);
}

void appSportUiDisplayInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == scr)
    {
        return;
    }

    appSportUiDelete();
    lv_obj_clean(actObj);

    scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &defaultStyle, LV_STATE_DEFAULT);

    latLabel = lv_label_create(scr);
    lv_obj_add_style(latLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(latLabel, "0°00'00.0\"N");

    lonLabel = lv_label_create(scr);
    lv_obj_add_style(lonLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(lonLabel, "0°00'00.0\"E");

    spdLabel = lv_label_create(scr);
    lv_obj_add_style(spdLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_text(spdLabel, "0.0");

    kmphLabel = lv_label_create(scr);
    lv_obj_add_style(kmphLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(kmphLabel, "km/h");

    satLabel = lv_label_create(scr);
    lv_obj_add_style(satLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(satLabel, "0");

    lv_obj_align(latLabel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(lonLabel, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_align(spdLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(kmphLabel, LV_ALIGN_CENTER, 0, 60);
    lv_obj_align(satLabel, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}

void appSportUiDisplay(struct Sport spoInfo, lv_scr_load_anim_t animType)
{
    appSportUiDisplayInit(animType);

    lv_label_set_text_fmt(latLabel, "%d°%d'%d\"%c",
                          getDegree(spoInfo.gpsLat), getMinute(spoInfo.gpsLat),
                          getSecond(spoInfo.gpsLat), getLatNS(spoInfo.gpsLat));
    lv_label_set_text_fmt(lonLabel, "%d°%d'%d\"%c",
                          getDegree(spoInfo.gpsLon), getMinute(spoInfo.gpsLon),
                          getSecond(spoInfo.gpsLon), getLonEW(spoInfo.gpsLon));
    lv_label_set_text_fmt(spdLabel, "%.1f", spoInfo.gpsSpd);
    lv_label_set_text_fmt(satLabel, "Sat:%d", spoInfo.gpsSat);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr, animType, 0, 0, false);
    }
    else
    {
        lv_scr_load(scr);
    }
}

void appSportUiDelete()
{
    if (scr != NULL)
    {
        lv_obj_clean(scr);
        scr = NULL;
    }
}