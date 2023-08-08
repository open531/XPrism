#ifndef PAGES_CLOCK_UI_H
#define PAGES_CLOCK_UI_H

#include "info.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void Clock_gui_init(void);
    void display_clock(Clock TimInfo);
    
    
    void Clock_gui_release(void);
    void Clock_gui_del(void);


#ifdef __cplusplus
} /* extern "C" */
#endif



#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
    extern const lv_img_dsc_t app_Clock;

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif