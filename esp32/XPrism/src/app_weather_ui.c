#include "app_weather_ui.h"
#include "icons_weather.h"
#include "lvgl.h"
#include "lv_xprism_color.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_wqy_microhei_20);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *basicScr = NULL;
static lv_obj_t *detailScr = NULL;

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

void appWeatherUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&textStyle, &lv_font_wqy_microhei_20);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, LV_COLOR_WHITE);
    lv_style_set_text_font(&numBigStyle, &lv_font_ubuntu_b_108);
}

void appWeatherUiDisplayBasicInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == basicScr)
    {
        return;
    }

    appWeatherUiDelete();
    lv_obj_clean(actObj);

    basicScr = lv_obj_create(NULL);
    lv_obj_add_style(basicScr, &defaultStyle, LV_STATE_DEFAULT);

    weatherImage = lv_img_create(basicScr);
    lv_img_set_src(weatherImage, &weather_sunny);
    lv_obj_set_size(weatherImage, 96, 96);
    lv_obj_set_pos(weatherImage, -48, -48);

    weatherLabel = lv_label_create(basicScr);
    lv_obj_add_style(weatherLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(weatherLabel, "晴");
    lv_obj_set_pos(weatherLabel, -72, 96);

    tempLabel = lv_label_create(basicScr);
    lv_obj_add_style(tempLabel, &numBigStyle, LV_STATE_DEFAULT);
    lv_label_set_text(tempLabel, "25°C");
    lv_obj_set_pos(tempLabel, 48, 48);

    cityLabel = lv_label_create(basicScr);
    lv_obj_add_style(cityLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(cityLabel, "北京");
    lv_obj_set_pos(cityLabel, -72, 48);
}

void appWeatherUiDisplayBasic(struct Weather weaInfo,
                              lv_scr_load_anim_t animType)
{
    appWeatherUiDisplayBasicInit(animType);

    lv_label_set_text(cityLabel, weaInfo.name);
    lv_label_set_text(weatherLabel, weaInfo.weatherDescription);
    lv_label_set_text_fmt(tempLabel, "%.1f°C", weaInfo.mainTemp);

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
        if (weaInfo.dt < weaInfo.sysSunrise || weaInfo.dt > weaInfo.sysSunset)
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
        if (weaInfo.dt < weaInfo.sysSunrise || weaInfo.dt > weaInfo.sysSunset)
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
        if (weaInfo.dt < weaInfo.sysSunrise || weaInfo.dt > weaInfo.sysSunset)
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

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(basicScr, animType, 300, 300, false);
    }
    else
    {
        lv_src_load(basicScr);
    }
}

void appWeatherUiDisplayDetailInit(lv_scr_load_anim_t animType)
{
    lv_obj_t *actObj = lv_scr_act();
    if (actObj == detailScr)
    {
        return;
    }

    appWeatherUiDelete();
    lv_obj_clean(actObj);

    detailScr = lv_obj_create(NULL);
    lv_obj_add_style(detailScr, &defaultStyle, LV_STATE_DEFAULT);

    airPanel = lv_obj_create(detailScr);
    lv_obj_add_style(airPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(airPanel, 240, 40);
    lv_obj_set_pos(airPanel, 0, -75);

    airLabel = lv_label_create(airPanel);
    lv_obj_add_style(airLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(airLabel, "气压与湿度");
    lv_obj_align(airLabel, LV_ALIGN_CENTER, 0, 0);

    windPanel = lv_obj_create(detailScr);
    lv_obj_add_style(windPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(windPanel, 240, 40);
    lv_obj_set_pos(windPanel, 0, -25);

    windLabel = lv_label_create(windPanel);
    lv_obj_add_style(windLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(windLabel, "风速与风向");
    lv_obj_align(windLabel, LV_ALIGN_CENTER, 0, 0);

    cloudPanel = lv_obj_create(detailScr);
    lv_obj_add_style(cloudPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(cloudPanel, 240, 40);
    lv_obj_set_pos(cloudPanel, 0, 25);

    cloudLabel = lv_label_create(cloudPanel);
    lv_obj_add_style(cloudLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(cloudLabel, "云量");
    lv_obj_align(cloudLabel, LV_ALIGN_CENTER, 0, 0);

    sunPanel = lv_obj_create(detailScr);
    lv_obj_add_style(sunPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(sunPanel, 240, 40);
    lv_obj_set_pos(sunPanel, 0, 75);

    sunLabel = lv_label_create(sunPanel);
    lv_obj_add_style(sunLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(sunLabel, "日出与日落");
    lv_obj_align(sunLabel, LV_ALIGN_CENTER, 0, 0);
}

void appWeatherUiDisplayDetail(struct Weather weaInfo,
                               lv_scr_load_anim_t animType)
{
    appWeatherUiDisplayDetailInit(animType);

    lv_label_set_text_fmt(airLabel, "气压：%d hPa\n湿度：%d%%",
                          weaInfo.mainPressure, weaInfo.mainHumidity);
    lv_label_set_text_fmt(windLabel, "风速：%.1f m/s\n风向：%d°",
                          weaInfo.windSpeed, weaInfo.windDeg);
    lv_label_set_text_fmt(cloudLabel, "云量：%d%%", weaInfo.cloudsAll);
    lv_label_set_text_fmt(sunLabel, "日出：%02d:%02d\n日落：%02d:%02d",
                          (weaInfo.sysSunrise / 3600 + weaInfo.timezone / 3600) % 24,
                          (weaInfo.sysSunrise / 60) % 60,
                          (weaInfo.sysSunset / 3600 + weaInfo.timezone / 3600) % 24,
                          (weaInfo.sysSunset / 60) % 60);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(detailScr, animType, 300, 300, false);
    }
    else
    {
        lv_src_load(detailScr);
    }
}

void appWeatherUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_del(basicScr);
        basicScr = NULL;
    }

    if (detailScr != NULL)
    {
        lv_obj_del(detailScr);
        detailScr = NULL;
    }
}
