#include "app_file_ui.h"
#include "lvgl.h"
#include "lv_xprism_color.h"


LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;
static lv_style_t style_line;

static lv_obj_t *explorerScr;

static lv_obj_t *pathLabel;
static lv_obj_t *file1Label;
static lv_obj_t *file2Label;
static lv_obj_t *file3Label;
static lv_obj_t *file4Label;

static int panelPosition;
static int nextPanelPosition;
static lv_obj_t * selectPanel;

#if LV_USE_TABLE
#endif

void appFileUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&textStyle, &lv_font_wqy_microhei_24);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numBigStyle, &lv_font_ubuntu_b_108);    

}

void appFileUiDisplayExplorerInit(const char* file1Name,const char* file2Name,const char* file3Name,lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == explorerScr)
    {
        return;
    }

    appFileUiDelete();
    lv_obj_clean(actObj);

    explorerScr = lv_obj_create(NULL);
    lv_obj_add_style(explorerScr, &defaultStyle, LV_STATE_DEFAULT);

    pathLabel = lv_label_create(explorerScr);
    lv_obj_add_style(pathLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(pathLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(pathLabel, "/");

    selectPanel = lv_obj_create(explorerScr);
    lv_obj_add_style(selectPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_width(selectPanel, 200);
    lv_obj_set_height(selectPanel, 60);

    // TODO: Add file list
    file1Label=lv_label_create(explorerScr);
    lv_obj_add_style(file1Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file1Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file1Label, 200);
    lv_label_set_text(file1Label,file1Name);

    file2Label=lv_label_create(explorerScr);
    lv_obj_add_style(file2Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file2Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file2Label, 200);
    lv_label_set_text(file2Label,file2Name);

    file3Label=lv_label_create(explorerScr);
    lv_obj_add_style(file3Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file3Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file3Label, 200);
    lv_label_set_text(file3Label,file3Name);

    lv_obj_align(selectPanel, LV_ALIGN_TOP_MID, 0,60);
    panelPosition=60;
    lv_obj_align(file1Label, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_align(file2Label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(file3Label, LV_ALIGN_BOTTOM_MID, 0, -60);

    lv_scr_load_anim(explorerScr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
}

void appFileUiDisplayExplorer(const char *path,const char* _file1Name,
                                  const char* _file2Name, const char* _file3Name,const char* _file4Name,
                                  lv_scr_load_anim_t animType,bool force)
{
    if (force == true)
    {
        appNotiUiDisplayInit(_file1Name,_file2Name,_file3Name);
        return;
    }
    
    if(panelPosition==-60)
    {
    int file1StartY, file1EndY, file2StartY,file2EndY,file3StartY,file3EndY,file4StartY,file4EndY;

    if (animType == LV_SCR_LOAD_ANIM_MOVE_BOTTOM)
    {
        file1StartY = 60 + 90;
        file1EndY = 60;
        file2StartY = 60;
        file2EndY = 0;
        file3StartY = 0;
        file3EndY = -60;
        file4StartY = -60;
        file4EndY = -60 - 90;
        
    }
    else
    {
        file1StartY = 60;
        file1EndY = 60 + 90;
        file2StartY = 0;
        file2EndY = 60;
        file3StartY = -60;
        file3EndY = 0;
        file4StartY = -60 - 90;
        file4EndY = -60;
    }

    file2Label=lv_label_create(explorerScr);
    lv_obj_add_style(file2Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file2Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file2Label, 200);
    lv_label_set_text(file2Label,_file2Name);

    file3Label=lv_label_create(explorerScr);
    lv_obj_add_style(file3Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file3Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file3Label, 200);
    lv_label_set_text(file3Label,_file3Name);
    
    file4Label=lv_label_create(explorerScr);
    lv_obj_add_style(file4Label, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_long_mode(file4Label,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(file4Label, 200);
    lv_label_set_text(file4Label,_file4Name);

    lv_obj_align(file2Label, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_align(file3Label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(file4Label, LV_ALIGN_BOTTOM_MID, 0, -60);

     static lv_anim_t file1Anim;
    lv_anim_init(&file1Anim);
    lv_anim_set_exec_cb(&file1Anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_var(&file1Anim, file1Label);
    lv_anim_set_values(&file1Anim, file1StartY, file1EndY);
    lv_anim_set_time(&file1Anim, 300);
    lv_anim_set_path_cb(&file1Anim, lv_anim_path_linear);

    static lv_anim_t file2Anim;
    lv_anim_init(&file2Anim);
    lv_anim_set_exec_cb(&file2Anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_var(&file2Anim, file2Label);
    lv_anim_set_values(&file2Anim, file2StartY, file2EndY);
    lv_anim_set_time(&file2Anim, 300);
    lv_anim_set_path_cb(&file2Anim, lv_anim_path_linear);

    static lv_anim_t file3Anim;
    lv_anim_init(&file3Anim);
    lv_anim_set_exec_cb(&file3Anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_var(&file3Anim, file3Label);
    lv_anim_set_values(&file3Anim, file3StartY, file3EndY);
    lv_anim_set_time(&file3Anim, 300);
    lv_anim_set_path_cb(&file3Anim, lv_anim_path_linear);

    static lv_anim_t file4Anim;
    lv_anim_init(&file4Anim);
    lv_anim_set_exec_cb(&file4Anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_var(&file4Anim, file4Label);
    lv_anim_set_values(&file4Anim, file4StartY, file4EndY);
    lv_anim_set_time(&file4Anim, 300);
    lv_anim_set_path_cb(&file4Anim, lv_anim_path_linear);

    lv_anim_start(&file1Anim);
    lv_anim_start(&file2Anim);
    lv_anim_start(&file3Anim);
    lv_anim_start(&file4Anim);
    
    ANIEND_WAIT
    lv_task_handler();

    lv_obj_del(file1Label);
    file1Label=file2Label;
    file2Label=file3Label;
    file3Label=file4Label;
    }
    else if(panelPosition==60)
    {
        nextPanelPosition=0;

        static lv_anim_t panelAnim;
        lv_anim_init(&panelAnim);
        lv_anim_set_exec_cb(&panelAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_var(&panelAnim, selectPanel);
        lv_anim_set_values(&panelAnim, panelPosition, nextPanelPosition);
        lv_anim_set_time(&panelAnim, 300);
        lv_anim_set_path_cb(&panelAnim, lv_anim_path_linear);

        lv_anim_start(&panelAnim);

        ANIEND_WAIT
        lv_task_handler();

        panelPosition=nextPanelPosition;
    }
    else if(panelPosition==0)
    {
        nextPanelPosition=-60;

        static lv_anim_t panelAnim;
        lv_anim_init(&panelAnim);
        lv_anim_set_exec_cb(&panelAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_var(&panelAnim, selectPanel);
        lv_anim_set_values(&panelAnim, panelPosition, nextPanelPosition);
        lv_anim_set_time(&panelAnim, 300);
        lv_anim_set_path_cb(&panelAnim, lv_anim_path_linear);

        lv_anim_start(&panelAnim);

        ANIEND_WAIT
        lv_task_handler();

        panelPosition=nextPanelPosition;
    }
}



void appNotiUiDelete()
{
    if (explorerScr != NULL)
    {
        lv_obj_clean(explorerScr);
        explorerScr = NULL;
    }
}