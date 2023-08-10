#include "button.h"

const char *buttonActionInfo[] = {"Back", "Forward", "Left", "Right", "None"};

Buttons::Buttons()
    : m_btnBack(PIN_BTN_BACK, true, true),
      m_btnForward(PIN_BTN_FORWARD, true, true),
      m_btnLeft(PIN_BTN_LEFT, true, true),
      m_btnRight(PIN_BTN_RIGHT, true, true)
{
}

void Buttons::routine()
{
    m_btnBack.tick();
    m_btnForward.tick();
    m_btnLeft.tick();
    m_btnRight.tick();
}

void Buttons::attachClick(ButtonAction action, void (*callback)(void))
{
    switch (action)
    {
    case BTN_BACK:
        m_btnBack.attachClick(callback);
        break;
    case BTN_FORWARD:
        m_btnForward.attachClick(callback);
        break;
    case BTN_LEFT:
        m_btnLeft.attachClick(callback);
        break;
    case BTN_RIGHT:
        m_btnRight.attachClick(callback);
        break;
    default:
        break;
    }
}

void Buttons::attachDoubleClick(ButtonAction action, void (*callback)(void))
{
    switch (action)
    {
    case BTN_BACK:
        m_btnBack.attachDoubleClick(callback);
        break;
    case BTN_FORWARD:
        m_btnForward.attachDoubleClick(callback);
        break;
    case BTN_LEFT:
        m_btnLeft.attachDoubleClick(callback);
        break;
    case BTN_RIGHT:
        m_btnRight.attachDoubleClick(callback);
        break;
    default:
        break;
    }
}

void Buttons::attachLongPressStart(ButtonAction action, void (*callback)(void))
{
    switch (action)
    {
    case BTN_BACK:
        m_btnBack.attachLongPressStart(callback);
        break;
    case BTN_FORWARD:
        m_btnForward.attachLongPressStart(callback);
        break;
    case BTN_LEFT:
        m_btnLeft.attachLongPressStart(callback);
        break;
    case BTN_RIGHT:
        m_btnRight.attachLongPressStart(callback);
        break;
    default:
        break;
    }
}

void Buttons::attachLongPressStop(ButtonAction action, void (*callback)(void))
{
    switch (action)
    {
    case BTN_BACK:
        m_btnBack.attachLongPressStop(callback);
        break;
    case BTN_FORWARD:
        m_btnForward.attachLongPressStop(callback);
        break;
    case BTN_LEFT:
        m_btnLeft.attachLongPressStop(callback);
        break;
    case BTN_RIGHT:
        m_btnRight.attachLongPressStop(callback);
        break;
    default:
        break;
    }
}

void Buttons::attachDuringLongPress(ButtonAction action, void (*callback)(void))
{
    switch (action)
    {
    case BTN_BACK:
        m_btnBack.attachDuringLongPress(callback);
        break;
    case BTN_FORWARD:
        m_btnForward.attachDuringLongPress(callback);
        break;
    case BTN_LEFT:
        m_btnLeft.attachDuringLongPress(callback);
        break;
    case BTN_RIGHT:
        m_btnRight.attachDuringLongPress(callback);
        break;
    default:
        break;
    }
}
