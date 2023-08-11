#ifndef APP_TIME_UI_H
#define APP_TIME_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "info.h"
#include "lvgl.h"

#define WAIT_ANIM                   \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成
    void appTimeUiInit();
    void appTimeUiDisplayBasicInit();
    void appTimeUiDisplayBasic(struct Time timInfo);
    void appTimeUiDelete();

    extern const lv_img_dsc_t icon_time;
    
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

