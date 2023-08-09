#include "app_center_ui.h"
#include "display.h"
#include "lv_xprism_color.h"

static lv_obj_t *appScr = NULL;
static lv_obj_t *prevIcon = NULL;
static lv_obj_t *prevName = NULL;
static lv_obj_t *currIcon = NULL;
static lv_obj_t *currName = NULL;
const void *prevIconSrc = NULL;

static lv_style_t defaultStyle;
static lv_style_t appNameStyle;

LV_FONT_DECLARE(lv_font_wqy_microhei_20);

void appCenterUiInit()
{
    if (appScr != NULL)
    {
        lv_obj_clean(appScr);
        appScr = NULL;
    }

    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);
    lv_style_set_radius(&defaultStyle, 0);
    lv_style_set_border_width(&defaultStyle, 0);

    lv_style_init(&appNameStyle);
    lv_style_set_opa(&appNameStyle, LV_OPA_COVER);
    lv_style_set_text_color(&appNameStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&appNameStyle, &lv_font_wqy_microhei_20);

    // app icon screen
    appScr = lv_obj_create(NULL);
    lv_obj_add_style(appScr, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(appScr, LV_OPA_0,
                            LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
}

void appCenterUiDisplayInit(const void *appIcon,
                            const char *appName)
{
    lv_obj_t *actScr = lv_scr_act();
    if (actScr == appScr)
    {
        lv_scr_load_anim(appScr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
        return;
    }

    lv_obj_clean(actScr);
    prevIcon = lv_img_create(appScr);
    prevIconSrc = appIcon;
    lv_img_set_src(prevIcon, appIcon);
    lv_obj_align(prevIcon, LV_ALIGN_CENTER, 0, -20);

    prevName = lv_label_create(appScr);
    lv_obj_add_style(prevName, &appNameStyle, LV_STATE_DEFAULT);
    lv_label_set_text(prevName, appName);
    lv_obj_align_to(prevName, prevIcon, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_scr_load_anim(appScr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
}

void appCenterUiDisplay(const void *appIcon,
                        const char *appName,
                        lv_scr_load_anim_t animType,
                        bool force)
{
    if (force == true)
    {
        appCenterUiDisplayInit(appIcon, appName);
        return;
    }

    if (appIcon == prevIconSrc)
    {
        return;
    }

    prevIconSrc = appIcon;
    int currXStart, currXEnd, prevXStart, prevXEnd;

    if (animType == LV_SCR_LOAD_ANIM_MOVE_LEFT)
    {
        currXStart = -SCREEN_WIDTH - 64;
        currXEnd = 0;
        prevXStart = 0;
        prevXEnd = SCREEN_WIDTH + 64;
    }
    else
    {
        currXStart = SCREEN_WIDTH + 64;
        currXEnd = 0;
        prevXStart = 0;
        prevXEnd = -SCREEN_WIDTH - 64;
    }

    currIcon = lv_img_create(appScr);
    lv_img_set_src(currIcon, appIcon);
    lv_obj_align(currIcon, LV_ALIGN_CENTER, 0, -20);
    currName = lv_label_create(appScr);
    lv_obj_add_style(currName, &appNameStyle, LV_STATE_DEFAULT);
    lv_label_set_text(currName, appName);
    lv_obj_del(prevName);
    prevName = currName;
    lv_obj_align_to(currName, currIcon, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    static lv_anim_t currIconAnim;
    lv_anim_init(&currIconAnim);
    lv_anim_set_exec_cb(&currIconAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&currIconAnim, currIcon);
    lv_anim_set_values(&currIconAnim, currXStart, currXEnd);
    lv_anim_set_time(&currIconAnim, 300);
    lv_anim_set_path_cb(&currIconAnim, lv_anim_path_linear);

    static lv_anim_t prevIconAnim;
    lv_anim_init(&prevIconAnim);
    lv_anim_set_exec_cb(&prevIconAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&prevIconAnim, prevIcon);
    lv_anim_set_values(&prevIconAnim, prevXStart, prevXEnd);
    lv_anim_set_time(&prevIconAnim, 300);
    lv_anim_set_path_cb(&prevIconAnim, lv_anim_path_linear);

    lv_anim_start(&currIconAnim);
    lv_anim_start(&prevIconAnim);
    WAIT_ANIM
    lv_task_handler();

    lv_obj_del(prevIcon);
    prevIcon = currIcon;
}

void appCenterUiDelete()
{
    if (appScr == NULL)
    {
        lv_obj_clean(appScr);
        appScr = NULL;
    }
}