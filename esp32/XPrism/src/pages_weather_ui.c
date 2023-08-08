#include "pages_weather_ui.h"
#include "icons_weather.h"
#include "info.h"
#include "lv_xprism_color.h"
#include "lvgl.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_20);

static lv_style_t default_style;
static lv_style_t zh_cn_style;
static lv_style_t numberSmall_style;
static lv_style_t numberBig_style;
static lv_style_t btn_style;
static lv_style_t bar_style;

static lv_obj_t *scr_1 = NULL;
static lv_obj_t *scr_2 = NULL;

static lv_obj_t *weatherImage = NULL;
static lv_obj_t *weatherLabel = NULL;
static lv_obj_t *tempLabel = NULL;
static lv_obj_t *cityLabel = NULL;
static lv_obj_t *airPanel = NULL;
static lv_obj_t *airLabel = NULL;
static lv_obj_t *windPanel = NULL;
static lv_obj_t *windLabel = NULL;
static lv_obj_t *cloudPanel = NULL;
static lv_obj_t *cloudLabel = NULL;
static lv_obj_t *sunPanel = NULL;
static lv_obj_t *sunLabel = NULL;

void weather_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, LV_COLOR_BLACK);

    lv_style_init(&zh_cn_style);
    lv_style_set_text_opa(&zh_cn_style, LV_OPA_COVER);
    lv_style_set_text_color(&zh_cn_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&zh_cn_style, &lv_font_wqy_microhei_20);

    lv_style_init(&numberSmall_style);
    lv_style_set_text_opa(&numberSmall_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberSmall_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberSmall_style, &lv_font_ubuntu_b_64);

    lv_style_init(&numberBig_style);
    lv_style_set_text_opa(&numberBig_style, LV_OPA_COVER);
    lv_style_set_text_color(&numberBig_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberBig_style, &lv_font_ubuntu_b_108);

    lv_style_init(&btn_style);
    lv_style_set_border_width(&bar_style, 0);

    lv_style_init(&bar_style);
    lv_style_set_bg_color(&bar_style, LV_COLOR_BLACK);
    lv_style_set_border_width(&bar_style, 2);
    lv_style_set_border_color(&bar_style, LV_COLOR_WHITE);
    lv_style_set_pad_top(&bar_style, 1);
    lv_style_set_pad_bottom(&bar_style, 1);
    lv_style_set_pad_left(&bar_style, 1);
    lv_style_set_pad_right(&bar_style, 1);
}

void display_weather_init(lv_scr_load_anim_t anim_type)
{
    lv_obj_t *act_obj = lv_scr_act();
    if (act_obj == scr_1)
    {
        return;
    }

    weather_gui_release();
    lv_obj_clean(act_obj);

    // screen
    scr_1 = lv_obj_create(NULL);
    lv_obj_add_style(scr_1, &default_style, LV_STATE_DEFAULT);

    // widgets
    weatherImage = lv_img_create(scr_1);
    lv_img_set_src(weatherImage, &weather_sunny);
    lv_obj_set_size(weatherImage, 96, 96);
    lv_obj_set_pos(weatherImage, -48, -48);

    weatherLabel = lv_label_create(scr_1);
    lv_obj_add_style(weatherLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(weatherLabel, "晴");
    lv_obj_set_pos(weatherLabel, -72, 96);

    tempLabel = lv_label_create(scr_1);
    lv_obj_add_style(tempLabel, &numberBig_style, LV_STATE_DEFAULT);
    lv_label_set_text(tempLabel, "25°C");
    lv_obj_set_pos(tempLabel, 48, 48);

    cityLabel = lv_label_create(scr_1);
    lv_obj_add_style(cityLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(cityLabel, "北京");
    lv_obj_set_pos(cityLabel, -72, 48);

    // TODO?
}

void display_weather(struct Weather weaInfo, lv_scr_load_anim_t anim_type)
{
    display_weather_init(anim_type);

    lv_label_set_text(cityLabel, weaInfo.name);
    lv_label_set_text(weatherLabel, weaInfo.description);
    lv_label_set_text_fmt(tempLabel, "%2d°C", weaInfo.temp);

    // weather icon
    lv_img_dsc_t *img = NULL;
    switch (weaInfo.id)
    {
    case 200:
    case 201:
    case 202:
    case 221:
    case 230:
    case 231:
    case 232:
        img = &weather_lightning_rainy;
        break;
    case 210:
    case 211:
    case 212:
        img = &weather_lightning;
        break;
    case 300:
    case 301:
    case 302:
    case 310:
    case 311:
    case 312:
    case 313:
    case 314:
    case 321:
    case 500:
    case 501:
    case 520:
    case 521:
    case 522:
    case 531:
        img = &weather_rainy;
        break;
    case 502:
    case 503:
    case 504:
        img = &weather_pouring;
        break;
    case 511:
    case 611:
    case 612:
    case 613:
    case 615:
    case 616:
        img = &weather_snowy_rainy;
        break;
    case 600:
    case 601:
    case 620:
    case 621:
        img = &weather_snowy;
        break;
    case 602:
    case 622:
        img = &weather_snowy_heavy;
        break;
    case 701:
    case 711:
    case 721:
    case 741:
    case 771:
        img = &weather_fog;
        break;
    case 731:
    case 751:
    case 761:
    case 762:
        img = &weather_dust;
        break;
    case 781:
        img = &weather_tornado;
        break;
    case 800:
        if (weaInfo.dt < weaInfo.sunrise || weaInfo.dt > weaInfo.sunset)
        {
            img = &weather_night;
        }
        else
        {
            img = &weather_sunny;
        }
        break;
    case 801:
    case 802:
        if (weaInfo.dt < weaInfo.sunrise || weaInfo.dt > weaInfo.sunset)
        {
            img = &weather_night_partly_cloudy;
        }
        else
        {
            img = &weather_partly_cloudy;
        }
        break;
    case 803:
    case 804:
        img = &weather_cloudy;
        break;
    default:
        if (weaInfo.dt < weaInfo.sunrise || weaInfo.dt > weaInfo.sunset)
        {
            img = &weather_night;
        }
        else
        {
            img = &weather_sunny;
        }
        break;
    }
    lv_img_set_src(weatherImage, img);

    if (anim_type != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    }
    else
    {
        lv_scr_load(scr_1);
    }
}

void display_detail_init(lv_scr_load_anim_t anim_type)
{
    lv_obj_t *act_obj = lv_scr_act();
    if (act_obj == scr_2)
    {
        return;
    }

    weather_gui_release();
    lv_obj_clean(act_obj);

    // screen
    scr_2 = lv_obj_create(NULL);
    lv_obj_add_style(scr_2, &default_style, LV_STATE_DEFAULT);

    // widgets
    airPanel = lv_cont_create(scr_2);
    lv_obj_add_style(airPanel, &bar_style, LV_STATE_DEFAULT);
    lv_obj_set_size(airPanel, 240, 40);
    lv_obj_set_pos(airPanel, 0, 0);

    airLabel = lv_label_create(airPanel);
    lv_obj_add_style(airLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(airLabel, "气压与湿度");
    lv_obj_set_pos(airLabel, 0, 0);

    windPanel = lv_cont_create(scr_2);
    lv_obj_add_style(windPanel, &bar_style, LV_STATE_DEFAULT);
    lv_obj_set_size(windPanel, 240, 40);
    lv_obj_set_pos(windPanel, 0, 40);

    windLabel = lv_label_create(windPanel);
    lv_obj_add_style(windLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(windLabel, "风速与风向");
    lv_obj_set_pos(windLabel, 0, 0);

    cloudPanel = lv_cont_create(scr_2);
    lv_obj_add_style(cloudPanel, &bar_style, LV_STATE_DEFAULT);
    lv_obj_set_size(cloudPanel, 240, 40);
    lv_obj_set_pos(cloudPanel, 0, 80);

    cloudLabel = lv_label_create(cloudPanel);
    lv_obj_add_style(cloudLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(cloudLabel, "云量");
    lv_obj_set_pos(cloudLabel, 0, 0);

    sunPanel = lv_cont_create(scr_2);
    lv_obj_add_style(sunPanel, &bar_style, LV_STATE_DEFAULT);
    lv_obj_set_size(sunPanel, 240, 40);
    lv_obj_set_pos(sunPanel, 0, 120);

    sunLabel = lv_label_create(sunPanel);
    lv_obj_add_style(sunLabel, &zh_cn_style, LV_STATE_DEFAULT);
    lv_label_set_text(sunLabel, "日出日落");
    lv_obj_set_pos(sunLabel, 0, 0);

    // TODO?
}

void display_detail(struct Weather weaInfo, lv_scr_load_anim_t anim_type)
{
    display_detail_init(anim_type);

    lv_label_set_text_fmt(airLabel, "气压: %2d kPa\t湿度: %2d%", weaInfo.pressure, weaInfo.humidity);
    lv_label_set_text_fmt(windLabel, "风速: %2d m/s\t风向: %d°", weaInfo.wind_speed, weaInfo.wind_deg);
    lv_label_set_text_fmt(cloudLabel, "云量: %d%%", weaInfo.clouds);
    lv_label_set_text_fmt(sunLabel, "日出: %d\t日落: %d", weaInfo.sunrise, weaInfo.sunset);

    if (anim_type != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(scr_2, anim_type, 300, 300, false);
    }
    else
    {
        lv_scr_load(scr_2);
    }
}

void weather_gui_release(void)
{
    if (scr_1 != NULL)
    {
        lv_obj_clean(scr_1);
        scr_1 = NULL;
        weatherImage = NULL;
        weatherLabel = NULL;
        tempLabel = NULL;
        cityLabel = NULL;
    }
    if (scr_2 != NULL)
    {
        lv_obj_clean(scr_2);
        scr_2 = NULL;
        airPanel = NULL;
        airLabel = NULL;
        windPanel = NULL;
        windLabel = NULL;
        cloudPanel = NULL;
        cloudLabel = NULL;
        sunPanel = NULL;
        sunLabel = NULL;
    }
}

void weather_gui_del(void)
{
    weather_gui_release();
}
