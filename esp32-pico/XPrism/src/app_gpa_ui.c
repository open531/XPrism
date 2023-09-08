#include "app_gpa_ui.h"
#include "icons_gpa.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_source_han_sans_24);

#define SCALE_SIZE 4

static lv_obj_t *gameGpaGui = NULL;
static lv_obj_t *gpaLabel = NULL;
static lv_obj_t *img[SCALE_SIZE * SCALE_SIZE];

static lv_style_t default_style;
static lv_style_t textStyle;

static double currentGpa;
static double highestGpa;

void appGpaUiInit()
{
    currentGpa = 0.0;
    highestGpa = 0.0;

    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, lv_color_hex(0xffffff));
    lv_style_set_text_font(&textStyle, &lv_font_source_han_sans_24);

    lv_obj_t *actObj = lv_scr_act();
    if (actObj == gameGpaGui)
    {
        return;
    }
    lv_obj_clean(actObj);

    gameGpaGui = lv_obj_create(NULL);
    lv_obj_add_style(gameGpaGui, &default_style, LV_STATE_DEFAULT);

    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        img[i] = lv_img_create(gameGpaGui);
        lv_img_set_src(img[i], &GPA_0);
        lv_obj_align(img[i], LV_ALIGN_TOP_LEFT,
                     34 + i % SCALE_SIZE * 44,
                     64 + i / SCALE_SIZE * 44);
    }

    gpaLabel = lv_label_create(gameGpaGui);
    lv_obj_add_style(gpaLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(gpaLabel, "当前绩点：0.00\n最高绩点：0.00");
    lv_obj_align(gpaLabel, LV_ALIGN_TOP_MID, 0, 0);

    lv_scr_load(gameGpaGui);
}

void appGpaUiDisplay()
{
}

void appGpaUiDelete()
{
    if (gameGpaGui != NULL)
    {
        lv_obj_clean(gameGpaGui);
        gameGpaGui = NULL;
    }
}

// 用于宽高同时增加的lv_anim_exec_xcb_t动画参数
static void anim_size_cb(void *var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

// 用于斜向移动的lv_anim_exec_xcb_t动画参数
static void anim_pos_cb(void *var, int32_t v)
{
    lv_obj_set_pos(var, v, v);
}

/*
 * 出生动画
 * i：出生的位置
 */
void born(int i)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_size_cb);
    lv_anim_set_var(&a, img[i]);
    lv_anim_set_time(&a, 300);

    /* 在动画中设置路径 */
    lv_anim_set_path_cb(&a, lv_anim_path_linear);

    lv_anim_set_values(&a, 0, 40);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, lv_obj_get_x(img[i]) + 20, lv_obj_get_x(img[i]));
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(img[i]) + 20, lv_obj_get_y(img[i]));
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_offset_x);
    lv_anim_set_values(&a, -20, 0);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_offset_y);
    lv_anim_set_values(&a, -20, 0);
    lv_anim_start(&a);
}

/*
 * 合并动画
 * i：合并的位置
 */
void zoom(int i)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_size_cb);
    lv_anim_set_var(&a, img[i]);
    lv_anim_set_delay(&a, 400);
    lv_anim_set_time(&a, 100);
    // 播完后回放
    lv_anim_set_playback_delay(&a, 0);
    lv_anim_set_playback_time(&a, 100);

    // 线性动画
    lv_anim_set_path_cb(&a, lv_anim_path_linear);

    lv_anim_set_values(&a, 40, 46);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, lv_obj_get_x(img[i]), lv_obj_get_x(img[i]) - 3);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(img[i]), lv_obj_get_y(img[i]) - 3);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_offset_x);
    lv_anim_set_values(&a, 0, 3);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_offset_y);
    lv_anim_set_values(&a, 0, 3);
    lv_anim_start(&a);
}

/*
 * 移动动画
 * i：移动的目标对象
 * direction：移动的方向，lv_obj_set_x或lv_obj_set_y
 * dist：移动的距离，如1、-1
 */
void move(int i, lv_anim_exec_xcb_t direction, int dist)
{
    lv_anim_t a;
    lv_anim_init(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)direction);
    lv_anim_set_var(&a, img[i]);
    lv_anim_set_time(&a, 500);
    if (direction == (lv_anim_exec_xcb_t)lv_obj_set_x)
    {
        lv_anim_set_values(&a, lv_obj_get_x(img[i]), lv_obj_get_x(img[i]) + dist * 58);
    }
    else
    {
        lv_anim_set_values(&a, lv_obj_get_y(img[i]), lv_obj_get_y(img[i]) + dist * 58);
    }

    // 在动画中设置路径
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);

    lv_anim_start(&a);
}

// 获取图片内容对象
const lv_img_dsc_t *getImgDsc(int index)
{
    switch (index)
    {
    case 0:
        return &GPA_0;
    case 2:
        return &GPA_D;
    case 4:
        return &GPA_DP;
    case 8:
        return &GPA_CM;
    case 16:
        return &GPA_C;
    case 32:
        return &GPA_CP;
    case 64:
        return &GPA_BM;
    case 128:
        return &GPA_B;
    case 256:
        return &GPA_BP;
    case 512:
        return &GPA_AM;
    case 1024:
        return &GPA_A;
    case 2048:
        return &GPA_AP;
    case -1:
        return &GPA_EX;
    case -2:
        return &GPA_F;
    case -3:
        return &GPA_W;
    default:
        return &GPA_0;
    }
}

// 刷新棋盘
void showBoard(int *map)
{
    double totalGpa = 0.0;
    int totalNum = 0;

    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        lv_img_set_src(img[i], getImgDsc(map[i]));
        lv_obj_align(img[i], LV_ALIGN_TOP_LEFT,
                     34 + i % SCALE_SIZE * 44,
                     64 + i / SCALE_SIZE * 44);
    }

    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        switch (map[i])
        {
        case 2: // D
            totalGpa += 1.3;
            totalNum++;
            break;
        case 4: // D+
            totalGpa += 1.6;
            totalNum++;
            break;
        case 8: // C-
            totalGpa += 2.0;
            totalNum++;
            break;
        case 16: // C
            totalGpa += 2.3;
            totalNum++;
            break;
        case 32: // C+
            totalGpa += 2.6;
            totalNum++;
            break;
        case 64: // B-
            totalGpa += 3.0;
            totalNum++;
            break;
        case 128: // B
            totalGpa += 3.3;
            totalNum++;
            break;
        case 256: // B+
            totalGpa += 3.6;
            totalNum++;
            break;
        case 512:  // A-
        case 1024: // A
        case 2048: // A+
            totalGpa += 4.0;
            totalNum++;
            break;
        case -2: // F
            totalNum++;
            break;
        default:
            break;
        }
    }

    if (totalNum != 0)
    {
        currentGpa = totalGpa / totalNum;
        highestGpa = currentGpa > highestGpa ? currentGpa : highestGpa;
    }

    lv_label_set_text_fmt(gpaLabel, "当前绩点：%.2f\n最高绩点：%.2f",
                          currentGpa, highestGpa);
}

/*
 * showAnim————用动画来更新棋盘
 * animMap：动画的轨迹数组
 * direction：移动的方向，1.上 2.下 3.左 4.右
 */
void showAnim(int *animMap, int direction)
{
    lv_anim_exec_xcb_t Normal;
    switch (direction)
    {
    case 1:
    case 2:
        Normal = (lv_anim_exec_xcb_t)lv_obj_set_y;
        break;
    case 3:
    case 4:
        Normal = (lv_anim_exec_xcb_t)lv_obj_set_x;
        break;
    }

    // 移动和合并
    for (int i = 0; i < 16; i++)
    {
        if (animMap[i] > 4)
        {
            zoom(i);
            move(i, Normal, animMap[i] - 8);
        }
        else if (animMap[i] != 0)
        {
            move(i, Normal, animMap[i]);
        }
    }
}

/*
 * newborn：新棋子的位置
 * map：新棋盘的地址
 */
void showNewBorn(int newborn, int *map)
{
    // 展示新棋盘
    showBoard(map);

    // 出现
    // born(newborn);
}