#include "pages_clock_ui.h"
#include "system.h"
#include "pages_main.h"
#include "ArduinoJson.h"
#include "action.h"
#include <esp32-hal-timer.h>
#include <map>

#include "lvgl.h"
#define Time_APP_NAME "时钟"
#define TIME_CONFIG_PATH "/clock.json"

struct ClockRunData
{
    unsigned long Lastupdate;
    unsigned long updateInterval;
    unsigned int currentPage;
};

unsigned long MyTestTimer;

int RTC_Hour;
int RTC_Minute;
int RTC_Second;

static ClockRunData *run_data=NULL;

LV_FONT_DECLARE(lv_font_ubuntu_b_108);
//LV_FONT_DECLARE(lv_font_u);
LV_FONT_DECLARE(ch_font20);
static lv_style_t default_style;
static lv_style_t chFont_style;
static lv_style_t numberSmall_style;
static lv_style_t numberBig_style;
static lv_style_t btn_style;
static lv_style_t bar_style;

static lv_obj_t *scr_1 = NULL;
static lv_obj_t *scr_2 = NULL;

static lv_obj_t *chart, *titleLabel;

static lv_obj_t *clockLabel_1 = NULL, *clockLabel_2 = NULL;

static lv_chart_series_t *ser1, *ser2;

void Clock_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, lv_color_hex(0x000000));

    lv_style_init(&chFont_style);
    lv_style_set_text_opa(&chFont_style, LV_OPA_COVER);
    lv_style_set_text_color(&chFont_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&chFont_style, &ch_font20);

    lv_style_init(&numberSmall_style);
    lv_style_set_text_opa(&numberSmall_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberSmall_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&numberSmall_style, &lv_font_ibmplex_64);

    lv_style_init(&numberBig_style);
    lv_style_set_text_opa(&numberBig_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberBig_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&numberBig_style, &lv_font_ibmplex_115);

    lv_style_init(&btn_style);
    lv_style_set_border_width(&btn_style, 0);
    lv_style_init(&bar_style);
    lv_style_set_bg_color(&bar_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&bar_style, 2);
    lv_style_set_border_color(&bar_style, lv_color_hex(0xFFFFFF));
    lv_style_set_pad_top(&bar_style, 1); // 指示器到背景四周的距离
    lv_style_set_pad_bottom(&bar_style, 1);
    lv_style_set_pad_left(&bar_style, 1);
    lv_style_set_pad_right(&bar_style, 1);
}

void display_Clock_init()
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == scr_1)
        return;

    Clock_gui_release();
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

    lv_obj_align(clockLabel_1, LV_ALIGN_LEFT_MID, 0, 10);
    lv_obj_align(clockLabel_2, LV_ALIGN_LEFT_MID, 165, 9);

    // if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    // {
    //     lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    // }
    // else
    // {
    // lv_scr_load(scr_1);
    // }
}

void display_time(struct Clock clockInfo)
{
    lv_label_set_text_fmt(clockLabel_1, "%02d#ffa500 %02d#", clockInfo.Hour, clockInfo.Minute);
    lv_label_set_text_fmt(clockLabel_2, "%02d", clockInfo.Second);
    
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

void second_clock()
{
    Clock second_clock;

}

static void ClockProcess(XPages *sys,const Action *action)
{
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    if (action->type == BACK)
    {
        sys->app_exit();
        return;
    }
    else if (action->type == FORWARD)
    {
        run_data->forceUpdate = 1;
        delay(500);
    }
    else if (action->type == LEFT)
    {
        anim_type = LV_SCR_LOAD_ANIM_MOVE_LEFT;
        run_data->currentPage = (run_data->currentPage + WEATHER_PAGE_SIZE - 1) % WEATHER_PAGE_SIZE;
    }
    else if (action->type == RIGHT)
    {
        anim_type = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        run_data->currentPage = (run_data->currentPage + 1) % WEATHER_PAGE_SIZE;
    }
    else
    {
        return;
    }

    if (run_data->currentPage == 0)
    {
        display_weather(run_data->wea, anim_type);
        if (run_data->wea.id == 0 || run_data->forceUpdate || (millis() - run_data->lastUpdate > run_data->updateInterval))
        {
            get_weather();
        }
    }
    else
    {
        display_detail(run_data->wea, anim_type);
    }
}