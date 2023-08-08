#include "pages_file_ui.h"
#include "lvgl.h"
#include "string.h"

lv_obj_t *file_explorer = NULL;

void file_gui_init(void)
{
    file_explorer = lv_file_explorer_create(lv_scr_act());
    lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_NAME);
    lv_file_explorer_open_dir(file_explorer, "/");
    lv_obj_add_event(file_explorer, file_explorer_event_handler, LV_EVENT_ALL, NULL);
}

void file_gui_del()
{
    if (file_explorer != NULL)
    {
        lv_obj_clean(file_explorer);
        file_explorer = NULL;
    }
}

//

static void lv_file_explorer_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_file_explorer_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void browser_file_event_handler(lv_event_t *e);

static void init_style(lv_obj_t *obj);
static void show_dir(lv_obj_t *obj, char *path);
static void strip_ext(char *dir);
static void sort_table_items(lv_obj_t *tb, int16_t lo, int16_t hi);
static void exch_table_item(lv_obj_t *tb, int16_t i, int16_t j);
static bool is_begin_with(const char *str1, const char *str2);
static bool is_end_with(const char *str1, const char *str2);

const lv_obj_class_t lv_file_explorer_class = {
    .constructor_cb = lv_file_explorer_constructor,
    .destructor_cb = lv_file_explorer_destructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_file_explorer_t),
    .base_class = &lv_obj_class};

lv_obj_t *lv_file_explorer_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_file_explorer_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_file_explorer_set_sort(lv_obj_t *obj, lv_file_explorer_sort_t sort)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);

    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;

    uint16_t sum = lv_table_get_row_cnt(explorer->file_list);

    switch (sort)
    {
    case LV_EXPLORER_SORT_KIND:
        sort_table_items(explorer->file_list, 0, sum - 1);
        break;
    case LV_EXPLORER_SORT_NAME:
        /* TODO */
        break;
    default:
        break;
    }
}

char *lv_file_explorer_get_sel_fn(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    return explorer->sel_fp;
}

char *lv_file_explorer_get_cur_path(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    return explorer->cur_path;
}

lv_obj_t *lv_file_explorer_get_file_list(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    return explorer->file_list;
}

lv_obj_t *lv_file_explorer_get_head_area(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    return explorer->head_area;
}

lv_obj_t *lv_file_explorer_get_path_obj(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_file_explorer_class);
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    return explorer->path_label;
}

void lv_file_explorer_open_dir(lv_obj_t *obj, char *dir)
{
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    show_dir(obj, dir);
}

static void lv_file_explorer_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;
    lv_memset_00(explorer->cur_path, sizeof(explorer->cur_path));
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    explorer->cont = lv_obj_create(obj);
    lv_obj_set_width(explorer->cont, LV_PCT(100));
    lv_obj_set_flex_grow(explorer->cont, 1);

    // 文件浏览区域
    explorer->browser_area = lv_obj_create(explorer->cont);
    lv_obj_set_size(explorer->browser_area, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(explorer->browser_area, LV_FLEX_FLOW_COLUMN);

    // head area
    explorer->head_area = lv_obj_create(explorer->browser_area);
    lv_obj_set_size(explorer->head_area, LV_PCT(100), LV_PCT(12));
    lv_obj_clear_flag(explorer->head_area, LV_OBJ_FLAG_SCROLLABLE);

    // 展示当前路径
    explorer->path_label = lv_label_create(explorer->head_area);
    lv_label_set_text(explorer->path_label, LV_SYMBOL_EYE_OPEN "https://lvgl.100ask.net");
    lv_obj_center(explorer->path_label);

    // 目录内容展示列表
    explorer->file_list = lv_table_create(explorer->browser_area);
    lv_obj_set_size(explorer->file_list, LV_PCT(100), LV_PCT(84));
    lv_table_set_col_width(explorer->file_list, 0, LV_PCT(100));
    lv_table_set_col_cnt(explorer->file_list, 1);
    lv_obj_add_event_cb(explorer->file_list, browser_file_event_handler, LV_EVENT_ALL, obj);

    // only scroll up and down
    lv_obj_set_scroll_dir(explorer->file_list, LV_DIR_TOP | LV_DIR_BOTTOM);
    init_style(obj);
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_file_explorer_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
}

static void init_style(lv_obj_t *obj)
{
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;

    // cont区域风格样式
    static lv_style_t cont_style;
    lv_style_init(&cont_style);
    lv_style_set_radius(&cont_style, 0);
    lv_style_set_bg_opa(&cont_style, LV_OPA_0);
    lv_style_set_border_width(&cont_style, 0);
    lv_style_set_outline_width(&cont_style, 0);
    lv_style_set_pad_column(&cont_style, 0);
    lv_style_set_pad_row(&cont_style, 0);
    lv_style_set_flex_flow(&cont_style, LV_FLEX_FLOW_ROW);
    lv_style_set_pad_all(&cont_style, 0);
    lv_style_set_layout(&cont_style, LV_LAYOUT_FLEX);

    // 文件查看、浏览区域风格样式
    static lv_style_t browser_area_style;
    lv_style_init(&browser_area_style);
    lv_style_set_pad_all(&browser_area_style, 0);
    lv_style_set_radius(&browser_area_style, 0);
    lv_style_set_border_width(&browser_area_style, 0);
    lv_style_set_outline_width(&browser_area_style, 0);
    lv_style_set_bg_color(&browser_area_style, lv_color_hex(0xffffff));

    // 文件浏览区域的样式风格
    static lv_style_t file_list_style;
    lv_style_init(&file_list_style);
    lv_style_set_bg_color(&file_list_style, lv_color_hex(0xffffff));
    lv_style_set_pad_all(&file_list_style, 0);
    lv_style_set_radius(&file_list_style, 0);
    lv_style_set_border_width(&file_list_style, 0);
    lv_style_set_outline_width(&file_list_style, 0);

    // 设置 file_explorer 的样式
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf2f1f6), 0);

    // header area 的样式
    lv_obj_set_style_radius(explorer->head_area, 0, 0);
    lv_obj_set_style_border_width(explorer->head_area, 0, 0);
    lv_obj_set_style_pad_top(explorer->head_area, 0, 0);

    lv_obj_add_style(explorer->cont, &cont_style, 0);
    lv_obj_add_style(explorer->browser_area, &browser_area_style, 0);
    lv_obj_add_style(explorer->file_list, &file_list_style, 0);
}

static void browser_file_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);

    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // struct stat stat_buf;
        char *file_name[LV_FILE_EXPLORER_PATH_MAX_LEN];
        char *str_fn = NULL;
        uint16_t row;
        uint16_t col;

        memset(file_name, 0, sizeof(file_name));
        lv_table_get_selected_cell(explorer->file_list, &row, &col);
        str_fn = lv_table_get_cell_value(explorer->file_list, row, col);

        str_fn = str_fn + 5;
        if ((strcmp(str_fn, ".") == 0))
            return;

        if ((strcmp(str_fn, "..") == 0) && (strlen(explorer->cur_path) > 3))
        {
            strip_ext(explorer->cur_path);
            strip_ext(explorer->cur_path); // 去掉最后的 '/' 路径
            lv_snprintf(file_name, sizeof(file_name), "%s", explorer->cur_path);
        }
        else
        {
            if (strcmp(str_fn, "..") != 0)
            {
                lv_snprintf(file_name, sizeof(file_name), "%s%s", explorer->cur_path, str_fn);
            }
        }

        lv_fs_dir_t dir;
        if (lv_fs_dir_open(&dir, file_name) == LV_FS_RES_OK)
        {
            lv_fs_dir_close(&dir);
            show_dir(obj, file_name);
        }
        else
        {
            if (strcmp(str_fn, "..") != 0)
            {
                explorer->sel_fp = str_fn;
                lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            }
        }
    }
    else if (code == LV_EVENT_SIZE_CHANGED)
    {
        lv_table_set_col_width(explorer->file_list, 0, lv_obj_get_width(explorer->file_list));
    }
}

static void show_dir(lv_obj_t *obj, char *path)
{
    lv_file_explorer_t *explorer = (lv_file_explorer_t *)obj;

    char fn[LV_FILE_EXPLORER_PATH_MAX_LEN];
    uint16_t index = 0;
    lv_fs_dir_t dir;
    lv_fs_res_t res;

    res = lv_fs_dir_open(&dir, path);
    if (res != LV_FS_RES_OK)
    {
        LV_LOG_USER("Open dir error %d!", res);
        return;
    }

    lv_table_set_cell_value_fmt(explorer->file_list, index++, 0, LV_SYMBOL_DIRECTORY "  %s", ".");
    lv_table_set_cell_value_fmt(explorer->file_list, index++, 0, LV_SYMBOL_DIRECTORY "  %s", "..");
    lv_table_set_cell_value(explorer->file_list, 0, 1, "0");
    lv_table_set_cell_value(explorer->file_list, 1, 1, "0");

    while (1)
    {
        res = lv_fs_dir_read(&dir, fn);
        if (res != LV_FS_RES_OK)
        {
            LV_LOG_USER("Driver, file or directory is not exists %d!", res);
            break;
        }

        /*fn is empty, if not more files to read*/
        if (strlen(fn) == 0)
        {
            LV_LOG_USER("Not more files to read!");
            break;
        }

        // 识别并展示文件
        if ((is_end_with(fn, ".png") == true) || (is_end_with(fn, ".PNG") == true) ||
            (is_end_with(fn, ".jpg") == true) || (is_end_with(fn, ".JPG") == true) ||
            (is_end_with(fn, ".bmp") == true) || (is_end_with(fn, ".BMP") == true) ||
            (is_end_with(fn, ".gif") == true) || (is_end_with(fn, ".GIF") == true))
        {
            lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_IMAGE "  %s", fn);
            lv_table_set_cell_value(explorer->file_list, index, 1, "1");
        }
        else if ((is_end_with(fn, ".mp3") == true) || (is_end_with(fn, ".MP3") == true))
        {
            lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_AUDIO "  %s", fn);
            lv_table_set_cell_value(explorer->file_list, index, 1, "2");
        }
        else if ((is_end_with(fn, ".mp4") == true) || (is_end_with(fn, ".MP4") == true))
        {
            lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_VIDEO "  %s", fn);
            lv_table_set_cell_value(explorer->file_list, index, 1, "3");
        }
        else if ((is_end_with(fn, ".") == true) || (is_end_with(fn, "..") == true))
        {
            /*is dir*/
            // lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_DIRECTORY "  %s", fn);
            continue;
        }
        else if (fn[0] == '/')
        { /*is dir*/
            lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_DIRECTORY "  %s", fn + 1);
            lv_table_set_cell_value(explorer->file_list, index, 1, "0");
        }
        else
        {
            lv_table_set_cell_value_fmt(explorer->file_list, index, 0, LV_SYMBOL_FILE "  %s", fn);
            lv_table_set_cell_value(explorer->file_list, index, 1, "4");
        }

        index++;
    }

    lv_fs_dir_close(&dir);
    lv_table_set_row_cnt(explorer->file_list, index);
    lv_100ask_file_explorer_set_sort(obj, LV_EXPLORER_SORT_KIND);
    // 让table移动到最顶部
    lv_obj_scroll_to_y(explorer->file_list, 0, LV_ANIM_OFF);

    lv_memset_00(explorer->cur_path, sizeof(explorer->cur_path));
    strcpy(explorer->cur_path, path);
    lv_label_set_text_fmt(explorer->path_label, LV_SYMBOL_EYE_OPEN " %s", path);

    size_t cur_path_len = strlen(explorer->cur_path);
    if ((*((explorer->cur_path) + cur_path_len) != '/') && (cur_path_len < LV_FILE_EXPLORER_PATH_MAX_LEN))
    {
        *((explorer->cur_path) + cur_path_len) = '/';
    }
}

// 去掉最后的后缀名
static void strip_ext(char *dir)
{
    char *end = dir + strlen(dir);

    while (end >= dir && *end != '/')
    {
        --end;
    }

    if (end > dir)
    {
        *end = '\0';
    }
    else if (end == dir)
    {
        *(end + 1) = '\0';
    }
}

static void exch_table_item(lv_obj_t *tb, int16_t i, int16_t j)
{
    const char *tmp;
    tmp = lv_table_get_cell_value(tb, i, 0);
    lv_table_set_cell_value(tb, 0, 2, tmp);
    lv_table_set_cell_value(tb, i, 0, lv_table_get_cell_value(tb, j, 0));
    lv_table_set_cell_value(tb, j, 0, lv_table_get_cell_value(tb, 0, 2));

    tmp = lv_table_get_cell_value(tb, i, 1);
    lv_table_set_cell_value(tb, 0, 2, tmp);
    lv_table_set_cell_value(tb, i, 1, lv_table_get_cell_value(tb, j, 1));
    lv_table_set_cell_value(tb, j, 1, lv_table_get_cell_value(tb, 0, 2));
}

// Quick sort 3 way
static void sort_table_items(lv_obj_t *tb, int16_t lo, int16_t hi)
{
    if (lo >= hi)
        return; // 单个元素或者没有元素的情况

    int16_t lt = lo;
    int16_t i = lo + 1; // 第一个元素是切分元素，所以指针i可以从lo+1开始
    int16_t gt = hi;
    const char *v = lv_table_get_cell_value(tb, lo, 1);
    while (i <= gt)
    {
        if (strcmp(lv_table_get_cell_value(tb, i, 1), v) < 0) // 小于切分元素的放在lt左边，因此指针lt和指针i整体右移
            exch_table_item(tb, lt++, i++);
        else if (strcmp(lv_table_get_cell_value(tb, i, 1), v) > 0) // 大于切分元素的放在gt右边，因此指针gt需要左移
            exch_table_item(tb, i, gt--);
        else
            i++;
    }
    // lt-gt的元素已经排定，只需对it左边和gt右边的元素进行递归求解
    sort_table_items(tb, lo, lt - 1);
    sort_table_items(tb, gt + 1, hi);
}

static bool is_begin_with(const char *str1, const char *str2)
{
    if (str1 == NULL || str2 == NULL)
        return false;

    uint16_t len1 = strlen(str1);
    uint16_t len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return false;

    uint16_t i = 0;
    char *p = str2;
    while (*p != '\0')
    {
        if (*p != str1[i])
            return false;
        p++;
        i++;
    }
    return true;
}

static bool is_end_with(const char *str1, const char *str2)
{
    if (str1 == NULL || str2 == NULL)
        return false;

    uint16_t len1 = strlen(str1);
    uint16_t len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return false;

    while (len2 >= 1)
    {
        if (str2[len2 - 1] != str1[len1 - 1])
            return false;
        len2--;
        len1--;
    }
    return true;
}

static void file_explorer_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *cur_path = lv_file_explorer_get_current_path(obj);
        const char *sel_fn = lv_file_explorer_get_selected_file_name(obj);
        uint16_t path_len = strlen(cur_path);
        uint16_t fn_len = strlen(sel_fn);

        if ((path_len + fn_len) <= LV_FILE_EXPLORER_PATH_MAX_LEN)
        {
            char file_info[LV_FILE_EXPLORER_PATH_MAX_LEN];
            strcpy(file_info, cur_path);
            strcat(file_info, sel_fn);
            LV_LOG_USER("%s", file_info);
        }
        else
        {
            LV_LOG_USER("%s%s", cur_path, sel_fn);
        }
    }
}
