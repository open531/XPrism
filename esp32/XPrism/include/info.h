#ifndef INFO_H
#define INFO_H

#include <WString.h>

struct Weather
{
    int id;
    String name;
    String main;
    String description;
    double temp;
    double feels_like;
    int pressure;
    int humidity;
    double wind_speed;
    int wind_deg;
    int clouds;
    int visibility;
    long dt;
    long sunrise;
    long sunset;
};

#endif