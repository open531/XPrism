#ifndef APP_TIME_UI_H
#define APP_TIME_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct Time
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int weekday;
        unsigned long timestamp;
    };

#include "lvgl.h"

#define WAIT_ANIM                   \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成
    void appTimeUiInit();
    void appTimeUiDisplayInit(lv_scr_load_anim_t animType);
    void appTimeUiDisplay(struct Time timInfo, lv_scr_load_anim_t animType);
    void appTimeUiDelete();

    extern const lv_img_dsc_t icon_time;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
