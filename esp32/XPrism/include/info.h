#ifndef INFO_H
#define INFO_H

struct Weather
{
    int id;
    char city[10];
    char main[20];
    char description[30];
    double temp;
    double feelsLike;
    int pressure;
    int humidity;
    double windSpeed;
    int windDeg;
    int clouds;
    int visibility;
    long dt;
    long sunrise;
    long sunset;
};

#endif
