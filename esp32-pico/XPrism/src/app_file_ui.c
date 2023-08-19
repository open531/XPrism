#include "app_file_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *explorerScr;

static lv_obj_t *pathLabel;
static lv_obj_t *file1Label;
static lv_obj_t *file2Label;
static lv_obj_t *file3Label;
static lv_obj_t *file4Label;

#if LV_USE_TABLE
#endif

void appFileUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);
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

void appFileUiDisplayExplorerInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == explorerScr)
    {
        return;
    }

    appFileUiDelete();
    lv_obj_clean(actObj);

    explorerScr = lv_obj_create(NULL);
    lv_obj_add_style(explorerScr, &defaultStyle, LV_STATE_DEFAULT);

    pathLabel = lv_label_create(explorerScr);
    lv_obj_add_style(pathLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(pathLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(pathLabel, "/");

    // TODO: Add file list
}