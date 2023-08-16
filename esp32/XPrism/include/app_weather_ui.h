#ifndef APP_WEATHER_UI_H
#define APP_WEATHER_UI_H

struct Weather
{
    int weatherId;
    char weatherMain[20];
    char weatherDescription[50];
    char weatherIcon[5];
    char base[10];
    double mainTemp;
    double mainFeelsLike;
    double mainTempMin;
    double mainTempMax;
    int mainPressure;
    int mainHumidity;
    int mainSeaLevel;
    int mainGrndLevel;
    int visibility;
    double windSpeed;
    int windDeg;
    double windGust;
    double rain1h;
    double rain3h;
    double snow1h;
    double snow3h;
    int cloudsAll;
    unsigned long dt;
    char sysCountry[5];
    unsigned long sysSunrise;
    unsigned long sysSunset;
    int timezone;
    int id;
    char name[50];
    int cod;
};

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND_WAIT                 \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void appWeatherUiInit();
    void appWeatherUiDisplayBasicInit(lv_scr_load_anim_t animType);
    void appWeatherUiDisplayBasic(struct Weather weaInfo, lv_scr_load_anim_t animType);
    void appWeatherUiDisplayDetailInit(lv_scr_load_anim_t animType);
    void appWeatherUiDisplayDetail(struct Weather weaInfo, lv_scr_load_anim_t animType);
    void appWeatherUiDelete();

    extern const lv_img_dsc_t icon_weather;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif