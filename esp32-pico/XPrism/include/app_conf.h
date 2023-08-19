#ifndef _APP_CONF_H
#define _APP_CONF_H

/**
 * 将某项APP对应的宏定义设置为1，可以将对应APP的
 * 代码编译进固件，设置为0则不编译进固件。便于腾
 * 出额外的flash空间给其他用户自定义app。
 * 注: 当某个app不使能时，对应的web配置项也不会
 * 出现在配置网页上。
 */

// 在开发新的app时，可以先把其他所有APP关闭，这样烧录速度也比较快

#define APP_TIME_USE 1
#if APP_TIME_USE
#include "app_time.h"
#endif
#define APP_WEATHER_USE 1
#if APP_WEATHER_USE
#include "app_weather.h"
#endif
#define APP_CLOCK_USE 1
#if APP_CLOCK_USE
#include "app_clock.h"
#endif
#define APP_NAVI_USE 1
#if APP_NAVI_USE
#include "app_navi.h"
#endif
#define APP_INFO_USE 1
#if APP_INFO_USE
#include "app_info.h"
#endif
#define APP_NOTI_USE 1
#if APP_NOTI_USE
#include "app_noti.h"
#endif
#define APP_FILE_USE 1
#if APP_FILE_USE
#include "app_file.h"
#endif

#endif
