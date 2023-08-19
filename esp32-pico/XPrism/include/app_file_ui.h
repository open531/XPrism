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
    void appFileUiDisplayExplorerInit(const char* file1Name,const char* file2Name,const char* file3Name,lv_scr_load_anim_t animType);
    void appFileUiDisplayExplorer(const char *path,const char* _file1Name,
                                  const char* _file2Name, const char* _file3Name,const char* _file4Name,
                                  lv_scr_load_anim_t animType,bool force);
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