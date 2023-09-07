#ifndef ACTION_H
#define ACTION_H

#include <Arduino.h>

extern const char *actionTypeInfo[];

enum ActionType
{
    ACT_NONE,
    ACT_BACK,
    ACT_FORWARD,
    ACT_LEFT,
    ACT_RIGHT,
    ACT_UP,
    ACT_DOWN,
};

struct Action
{
    volatile ActionType action;
    boolean isValid;
};

#endif