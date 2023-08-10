#ifndef INFO_H
#define INFO_H

struct Time
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int weekday;
    unsigned long timestamp;
};

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

#endif
