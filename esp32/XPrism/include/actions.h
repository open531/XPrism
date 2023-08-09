#ifndef ACTIONS_H
#define ACTIONS_H

#include "OneButton.h"

#define PIN_BTN_BACK 35
#define PIN_BTN_FORWARD 38
#define PIN_BTN_LEFT 37
#define PIN_BTN_RIGHT 34

enum ACTION_TYPE
{
    UNKNOWN = 0,
    BACK,
    FORWARD,
    LEFT,
    RIGHT,
};

enum ACTION_STATE
{
    NONE = 0,
    CLICK,
    DOUBLE_CLICK,
    MULTI_CLICK,
    LONG_PRESS_START,
    LONG_PRESS_STOP,
    DURING_LONG_PRESS,
};

struct Action
{
    volatile ACTION_TYPE type;
    volatile ACTION_STATE state;
    boolean isValid;
};

extern char *action_type_info[];

class Buttons
{
private:
    OneButton btn_back;
    OneButton btn_forward;
    OneButton btn_left;
    OneButton btn_right;

public:
    Buttons();
    void init(const int pin_back = PIN_BTN_BACK,
              const int pin_forward = PIN_BTN_FORWARD,
              const int pin_left = PIN_BTN_LEFT,
              const int pin_right = PIN_BTN_RIGHT);
    void tick();
    void reset();
    void attachClick(ACTION_TYPE type, callbackFunction newFunction);
    void attachDoubleClick(ACTION_TYPE type, callbackFunction newFunction);
    void attachMultiClick(ACTION_TYPE type, callbackFunction newFunction);
    void attachLongPressStart(ACTION_TYPE type, callbackFunction newFunction);
    void attachLongPressStop(ACTION_TYPE type, callbackFunction newFunction);
    void attachDuringLongPress(ACTION_TYPE type, callbackFunction newFunction);
    Action *get_action();
};

#endif