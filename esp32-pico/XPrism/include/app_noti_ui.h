#ifndef APP_NOTI_UI_H
#define APP_NOTI_UI_H

struct Notification
{
    char title[32];
    char text[128];
    unsigned long long time;
    char timeStr[32];
};

struct NotiStream
{
    struct Notification *notis;
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
    void appNotiUiDisplayInit(struct Notification notInfo);
    void appNotiUiDisplay(struct Notification notInfo, lv_scr_load_anim_t animType, bool force);
    void appNotiUiDelete();

    extern const lv_img_dsc_t icon_noti;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif