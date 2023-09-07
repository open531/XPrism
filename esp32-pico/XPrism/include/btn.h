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

extern const char *btnActionTypeInfo[];

enum BTNActionType
{
    BTN_NONE,
    BTN_BACK,
    BTN_FORWARD,
    BTN_LEFT,
    BTN_RIGHT
};

struct BTNAction
{
    volatile BTNActionType btnAction;
    boolean isValid;
};

class Buttons
{
private:
    int flag;
    long lastUpdate;

public:
    BTNAction btnActionInfo;
    // 用来储存历史动作
    BTNActionType btnActionInfoHistory[ACTION_HISTORY_BUF_LEN];
    int btnActionInfoHistoryInd; // 标志储存的位置

public:
    Buttons();
    void init();
    BTNAction *update(int interval);
    BTNAction *getAction(void); // 获取动作
};

#endif
