#ifndef PAGES_CONF_H
#define PAGES_CONF_H

// List of all the pages
#define INSTALL_PAGE_TIME true
#if INSTALL_PAGE_TIME
#include "pages_time.h"
#endif

#define INSTALL_PAGE_WEATHER true
#if INSTALL_PAGE_WEATHER
#include "pages_weather.h"
#endif

#define INSTALL_PAGE_CLOCK true
#if INSTALL_PAGE_CLOCK
#include "pages_clock.h"
#endif

#define INSTALL_PAGE_FILE true
#if INSTALL_PAGE_FILE
#include "pages_file.h"
#endif

#endif