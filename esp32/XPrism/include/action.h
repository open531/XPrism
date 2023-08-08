#ifndef ACTIONS_H
#define ACTIONS_H

#include "lvgl.h"
#include <stdbool.h>

enum ACTION_TYPE
{
    BACK = 0,
    FORWARD,
    LEFT,
    RIGHT,
};

struct Action
{
    ACTION_TYPE type;
    int isValid;
    int long_time;
};

#endif