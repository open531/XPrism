#ifndef APP_NAVI_UI_H
#define APP_NAVI_UI_H

struct Navi
{
    char *pointEncoded;
    int pointLength;
    double *pointLat;
    double *pointLon;
    int instructionLength;
    char **instructionText;
    int *instructionSign;
    int *instructionIntervalBegin;
    int *instructionIntervalEnd;
};

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appNaviUiInit();
    void appNaviUiDisplayInit(lv_scr_load_anim_t animType);
    void appNaviUiDisplay(struct Navi navInfo, lv_scr_load_anim_t animType);
    void appNaviUiDelete();

    extern const lv_img_dsc_t icon_map;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif