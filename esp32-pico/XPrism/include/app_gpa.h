#ifndef APP_GPA_H
#define APP_GPA_H

#include "app.h"

extern App gpaApp;

#include <Arduino.h>
#include <iostream>

#define SCALE_SIZE 4
#define WIN_SCORE 2048

class GameGpa
{
private:
    int board[4][4];
    int previous[4][4];
    std::string Location[4][4];
    int moveRecord[4][4];

public:
    void init();
    int addRandom(void);
    void moveUp(void);
    void moveDown(void);
    void moveLeft(void);
    void moveRight(void);
    int judge(void);
    void recordLocation(int direction);
    void countMoveRecord(int direction);
    void recordBoard();
    int comparePre();

    int *getBoard();
    std::string *getLocation();
    int *getMoveRecord();
};

#endif