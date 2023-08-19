#ifndef APP_SPORT_UI_H
#define APP_SPORT_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct Sport
    {
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

    void appSportUiInit();
    void appSportUiDisplayInit(lv_scr_load_anim_t animType);
    void appSportUiDisplay(struct Sport spoInfo, lv_scr_load_anim_t animType);
    void appSportUiDelete();

    extern const lv_img_dsc_t icon_sport;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif