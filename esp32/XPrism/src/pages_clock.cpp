#include "pages_clock_ui.h"
#include "system.h"
#include "pages_main.h"
#include "ArduinoJson.h"
#include "actions.h"
#include <esp32-hal-timer.h>
#include <map>
#include <String>
#include "ESP32Time.h"

#include "lvgl.h"
#define Time_APP_NAME "时钟"
#define TIME_CONFIG_PATH "/clock.json"

struct ClockRunData
{
    unsigned long forceUpdate;
    unsigned long lastUpdate;
    unsigned long updateInterval;
    unsigned int currentPage;
};

ESP32Time esp32time;

unsigned long MyTestTimer;

static int RTC_Hour;
static int RTC_Minute;
static int RTC_Second;

static int RTR_Hour;
static int RTR_Minute;
static int RTR_Second;

static ClockRunData *run_data=NULL;

LV_FONT_DECLARE(lv_font_ubuntu_b_108);
LV_FONT_DECLARE(lv_font_ubuntu_b_64);
LV_FONT_DECLARE(lv_font_wqy_microhei_20);
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

static ClockRunData *data;

void initialdata()
{
    data->lastUpdate=0;
    data->updateInterval=100;
}

void Clock_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, lv_color_hex(0x000000));

    lv_style_init(&chFont_style);
    lv_style_set_text_opa(&chFont_style, LV_OPA_COVER);
    lv_style_set_text_color(&chFont_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&chFont_style, &lv_font_wqy_microhei_20);

    lv_style_init(&numberSmall_style);
    lv_style_set_text_opa(&numberSmall_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberSmall_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&numberSmall_style, &lv_font_ubuntu_b_64);

    lv_style_init(&numberBig_style);
    lv_style_set_text_opa(&numberBig_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberBig_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&numberBig_style, &lv_font_ubuntu_b_108);

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

void display_clock(struct Clock clockInfo)
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

void Clock_gui_release(void)
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

// easy to use helper-function for non-blocking timing
boolean TimePeriodIsOver (unsigned long &periodStartTime, unsigned long TimePeriod) {
  unsigned long StopStart=0,StopFinish=0;
  unsigned long currentMillis  = millis();
  if(data->forceUpdate==0)
  {
    StopStart=millis();
    return false;
  }
  if(data->forceUpdate==1)
  {
    StopFinish=millis();
    currentMillis=currentMillis-(StopFinish-StopStart);
  if ( currentMillis - periodStartTime >= TimePeriod )
  {
    periodStartTime = currentMillis; // set new expireTime
    return true;                // more time than TimePeriod) has elapsed since last time if-condition was true
  }
  else return false;            // not expired
  }
}

unsigned long RTC_Timer;

void SoftRTC() {//计时
  if ( TimePeriodIsOver(RTC_Timer, 1000) ) { // once every 100 milliseconds 
    RTC_Second ++;                   // increase 1/10th-seconds counter
    
      if (RTC_Second == 60) {               // if seconds counter reaches 60
        RTC_Minute++;                       // increase minutes counter by 1
        RTC_Second = 0;                     // reset seconds counter to zero
      }
    

    if (RTC_Minute == 60) {                 // if minutes counter reaches 60
      RTC_Hour++;                           // increase hour counter by 1
      RTC_Minute = 0;                       // reset minutes counter to zero
    }

    if (RTC_Hour == 24) {                   // if hours counter reaches 24 
      RTC_Hour = 0;                         // reset hours counter to zero
    }
  }
}

void Second_Clear()
{
    RTC_Hour=0;
    RTC_Minute=0;
    RTC_Second=0;
}

void ReSecond_Clear()
{
    RTR_Hour=0;
    RTR_Minute=0;
    RTR_Second=0;
}

static void task_update_second(Clock *second_clock)
{
    initialdata();
    
    while(1)
    {
        SoftRTC();
        second_clock->Hour=RTC_Hour;
        second_clock->Minute=RTC_Minute;
        second_clock->Second=RTC_Second;
        display_clock(*second_clock);
    }
    
    
}

void second_clock()//秒表
{
    Clock *second_clock;
    task_update_second(second_clock);

}

//需要补充：
Clock initialRTR()//倒计时初始化
{

}

unsigned long RTR_Timer;

void SoftRTR() {//倒计时
  if ( TimePeriodIsOver(RTR_Timer, 1000) ) { // once every 100 milliseconds 
    RTR_Second --;                   // increase 1/10th-seconds counter
    
      if (RTR_Second <=0 && RTR_Minute>0 && RTR_Hour>0) {               // if seconds counter reaches 60
        RTR_Minute--;                       // increase minutes counter by 1
        RTR_Second = 60;                     // reset seconds counter to zero
      }
    

    if (RTR_Minute <=0 && RTR_Hour>0) {                 // if minutes counter reaches 60
      RTR_Hour--;                           // increase hour counter by 1
      RTR_Minute = 60;                       // reset minutes counter to zero
    }

    if (RTR_Hour <=0) {                   // if hours counter reaches 24 
      RTR_Hour = 0;                         // reset hours counter to zero
    }
  }
}



static void task_update_resecond(Clock *resecond_clock,Clock *set_clock)
{
    initialdata();
    while(1)
    {
        SoftRTR();
        resecond_clock->Hour=RTR_Hour;
        resecond_clock->Minute=RTR_Minute;
        resecond_clock->Second=RTR_Second;
        display_clock(*resecond_clock);
    }
}

void resecond_clock()//计时器
{
    Clock *resecond_clock,*set_clock;
    task_update_resecond(resecond_clock,set_clock);

    set_clock->initialClock();
}

static void ClockProcess(XPages *sys,const Action *action)
{
    data->forceUpdate=0;
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    if (action->type == BACK)
    {
        sys->app_exit();
        return;
    }
    else if (action->type == FORWARD)
    {
        if(data->currentPage==0)
        {
            if(data->forceUpdate==0)
            {
                data->forceUpdate=1;
            }
            else if(data->forceUpdate==1)
            {
                data->forceUpdate=0;
            }
        }
        else if(data->currentPage==1)
        {
            if(data->forceUpdate==0)
            {
                data->forceUpdate=1;
            }
            else if(data->forceUpdate==1)
            {
                data->forceUpdate=0;
            }
        }
        delay(500);
    }
    else if (action->type == LEFT)
    {
        if(data->currentPage==0)
        {
            Second_Clear();
            data->forceUpdate==0;
        }
        else if(data->currentPage==1)
        {
            ReSecond_Clear();
            data->forceUpdate==0;
        }
        data->currentPage = (data->currentPage + 1) % 2;
    }
    else if (action->type == RIGHT)
    {
        if(data->currentPage==0)
        {
            Second_Clear();
            data->forceUpdate==0;
        }
        else if(data->currentPage==1)
        {
            ReSecond_Clear();
            data->forceUpdate==0;
        }
        data->currentPage = (data->currentPage + 1) % 2;
    }
    else
    {
        return;
    }

    if (data->currentPage == 0)
    {
        second_clock();
        
    }
    else
    {
        resecond_clock();
    }
}