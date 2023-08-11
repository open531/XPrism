#ifndef BUTTON_H
#define BUTTON_H

#include "OneButton.h"
#include "pin.h"

extern const char *buttonActionInfo[];

enum ButtonAction
{
    BTN_BACK,
    BTN_FORWARD,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_NONE
};

class Buttons
{
private:
    OneButton m_btnBack;
    OneButton m_btnForward;
    OneButton m_btnLeft;
    OneButton m_btnRight;

public:
    Buttons();
    void routine();
    void attachClick(ButtonAction action, void (*callback)(void));
    void attachDoubleClick(ButtonAction action, void (*callback)(void));
    void attachLongPressStart(ButtonAction action, void (*callback)(void));
    void attachLongPressStop(ButtonAction action, void (*callback)(void));
    void attachDuringLongPress(ButtonAction action, void (*callback)(void));
};

#endif