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
    lv_label_set_text_fmt(gpsLabel, "Lat: %d°%d'%d\"%c\nLon: %d°%d'%d\"%c\nAlt: %f m\nSpd: %f km/h\nSat: %d",
                          getDegree(infInfo.gpsLat), getMinute(infInfo.gpsLat),
                          getSecond(infInfo.gpsLat), getLatNS(infInfo.gpsLat),
                          getDegree(infInfo.gpsLon), getMinute(infInfo.gpsLon),
                          getSecond(infInfo.gpsLon), getLonEW(infInfo.gpsLon),
                          infInfo.gpsAlt, infInfo.gpsSpd, infInfo.gpsSat);

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