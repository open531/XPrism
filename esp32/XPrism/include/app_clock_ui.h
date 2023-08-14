#ifndef APP_CLOCK_UI_H
#define APP_CLOCK_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct Clock
    {
        int hour;
        int minute;
        int second;
    };

#include "lvgl.h"

#define WAIT_ANIM                   \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成
    void appClockUiInit();
    void appClockUiDisplayBasicInit(lv_scr_load_anim_t animType);
    void appClockUiDisplayBasic(struct Clock cloInfo,lv_scr_load_anim_t animType);
    void appClockUiDelete();

    extern const lv_img_dsc_t icon_clock;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif