#include "app_noti_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_source_han_sans_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *scr = NULL;

static lv_obj_t *currTitleLabel = NULL;
static lv_obj_t *currTextLabel = NULL;
static lv_obj_t *currTimeLabel = NULL;
static lv_obj_t *prevTitleLabel = NULL;
static lv_obj_t *prevTextLabel = NULL;
static lv_obj_t *prevTimeLabel = NULL;

static struct Notification currNotInfo;

void appNotiUiInit()
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

void appNotiUiDisplayInit(struct Notification notInfo)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == scr)
    {
        return;
    }

    appNotiUiDelete();
    lv_obj_clean(actObj);

    scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &defaultStyle, LV_STATE_DEFAULT);

    currNotInfo = notInfo;

    prevTitleLabel = lv_label_create(scr);
    lv_obj_add_style(prevTitleLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevTitleLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(prevTitleLabel, 200);
    lv_label_set_text(prevTitleLabel, notInfo.title);

    prevTextLabel = lv_label_create(scr);
    lv_obj_add_style(prevTextLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevTextLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(prevTextLabel, 200);
    lv_label_set_text(prevTextLabel, notInfo.text);

    prevTimeLabel = lv_label_create(scr);
    lv_obj_add_style(prevTimeLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(prevTimeLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(prevTimeLabel, 200);
    lv_label_set_text(prevTimeLabel, notInfo.timeStr);

    lv_obj_align(prevTitleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align(prevTextLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(prevTimeLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
}

void appNotiUiDisplay(struct Notification notInfo,
                      lv_scr_load_anim_t animType, bool force)
{
    if (force == true)
    {
        appNotiUiDisplayInit(notInfo);
        return;
    }

    if (notInfo.time == currNotInfo.time)
    {
        return;
    }

    currNotInfo = notInfo;

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

    currTitleLabel = lv_label_create(scr);
    lv_obj_add_style(currTitleLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currTitleLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(currTitleLabel, 200);
    lv_label_set_text(currTitleLabel, notInfo.title);

    currTextLabel = lv_label_create(scr);
    lv_obj_add_style(currTextLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currTextLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(currTextLabel, 200);
    lv_label_set_text(currTextLabel, notInfo.text);

    currTimeLabel = lv_label_create(scr);
    lv_obj_add_style(currTimeLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(currTimeLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(currTimeLabel, 200);
    lv_label_set_text(currTimeLabel, notInfo.timeStr);

    lv_obj_align(currTitleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align(currTextLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(currTimeLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

    static lv_anim_t currTitleAnim;
    lv_anim_init(&currTitleAnim);
    lv_anim_set_exec_cb(&currTitleAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currTitleAnim, currTitleLabel);
    lv_anim_set_values(&currTitleAnim, currStartX, currEndX);
    lv_anim_set_time(&currTitleAnim, 300);
    lv_anim_set_path_cb(&currTitleAnim, lv_anim_path_linear);

    static lv_anim_t currTextAnim;
    lv_anim_init(&currTextAnim);
    lv_anim_set_exec_cb(&currTextAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currTextAnim, currTextLabel);
    lv_anim_set_values(&currTextAnim, currStartX, currEndX);
    lv_anim_set_time(&currTextAnim, 300);
    lv_anim_set_path_cb(&currTextAnim, lv_anim_path_linear);

    static lv_anim_t currTimeAnim;
    lv_anim_init(&currTimeAnim);
    lv_anim_set_exec_cb(&currTimeAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currTimeAnim, currTimeLabel);
    lv_anim_set_values(&currTimeAnim, currStartX, currEndX);
    lv_anim_set_time(&currTimeAnim, 300);
    lv_anim_set_path_cb(&currTimeAnim, lv_anim_path_linear);

    static lv_anim_t prevTitleAnim;
    lv_anim_init(&prevTitleAnim);
    lv_anim_set_exec_cb(&prevTitleAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevTitleAnim, prevTitleLabel);
    lv_anim_set_values(&prevTitleAnim, prevStartX, prevEndX);
    lv_anim_set_time(&prevTitleAnim, 300);
    lv_anim_set_path_cb(&prevTitleAnim, lv_anim_path_linear);

    static lv_anim_t prevTextAnim;
    lv_anim_init(&prevTextAnim);
    lv_anim_set_exec_cb(&prevTextAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevTextAnim, prevTextLabel);
    lv_anim_set_values(&prevTextAnim, prevStartX, prevEndX);
    lv_anim_set_time(&prevTextAnim, 300);
    lv_anim_set_path_cb(&prevTextAnim, lv_anim_path_linear);

    static lv_anim_t prevTimeAnim;
    lv_anim_init(&prevTimeAnim);
    lv_anim_set_exec_cb(&prevTimeAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevTimeAnim, prevTimeLabel);
    lv_anim_set_values(&prevTimeAnim, prevStartX, prevEndX);
    lv_anim_set_time(&prevTimeAnim, 300);
    lv_anim_set_path_cb(&prevTimeAnim, lv_anim_path_linear);

    lv_anim_start(&currTitleAnim);
    lv_anim_start(&currTextAnim);
    lv_anim_start(&currTimeAnim);
    lv_anim_start(&prevTitleAnim);
    lv_anim_start(&prevTextAnim);
    lv_anim_start(&prevTimeAnim);
    // ANIEND_WAIT
    // lv_task_handler();

    lv_obj_del(prevTitleLabel);
    lv_obj_del(prevTextLabel);
    lv_obj_del(prevTimeLabel);
    prevTitleLabel = currTitleLabel;
    prevTextLabel = currTextLabel;
    prevTimeLabel = currTimeLabel;
}

void appNotiUiDelete()
{
    if (scr != NULL)
    {
        lv_obj_clean(scr);
        scr = NULL;
    }
}