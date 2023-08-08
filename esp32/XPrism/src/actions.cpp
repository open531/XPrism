#include "actions.h"
#include "system.h"

const char *action_type_info[] = {
    "BACK",
    "FORWARD",
    "LEFT",
    "RIGHT",
};

Buttons::Buttons()
{
}

void Buttons::init(const int pin_back = PIN_BTN_BACK,
                   const int pin_forward = PIN_BTN_FORWARD,
                   const int pin_left = PIN_BTN_LEFT,
                   const int pin_right = PIN_BTN_RIGHT)
{
    btn_back = OneButton(pin_back, true);
    btn_forward = OneButton(pin_forward, true);
    btn_left = OneButton(pin_left, true);
    btn_right = OneButton(pin_right, true);
}

void Buttons::tick()
{
    btn_back.tick();
    btn_forward.tick();
    btn_left.tick();
    btn_right.tick();
}

void Buttons::reset()
{
    btn_back.reset();
    btn_forward.reset();
    btn_left.reset();
    btn_right.reset();
}

void Buttons::attachClick(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachClick(newFunction);
        break;
    case FORWARD:
        btn_forward.attachClick(newFunction);
        break;
    case LEFT:
        btn_left.attachClick(newFunction);
        break;
    case RIGHT:
        btn_right.attachClick(newFunction);
        break;
    default:
        break;
    }
}

void Buttons::attachDoubleClick(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachDoubleClick(newFunction);
        break;
    case FORWARD:
        btn_forward.attachDoubleClick(newFunction);
        break;
    case LEFT:
        btn_left.attachDoubleClick(newFunction);
        break;
    case RIGHT:
        btn_right.attachDoubleClick(newFunction);
        break;
    default:
        break;
    }
}

void Buttons::attachMultiClick(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachMultiClick(newFunction);
        break;
    case FORWARD:
        btn_forward.attachMultiClick(newFunction);
        break;
    case LEFT:
        btn_left.attachMultiClick(newFunction);
        break;
    case RIGHT:
        btn_right.attachMultiClick(newFunction);
        break;
    default:
        break;
    }
}

void Buttons::attachLongPressStart(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachLongPressStart(newFunction);
        break;
    case FORWARD:
        btn_forward.attachLongPressStart(newFunction);
        break;
    case LEFT:
        btn_left.attachLongPressStart(newFunction);
        break;
    case RIGHT:
        btn_right.attachLongPressStart(newFunction);
        break;
    default:
        break;
    }
}

void Buttons::attachLongPressStop(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachLongPressStop(newFunction);
        break;
    case FORWARD:
        btn_forward.attachLongPressStop(newFunction);
        break;
    case LEFT:
        btn_left.attachLongPressStop(newFunction);
        break;
    case RIGHT:
        btn_right.attachLongPressStop(newFunction);
        break;
    default:
        break;
    }
}

void Buttons::attachDuringLongPress(ACTION_TYPE type, callbackFunction newFunction)
{
    switch (type)
    {
    case BACK:
        btn_back.attachDuringLongPress(newFunction);
        break;
    case FORWARD:
        btn_forward.attachDuringLongPress(newFunction);
        break;
    case LEFT:
        btn_left.attachDuringLongPress(newFunction);
        break;
    case RIGHT:
        btn_right.attachDuringLongPress(newFunction);
        break;
    default:
        break;
    }
}

Action *Buttons::get_action()
{
    static Action action;
    action.isValid = false;
    // action.long_time = false;
    action.type = ACTION_TYPE::UNKNOWN;

    // if (btn_back.isLongPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = true;
    //     action.type = BACK;
    // }
    // else if (btn_forward.isLongPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = true;
    //     action.type = FORWARD;
    // }
    // else if (btn_left.isLongPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = true;
    //     action.type = LEFT;
    // }
    // else if (btn_right.isLongPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = true;
    //     action.type = RIGHT;
    // }
    // else if (btn_back.isPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = false;
    //     action.type = BACK;
    // }
    // else if (btn_forward.isPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = false;
    //     action.type = FORWARD;
    // }
    // else if (btn_left.isPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = false;
    //     action.type = LEFT;
    // }
    // else if (btn_right.isPressed())
    // {
    //     action.isValid = true;
    //     action.long_time = false;
    //     action.type = RIGHT;
    // }

    return &action;
}