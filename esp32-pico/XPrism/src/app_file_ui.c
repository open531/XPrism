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
static lv_style_t style_line;

static lv_obj_t *explorerScr;

static lv_obj_t *pathLabel;
static lv_obj_t *currFile1Label;
static lv_obj_t *currFile2Label;
static lv_obj_t *currFile3Label;
static lv_obj_t *prevFile1Label;
static lv_obj_t *prevFile2Label;
static lv_obj_t *prevFile3Label;

static lv_obj_t *selectPanel;

static int check;

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

void appFileUiDisplayExplorerInit(const char *path, const char *file1Name,
                                  const char *file2Name, const char *file3Name,
                                  int temp, lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    // if (actObj == explorerScr)
    // {
    //     return;
    // }

    // appFileUiDelete();
    lv_obj_clean(actObj);

    check = temp;

    explorerScr = lv_obj_create(NULL);
    lv_obj_add_style(explorerScr, &defaultStyle, LV_STATE_DEFAULT);

    pathLabel = lv_label_create(explorerScr);
    lv_obj_add_style(pathLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(pathLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(pathLabel, 200);
    lv_label_set_text(pathLabel, path);

    selectPanel = lv_obj_create(explorerScr);
    lv_obj_add_style(selectPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_width(selectPanel, 200);
    lv_obj_set_height(selectPanel, 60);

    prevFile1Label = lv_label_create(explorerScr);
    lv_obj_add_style(prevFile1Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevFile1Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(prevFile1Label, 180);
    lv_label_set_text(prevFile1Label, file1Name);

    prevFile2Label = lv_label_create(explorerScr);
    lv_obj_add_style(prevFile2Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevFile2Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(prevFile2Label, 180);
    lv_label_set_text(prevFile2Label, file2Name);

    prevFile3Label = lv_label_create(explorerScr);
    lv_obj_add_style(prevFile3Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevFile3Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(prevFile3Label, 180);
    lv_label_set_text(prevFile3Label, file3Name);

    lv_obj_align(pathLabel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(selectPanel, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_align(prevFile1Label, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_align(prevFile2Label, LV_ALIGN_TOP_MID, 0, 120);
    lv_obj_align(prevFile3Label, LV_ALIGN_TOP_MID, 0, 180);

    lv_scr_load_anim(explorerScr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
}

void appFileUiDisplayExplorer(const char *path, const char *file1Name,
                              const char *file2Name, const char *file3Name,
                              int temp, lv_scr_load_anim_t animType, bool force)
{
    if (force == true)
    {
        appFileUiDisplayExplorerInit(path, file1Name, file2Name, file3Name,
                                     temp, animType);
        return;
    }

    if (check == temp)
    {
        return;
    }

    check = temp;

    int currStartX, currEndX, prevStartX, prevEndX;

    if (animType == LV_SCR_LOAD_ANIM_MOVE_LEFT)
    {
        currStartX = -120 - 100;
        currEndX = 0;
        prevStartX = 0;
        prevEndX = 120 + 100;
    }
    else
    {
        currStartX = 120 + 100;
        currEndX = 0;
        prevStartX = 0;
        prevEndX = -120 - 100;
    }
    currFile1Label = lv_label_create(explorerScr);
    lv_obj_add_style(currFile1Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currFile1Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(currFile1Label, 180);
    lv_label_set_text(currFile1Label, file1Name);

    currFile2Label = lv_label_create(explorerScr);
    lv_obj_add_style(currFile2Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currFile2Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(currFile2Label, 180);
    lv_label_set_text(currFile2Label, file2Name);

    currFile3Label = lv_label_create(explorerScr);
    lv_obj_add_style(currFile3Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currFile3Label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(currFile3Label, 180);
    lv_label_set_text(currFile3Label, file3Name);

    lv_obj_align(currFile1Label, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_align(currFile2Label, LV_ALIGN_TOP_MID, 0, 120);
    lv_obj_align(currFile3Label, LV_ALIGN_TOP_MID, 0, 180);

    static lv_anim_t currFile1Anim;
    lv_anim_init(&currFile1Anim);
    lv_anim_set_exec_cb(&currFile1Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currFile1Anim, currFile1Label);
    lv_anim_set_values(&currFile1Anim, currStartX, currEndX);
    lv_anim_set_time(&currFile1Anim, 300);
    lv_anim_set_path_cb(&currFile1Anim, lv_anim_path_linear);

    static lv_anim_t currFile2Anim;
    lv_anim_init(&currFile2Anim);
    lv_anim_set_exec_cb(&currFile2Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currFile2Anim, currFile2Label);
    lv_anim_set_values(&currFile2Anim, currStartX, currEndX);
    lv_anim_set_time(&currFile2Anim, 300);
    lv_anim_set_path_cb(&currFile2Anim, lv_anim_path_linear);

    static lv_anim_t currFile3Anim;
    lv_anim_init(&currFile3Anim);
    lv_anim_set_exec_cb(&currFile3Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currFile3Anim, currFile3Label);
    lv_anim_set_values(&currFile3Anim, currStartX, currEndX);
    lv_anim_set_time(&currFile3Anim, 300);
    lv_anim_set_path_cb(&currFile3Anim, lv_anim_path_linear);

    static lv_anim_t prevFile1Anim;
    lv_anim_init(&prevFile1Anim);
    lv_anim_set_exec_cb(&prevFile1Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevFile1Anim, prevFile1Label);
    lv_anim_set_values(&prevFile1Anim, prevStartX, prevEndX);
    lv_anim_set_time(&prevFile1Anim, 300);
    lv_anim_set_path_cb(&prevFile1Anim, lv_anim_path_linear);

    static lv_anim_t prevFile2Anim;
    lv_anim_init(&prevFile2Anim);
    lv_anim_set_exec_cb(&prevFile2Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevFile2Anim, prevFile2Label);
    lv_anim_set_values(&prevFile2Anim, prevStartX, prevEndX);
    lv_anim_set_time(&prevFile2Anim, 300);
    lv_anim_set_path_cb(&prevFile2Anim, lv_anim_path_linear);

    static lv_anim_t prevFile3Anim;
    lv_anim_init(&prevFile3Anim);
    lv_anim_set_exec_cb(&prevFile3Anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevFile3Anim, prevFile3Label);
    lv_anim_set_values(&prevFile3Anim, prevStartX, prevEndX);
    lv_anim_set_time(&prevFile3Anim, 300);
    lv_anim_set_path_cb(&prevFile3Anim, lv_anim_path_linear);

    lv_anim_start(&currFile1Anim);
    lv_anim_start(&currFile2Anim);
    lv_anim_start(&currFile3Anim);
    lv_anim_start(&prevFile1Anim);
    lv_anim_start(&prevFile2Anim);
    lv_anim_start(&prevFile3Anim);
    // ANIEND_WAIT
    // lv_task_handler();

    lv_obj_del(prevFile1Label);
    lv_obj_del(prevFile2Label);
    lv_obj_del(prevFile3Label);
    prevFile1Label = currFile1Label;
    prevFile2Label = currFile2Label;
    prevFile3Label = currFile3Label;
}

void appFileUiDelete()
{
    if (explorerScr != NULL)
    {
        lv_obj_clean(explorerScr);
        explorerScr = NULL;
    }
}