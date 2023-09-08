#ifndef APP_GPA_UI_H
#define APP_GPA_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appGpaUiInit();
    void appGpaUiDisplay();
    void appGpaUiDelete();

    const lv_img_dsc_t *getImgDsc(int index);
    void showBoard(int *map);
    void showAnim(int *animMap, int direction);
    void showNewBorn(int newborn, int *map);
    void born(int i);
    void zoom(int i);
    void move(int i, lv_anim_exec_xcb_t direction, int dist);

    extern const lv_img_dsc_t icon_game;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif