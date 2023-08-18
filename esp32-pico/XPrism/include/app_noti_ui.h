#ifndef APP_NOTI_UI_H
#define APP_NOTI_UI_H

struct Notification
{
    char *title;
    char *text;
    unsigned long time;
};

struct NotiStream
{
    Notification *notis;
    unsigned int size;
};

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appNotiUiInit();
    void appNotiUiDisplayInit(lv_scr_load_anim_t animType);
    void appNotiUiDisplay(struct Notification notInfo, lv_scr_load_anim_t animType);
    void appNotiUiDelete();

    extern const lv_img_dsc_t icon_map;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif