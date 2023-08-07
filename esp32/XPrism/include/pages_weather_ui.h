#ifndef PAGES_WEATHER_UI_H
#define PAGES_WEATHER_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
#include "info.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void weather_gui_init(void);
    void display_weather_init(lv_scr_load_anim_t anim_type);
    void display_weather(Weather weaInfo, lv_scr_load_anim_t anim_type);
    void display_detail_init(lv_scr_load_anim_t anim_type);
    void display_detail(Weather weaInfo, lv_scr_load_anim_t anim_type);
    void weather_gui_release(void);
    void weather_gui_del(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
    extern const lv_img_dsc_t app_weather;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif