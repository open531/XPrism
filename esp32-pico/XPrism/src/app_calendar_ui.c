#include "app_calendar_ui.h"
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

static lv_obj_t *calendar;
static lv_obj_t *dateLabel;

void appCalendarUiInit()
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

void appCalendarUiDisplayInit(struct Date today, lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == scr)
    {
        return;
    }

    appCalendarUiDelete();
    lv_obj_clean(actObj);

    scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &defaultStyle, LV_STATE_DEFAULT);

    calendar = lv_calendar_create(scr);
    lv_obj_set_size(calendar, 240, 200);
    lv_calendar_set_today_date(calendar, today.year, today.month, today.day);
    lv_calendar_set_showed_date(calendar, today.year, today.month);

    dateLabel = lv_label_create(scr);
    lv_obj_add_style(dateLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(dateLabel, "%d-%02d", today.year, today.month);

    lv_obj_align(calendar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_align(dateLabel, LV_ALIGN_TOP_MID, 0, 0);
}

void appCalendarUiDisplay(struct Date today,
                          struct Date date,
                          struct Date *highlight,
                          lv_scr_load_anim_t animType)
{
    appCalendarUiDisplayInit(today, animType);

    lv_calendar_set_showed_date(calendar, date.year, date.month);

    lv_label_set_text_fmt(dateLabel, "%d-%02d", date.year, date.month);

    // if (highlight != NULL)
    // {
    //     lv_calendar_set_highlighted_dates(calendar, highlight, 1);
    // }

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr, animType, 0, 0, false);
    }
    else
    {
        lv_scr_load(scr);
    }
}

void appCalendarUiDelete()
{
    if (scr != NULL)
    {
        lv_obj_clean(scr);
        scr = NULL;
    }
}