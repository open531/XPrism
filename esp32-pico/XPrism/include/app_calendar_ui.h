#ifndef APP_CALENDAR_UI_H
#define APP_CALENDAR_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct Date
    {
        int year;
        int month;
        int day;
    };

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成
    void appCalendarUiInit();
    void appCalendarUiDisplayInit(struct Date today, lv_scr_load_anim_t animType);
    void appCalendarUiDisplay(struct Date today,
                              struct Date date,
                              struct Date *highlight,
                              lv_scr_load_anim_t animType);
    void appCalendarUiDelete();

    extern const lv_img_dsc_t icon_calendar;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif