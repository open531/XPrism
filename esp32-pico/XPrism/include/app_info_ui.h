#ifndef APP_INFO_UI_H
#define APP_INFO_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct Info
    {
        int ip[4];
        double gpsLat;
        double gpsLon;
        double gpsAlt;
        double gpsSpd;
        int gpsSat;
    };

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appInfoUiInit();
    void appInfoUiDisplayInit(lv_scr_load_anim_t animType);
    void appInfoUiDisplay(struct Info infInfo, lv_scr_load_anim_t animType);
    void appInfoUiDelete();

    extern const lv_img_dsc_t icon_about;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif