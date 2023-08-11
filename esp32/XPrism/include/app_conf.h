#ifndef APP_CONF_H
#define APP_CONF_H

#define INSTALL_APP_TIME 0
#define INSTALL_APP_WEATHER 1
#define INSTALL_APP_CLOCK 0
#define INSTALL_APP_FILE 0

#if INSTALL_APP_TIME
#include "app_time.h"
#endif

#if INSTALL_APP_WEATHER
#include "app_weather.h"
#endif

#if INSTALL_APP_CLOCK
#include "app_clock.h"
#endif

#if INSTALL_APP_FILE
#include "app_file.h"
#endif

#endif