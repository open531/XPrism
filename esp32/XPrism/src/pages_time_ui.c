#include "pages_time.h"
#include "pages_time_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108);
LV_FONT_DECLARE(lv_font_ubuntu_b_64);
LV_FONT_DECLARE(lv_font_wqy_microhei_20);

static lv_style_t default_style;
static lv_style_t chFont_style;
static lv_style_t numberSmall_style;
static lv_style_t numberBig_style;
static lv_style_t btn_style;

static lv_obj_t *scr_1 = NULL;
static lv_obj_t *scr_2 = NULL;

static lv_obj_t *chart, *titleLabel;

static lv_obj_t *clockLabel_1 = NULL, *clockLabel_2 = NULL;
static lv_obj_t *dateLabel = NULL;

static lv_chart_series_t *ser1, *ser2;

void Time_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, LV_COLOR_BLACK);

    lv_style_init(&chFont_style);
    lv_style_set_text_opa(&chFont_style, LV_OPA_COVER);
    lv_style_set_text_color(&chFont_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&chFont_style, &lv_font_wqy_microhei_20);

    lv_style_init(&numberSmall_style);
    lv_style_set_text_opa(&numberSmall_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberSmall_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberSmall_style, &lv_font_ubuntu_b_64);

    lv_style_init(&numberBig_style);
    lv_style_set_text_opa(&numberBig_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberBig_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberBig_style, &lv_font_ubuntu_b_108);

    lv_style_init(&btn_style);
    lv_style_set_border_width(&btn_style, 0);
}

void display_Time_init()
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == scr_1)
        return;

    Time_gui_release();
    lv_obj_clean(act_obj); // 清空此前页面

    scr_1 = lv_obj_create(NULL);
    lv_obj_add_style(scr_1, &default_style, LV_STATE_DEFAULT);

    clockLabel_1 = lv_label_create(scr_1);
    lv_obj_add_style(clockLabel_1, &numberBig_style, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockLabel_1, true);
    lv_label_set_text_fmt(clockLabel_1, "%02d#ffa500 %02d#", 10, 52);

    clockLabel_2 = lv_label_create(scr_1);
    lv_obj_add_style(clockLabel_2, &numberSmall_style, LV_STATE_DEFAULT);
    lv_label_set_recolor(clockLabel_2, true);
    lv_label_set_text_fmt(clockLabel_2, "%02d", 00);

    dateLabel = lv_label_create(scr_1);
    lv_obj_add_style(dateLabel, &chFont_style, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(dateLabel, "%2d月%2d日   周%s", 11, 23, weekDayCh[1]);

    lv_obj_align(clockLabel_1, LV_ALIGN_LEFT_MID, 0, 10);
    lv_obj_align(clockLabel_2, LV_ALIGN_LEFT_MID, 165, 9);
    lv_obj_align(dateLabel, LV_ALIGN_LEFT_MID, 10, 32);

    // if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    // {
    //     lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    // }
    // else
    // {
    // lv_scr_load(scr_1);
    // }
}

void display_time(struct Time timeInfo)
{
    lv_label_set_text_fmt(clockLabel_1, "%02d#ffa500 %02d#", timeInfo.Hour, timeInfo.Minute);
    lv_label_set_text_fmt(clockLabel_2, "%02d", timeInfo.Second);
    lv_label_set_text_fmt(dateLabel, "%2d月%2d日   周%s", timeInfo.Month, timeInfo.Day,
                          weekDayCh[timeInfo.Weekday]);

    // lv_obj_align(clockLabel_1, NULL, LV_ALIGN_LEFT_MID, 0, 10);
    // lv_obj_align(clockLabel_2, NULL, LV_ALIGN_LEFT_MID, 165, 9);
    // lv_obj_align(dateLabel, NULL, LV_ALIGN_LEFT_MID, 10, 32);

    // if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    // {
    //     lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    // }
    // else
    // {
    //     lv_scr_load(scr_1);
    // }
}

void Time_gui_release(void)
{
    if (scr_1 != NULL)
    {
        lv_obj_clean(scr_1);
        scr_1 = NULL;

        clockLabel_1 = NULL;
        clockLabel_2 = NULL;
        dateLabel = NULL;
    }

    if (scr_2 != NULL)
    {
        lv_obj_clean(scr_2);
        scr_2 = NULL;
        chart = NULL;
        titleLabel = NULL;
        ser1 = NULL;
        ser2 = NULL;
    }
}

void Time_gui_del(void)
{
    Time_gui_release();

    // 手动清除样式，防止内存泄漏
    // lv_style_reset(&default_style);
    // lv_style_reset(&chFont_style);
    // lv_style_reset(&numberSmall_style);
    // lv_style_reset(&numberBig_style);
    // lv_style_reset(&btn_style);
    // lv_style_reset(&bar_style);
}
