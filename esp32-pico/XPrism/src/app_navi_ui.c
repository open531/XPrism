#include "app_navi_ui.h"
#include "icons_navi.h"
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

static lv_obj_t *latLabel;
static lv_obj_t *lonLabel;
static lv_obj_t *arrowImg;
static lv_obj_t *textLabel;

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
    return (int)s;
}

void appNaviUiInit()
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

void appNaviUiDisplayInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == scr)
    {
        return;
    }

    appNaviUiDelete();
    lv_obj_clean(actObj);

    scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &defaultStyle, LV_STATE_DEFAULT);

    latLabel = lv_label_create(scr);
    lv_obj_add_style(latLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(latLabel, "0°00'00.0\"N");

    lonLabel = lv_label_create(scr);
    lv_obj_add_style(lonLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(lonLabel, "0°00'00.0\"E");

    arrowImg = lv_img_create(scr);
    lv_img_set_src(arrowImg, &icon_empty);

    textLabel = lv_label_create(scr);
    lv_obj_add_style(textLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(textLabel, "请使用手机进行导航");

    lv_obj_align(latLabel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(lonLabel, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_align(arrowImg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(textLabel, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void appNaviUiDisplay(struct Navi navInfo, lv_scr_load_anim_t animType)
{
    appNaviUiDisplayInit(animType);

    if (navInfo.instructionLength > 0)
    {
        lv_label_set_text_fmt(latLabel, "%d°%d'%d\"%c",
                              getDegree(navInfo.currLat), getMinute(navInfo.currLat),
                              getSecond(navInfo.currLat), getLatNS(navInfo.currLat));
        lv_label_set_text_fmt(lonLabel, "%d°%d'%d\"%c",
                              getDegree(navInfo.currLon), getMinute(navInfo.currLon),
                              getSecond(navInfo.currLon), getLonEW(navInfo.currLon));
        lv_label_set_text(textLabel, navInfo.instructionText[navInfo.currInstruction]);
        switch (navInfo.instructionSign[navInfo.currInstruction])
        {
        case -8:
            lv_img_set_src(arrowImg, &icon_left_uturn);
            break;
        case -7:
        case -3:
        case -2:
            lv_img_set_src(arrowImg, &icon_left);
            break;
        case -1:
            lv_img_set_src(arrowImg, &icon_left_slight);
            break;
        case 0:
            lv_img_set_src(arrowImg, &icon_continue);
            break;
        case 1:
            lv_img_set_src(arrowImg, &icon_right_slight);
            break;
        case 2:
        case 3:
        case 7:
            lv_img_set_src(arrowImg, &icon_right);
            break;
        case 8:
            lv_img_set_src(arrowImg, &icon_right_uturn);
            break;
        case 4:
        case 5:
        case 6:
            lv_img_set_src(arrowImg, &icon_finish);
            break;
        default:
            lv_img_set_src(arrowImg, &icon_empty);
            break;
        }
    }
    else
    {
        lv_label_set_text_fmt(latLabel, "%d°%d'%d\"%c",
                              getDegree(navInfo.currLat), getMinute(navInfo.currLat),
                              getSecond(navInfo.currLat), getLatNS(navInfo.currLat));
        lv_label_set_text_fmt(lonLabel, "%d°%d'%d\"%c",
                              getDegree(navInfo.currLon), getMinute(navInfo.currLon),
                              getSecond(navInfo.currLon), getLonEW(navInfo.currLon));
        lv_img_set_src(arrowImg, &icon_empty);
        lv_label_set_text(textLabel, "请使用手机进行导航");
    }

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(scr);
    }
}

void appNaviUiDelete()
{
    if (scr != NULL)
    {
        lv_obj_clean(scr);
        scr = NULL;
    }
}