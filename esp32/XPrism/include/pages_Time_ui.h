#ifndef PAGES_TIME_UI_H
#define PAGES_TIME_UI_H

#include "info.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void Time_gui_init(void);
    void display_time(Time TimInfo);
    
    
    void Time_gui_release(void);
    void Time_gui_del(void);


#ifdef __cplusplus
} /* extern "C" */
#endif



#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
    extern const lv_img_dsc_t app_time;

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif