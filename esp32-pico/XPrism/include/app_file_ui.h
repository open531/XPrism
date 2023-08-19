#ifndef APP_FILE_UI_H
#define APP_FILE_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appFileUiInit();
    void appFileUiDisplayExplorerInit(lv_scr_load_anim_t animType);
    void appFileUiDisplayExplorer(const char *path, const char *file1,
                                  const char *file2, const char *file3,
                                  lv_scr_load_anim_t animType);
    void appFileUiDisplayImageInit(lv_scr_load_anim_t animType);
    void appFileUiDisplayImage(const char *file_name, lv_scr_load_anim_t animType);
    void appFileUiDisplayVideoInit(lv_scr_load_anim_t animType);
    void appFileUiDisplayVideo(const char *file_name, lv_scr_load_anim_t animType);
    void appFileUiDisplayTextInit(lv_scr_load_anim_t animType);
    void appFileUiDisplayText(const char *file_name, lv_scr_load_anim_t animType);
    void appFileUiDelete();

    extern const lv_img_dsc_t icon_file;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif