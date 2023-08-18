#include "btn.h"
#include "common.h"

const char *action_type_info[] = {"BTN_NONE", "BTN_BACK", "BTN_FORWARD", "BTN_LEFT", "BTN_RIGHT"};

Buttons::Buttons()
{
    action_info.isValid = false;
    // 初始化数据
    for (int pos = 0; pos < ACTION_HISTORY_BUF_LEN; ++pos)
    {
        act_info_history[pos] = ActionType::BTN_NONE;
    }
    act_info_history_ind = ACTION_HISTORY_BUF_LEN - 1;
}

void Buttons::init()
{
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);
    pinMode(BTN_FORWARD_PIN, INPUT_PULLUP);
    pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
    pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
}

Action *Buttons::update(int interval)
{
    if (GET_SYS_MILLIS() - last_update_time > interval)
    {
        if (!action_info.isValid)
        {
            if (digitalRead(BTN_BACK_PIN) == LOW)
            {
                action_info.active = ActionType::BTN_BACK;
                action_info.isValid = true;
            }
            else if (digitalRead(BTN_FORWARD_PIN) == LOW)
            {
                action_info.active = ActionType::BTN_FORWARD;
                action_info.isValid = true;
            }
            else if (digitalRead(BTN_LEFT_PIN) == LOW)
            {
                action_info.active = ActionType::BTN_LEFT;
                action_info.isValid = true;
            }
            else if (digitalRead(BTN_RIGHT_PIN) == LOW)
            {
                action_info.active = ActionType::BTN_RIGHT;
                action_info.isValid = true;
            }
            else
            {
                action_info.isValid = false;
            }
        }
        last_update_time = GET_SYS_MILLIS();
    }
    return &action_info;
}

Action *Buttons::getAction(void)
{
    if (!action_info.isValid)
    {
        if (digitalRead(BTN_BACK_PIN) == LOW)
        {
            action_info.active = ActionType::BTN_BACK;
            action_info.isValid = true;
        }
        else if (digitalRead(BTN_FORWARD_PIN) == LOW)
        {
            action_info.active = ActionType::BTN_FORWARD;
            action_info.isValid = true;
        }
        else if (digitalRead(BTN_LEFT_PIN) == LOW)
        {
            action_info.active = ActionType::BTN_LEFT;
            action_info.isValid = true;
        }
        else if (digitalRead(BTN_RIGHT_PIN) == LOW)
        {
            action_info.active = ActionType::BTN_RIGHT;
            action_info.isValid = true;
        }
        else
        {
            action_info.isValid = false;
        }
    }
    last_update_time = GET_SYS_MILLIS();
    return &action_info;
}