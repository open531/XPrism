#include "btn.h"
#include "common.h"

const char *btnActionTypeInfo[] = {"BTN_NONE",
                                   "BTN_BACK",
                                   "BTN_FORWARD",
                                   "BTN_LEFT",
                                   "BTN_RIGHT"};

Buttons::Buttons()
{
    btnActionInfo.isValid = false;
    // 初始化数据
    for (int pos = 0; pos < ACTION_HISTORY_BUF_LEN; ++pos)
    {
        btnActionInfoHistory[pos] = BTN_NONE;
    }
    btnActionInfoHistoryInd = ACTION_HISTORY_BUF_LEN - 1;
}

void Buttons::init()
{
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);
    pinMode(BTN_FORWARD_PIN, INPUT_PULLUP);
    pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
    pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
}

BTNAction *Buttons::update(int interval)
{
    btnActionInfo.btnAction = BTN_NONE;
    if (GET_SYS_MILLIS() - lastUpdate > interval)
    {
        if (!btnActionInfo.isValid)
        {
            if (digitalRead(BTN_BACK_PIN) == LOW)
            {
                btnActionInfo.btnAction = BTN_BACK;
                btnActionInfo.isValid = true;
            }
            else if (digitalRead(BTN_FORWARD_PIN) == LOW)
            {
                btnActionInfo.btnAction = BTN_FORWARD;
                btnActionInfo.isValid = true;
            }
            else if (digitalRead(BTN_LEFT_PIN) == LOW)
            {
                btnActionInfo.btnAction = BTN_LEFT;
                btnActionInfo.isValid = true;
            }
            else if (digitalRead(BTN_RIGHT_PIN) == LOW)
            {
                btnActionInfo.btnAction = BTN_RIGHT;
                btnActionInfo.isValid = true;
            }
            else
            {
                btnActionInfo.isValid = false;
            }
        }
        else
        {
            if (digitalRead(BTN_BACK_PIN) == HIGH &&
                digitalRead(BTN_FORWARD_PIN) == HIGH &&
                digitalRead(BTN_LEFT_PIN) == HIGH &&
                digitalRead(BTN_RIGHT_PIN) == HIGH)
            {
                btnActionInfo.btnAction = BTN_NONE;
                btnActionInfo.isValid = false;
            }
        }
        lastUpdate = GET_SYS_MILLIS();
    }
    return &btnActionInfo;
}

BTNAction *Buttons::getAction(void)
{
    btnActionInfo.btnAction = BTN_NONE;
    if (!btnActionInfo.isValid)
    {
        if (digitalRead(BTN_BACK_PIN) == LOW)
        {
            btnActionInfo.btnAction = BTN_BACK;
            btnActionInfo.isValid = true;
        }
        else if (digitalRead(BTN_FORWARD_PIN) == LOW)
        {
            btnActionInfo.btnAction = BTN_FORWARD;
            btnActionInfo.isValid = true;
        }
        else if (digitalRead(BTN_LEFT_PIN) == LOW)
        {
            btnActionInfo.btnAction = BTN_LEFT;
            btnActionInfo.isValid = true;
        }
        else if (digitalRead(BTN_RIGHT_PIN) == LOW)
        {
            btnActionInfo.btnAction = BTN_RIGHT;
            btnActionInfo.isValid = true;
        }
        else
        {
            btnActionInfo.isValid = false;
        }
    }
    else
    {
        if (digitalRead(BTN_BACK_PIN) == HIGH &&
            digitalRead(BTN_FORWARD_PIN) == HIGH &&
            digitalRead(BTN_LEFT_PIN) == HIGH &&
            digitalRead(BTN_RIGHT_PIN) == HIGH)
        {
            btnActionInfo.btnAction = BTN_NONE;
            btnActionInfo.isValid = false;
        }
    }
    lastUpdate = GET_SYS_MILLIS();
    return &btnActionInfo;
}