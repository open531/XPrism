#include "app_weather_ui.h"
#include "icons_weather.h"
#include "lvgl.h"

LV_FONT_DECLARE(lv_font_ubuntu_b_108)
LV_FONT_DECLARE(lv_font_ubuntu_b_64)
LV_FONT_DECLARE(lv_font_source_han_sans_24);

static lv_style_t defaultStyle;
static lv_style_t textStyle;
static lv_style_t numSmallStyle;
static lv_style_t numBigStyle;

static lv_obj_t *basicScr = NULL;
static lv_obj_t *detailScr = NULL;

static lv_obj_t *weatherImage = NULL;
static lv_obj_t *weatherLabel = NULL;
static lv_obj_t *tempLabel = NULL;
static lv_obj_t *feelsLikeLabel = NULL;
static lv_obj_t *humidityLabel = NULL;
static lv_obj_t *cityLabel = NULL;
static lv_obj_t *airPanel = NULL;
static lv_obj_t *airLabel = NULL;
static lv_obj_t *windPanel = NULL;
static lv_obj_t *windLabel = NULL;
static lv_obj_t *srcPanel = NULL;
static lv_obj_t *srcLabel = NULL;

void appWeatherUiInit()
{
    lv_style_init(&defaultStyle);
    lv_style_set_bg_color(&defaultStyle, lv_color_hex(0x000000));

    lv_style_init(&textStyle);
    lv_style_set_text_opa(&textStyle, LV_OPA_COVER);
    lv_style_set_text_color(&textStyle, lv_color_hex(0xffffff));
    lv_style_set_text_font(&textStyle, &lv_font_source_han_sans_24);

    lv_style_init(&numSmallStyle);
    lv_style_set_text_opa(&numSmallStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numSmallStyle, lv_color_hex(0xffffff));
    lv_style_set_text_font(&numSmallStyle, &lv_font_ubuntu_b_64);

    lv_style_init(&numBigStyle);
    lv_style_set_text_opa(&numBigStyle, LV_OPA_COVER);
    lv_style_set_text_color(&numBigStyle, lv_color_hex(0xffffff));
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

    weatherLabel = lv_label_create(basicScr);
    lv_obj_add_style(weatherLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(weatherLabel, "晴");

    tempLabel = lv_label_create(basicScr);
    lv_obj_add_style(tempLabel, &numSmallStyle, LV_STATE_DEFAULT);
    lv_label_set_text(tempLabel, "25");

    feelsLikeLabel = lv_label_create(basicScr);
    lv_obj_add_style(feelsLikeLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(feelsLikeLabel, "体感：25°C");

    humidityLabel = lv_label_create(basicScr);
    lv_obj_add_style(humidityLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(humidityLabel, "湿度：25%%");

    cityLabel = lv_label_create(basicScr);
    lv_obj_add_style(cityLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_recolor(cityLabel, true);
    lv_label_set_text(cityLabel, "北京");

    lv_obj_align(weatherImage, LV_ALIGN_TOP_RIGHT, -60, 0);
    lv_obj_align(weatherLabel, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_align(tempLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_align(feelsLikeLabel, LV_ALIGN_RIGHT_MID, 0, 60);
    lv_obj_align(humidityLabel, LV_ALIGN_RIGHT_MID, 0, 90);
    lv_obj_align(cityLabel, LV_ALIGN_TOP_RIGHT, 0, 30);
}

void appWeatherUiDisplayBasic(struct Weather weaInfo,
                              lv_scr_load_anim_t animType)
{
    appWeatherUiDisplayBasicInit(animType);

    lv_label_set_text(cityLabel, weaInfo.name);
    lv_label_set_text(weatherLabel, weaInfo.weatherDescription);
    lv_label_set_text_fmt(tempLabel, "%.2f", weaInfo.mainTemp);
    lv_label_set_text_fmt(feelsLikeLabel, "体感：%.2f°C", weaInfo.mainFeelsLike);
    lv_label_set_text_fmt(humidityLabel, "湿度：%d%%", weaInfo.mainHumidity);

    lv_img_dsc_t *img = NULL;
    switch (weaInfo.weatherId)
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
    case 502:
    case 503:
    case 504:
    case 520:
    case 521:
    case 522:
    case 531:
        img = &weather_rainy;
        break;
    case 511:
    case 600:
    case 601:
    case 602:
    case 611:
    case 612:
    case 613:
    case 615:
    case 616:
    case 620:
    case 621:
    case 622:
        img = &weather_snowy;
        break;
    case 701:
    case 711:
    case 721:
    case 731:
    case 741:
    case 751:
    case 761:
    case 762:
    case 771:
        img = &weather_fog;
        break;
    case 781:
        img = &weather_tornado;
        break;
    case 800:
        img = &weather_sunny;
        break;
    case 801:
    case 802:
        img = &weather_partly_cloudy;
        break;
    case 803:
    case 804:
        img = &weather_cloudy;
        break;
    default:
        img = &weather_sunny;
        break;
    }
    lv_img_set_src(weatherImage, img);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(basicScr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(basicScr);
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
    lv_obj_set_width(airPanel, 200);
    lv_obj_set_height(airPanel, 70);

    airLabel = lv_label_create(airPanel);
    lv_obj_add_style(airLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(airLabel, "气压与云量");
    lv_obj_align(airLabel, LV_ALIGN_CENTER, 0, 0);

    windPanel = lv_obj_create(detailScr);
    lv_obj_add_style(windPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_width(windPanel, 200);
    lv_obj_set_height(windPanel, 70);

    windLabel = lv_label_create(windPanel);
    lv_obj_add_style(windLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(windLabel, "风速与风向");
    lv_obj_align(windLabel, LV_ALIGN_CENTER, 0, 0);

    srcPanel = lv_obj_create(detailScr);
    lv_obj_add_style(srcPanel, &defaultStyle, LV_STATE_DEFAULT);
    lv_obj_set_width(srcPanel, 200);
    lv_obj_set_height(srcPanel, 70);

    srcLabel = lv_label_create(srcPanel);
    lv_obj_add_style(srcLabel, &textStyle, LV_STATE_DEFAULT);
    lv_label_set_text(srcLabel, "数据来源：\nOpenWeatherMap");
    lv_obj_align(srcLabel, LV_ALIGN_CENTER, 0, 0);

    lv_obj_align(airPanel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(windPanel, LV_ALIGN_TOP_MID, 0, 80);
    lv_obj_align(srcPanel, LV_ALIGN_TOP_MID, 0, 160);

    if (animType != LV_SCR_LOAD_ANIM_NONE)
    {
        lv_scr_load_anim(detailScr, animType, 300, 300, false);
    }
    else
    {
        lv_scr_load(detailScr);
    }
}

void appWeatherUiDisplayDetail(struct Weather weaInfo,
                               lv_scr_load_anim_t animType)
{
    appWeatherUiDisplayDetailInit(animType);

    lv_label_set_text_fmt(airLabel, "气压：%d hPa\n云量：%d%%",
                          weaInfo.mainPressure, weaInfo.cloudsAll);
    lv_label_set_text_fmt(windLabel, "风速：%.1f m/s\n风向：%d°",
                          weaInfo.windSpeed, weaInfo.windDeg);
}

void appWeatherUiDelete()
{
    if (basicScr != NULL)
    {
        lv_obj_clean(basicScr);
        basicScr = NULL;
    }

    if (detailScr != NULL)
    {
        lv_obj_clean(detailScr);
        detailScr = NULL;
    }
}
