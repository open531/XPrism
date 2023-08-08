#ifndef ACTIONS_H
#define ACTIONS_H

#include "lvgl.h"

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
    boolean isValid;
    boolean long_time;
};

#endif