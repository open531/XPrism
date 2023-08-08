#include "pages_time.h"
#include "pages_time_ui.h"
#include "system.h"
#include "pages_main.h"
#include "ArduinoJson.h"
#include <esp32-hal-timer.h>
#include <map>

#include "lvgl.h"
#define Time_APP_NAME "时间"
#define TIME_CONFIG_PATH "/time.json"

static const char weekDayCh[7][4] = {"日", "一", "二", "三", "四", "五", "六"};
