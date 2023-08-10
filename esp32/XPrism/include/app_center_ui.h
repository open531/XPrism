#ifndef APP_CENTER_UI_H
#define APP_CENTER_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define WAIT_ANIM                   \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appCenterUiInit();
    void appCenterUiDisplayInit(const void *appIcon,
                                const char *appName);
    void appCenterUiDisplay(const void *appIcon,
                            const char *appName,
                            lv_scr_load_anim_t animType,
                            bool force);
    void appCenterUiDelete();
    extern lv_img_dsc_t icon_loading;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif