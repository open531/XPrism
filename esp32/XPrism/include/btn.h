#ifndef BTN_H
#define BTN_H

#include "lv_port_indev.h"
#include <Arduino.h>
#include <list>
#define ACTION_HISTORY_BUF_LEN 5

#define BTN_BACK_PIN 25
#define BTN_FORWARD_PIN 32
#define BTN_LEFT_PIN 33
#define BTN_RIGHT_PIN 27

extern const char *action_type_info[];

enum ActionType
{
    BTN_NONE,
    BTN_BACK,
    BTN_FORWARD,
    BTN_LEFT,
    BTN_RIGHT
};

struct Action
{
    volatile ActionType active;
    boolean isValid;
};

class Buttons
{
private:
    int flag;
    long last_update_time;

public:
    Action action_info;
    // 用来储存历史动作
    ActionType act_info_history[ACTION_HISTORY_BUF_LEN];
    int act_info_history_ind; // 标志储存的位置

public:
    Buttons();
    void init();
    Action *update(int interval);
    Action *getAction(void); // 获取动作
};

#endif
