#include "app_gpa.h"
#include "app_gpa_ui.h"
#include "icons.h"
#include "app_center.h"

#define APP_GPA_NAME "GPA"
#define APP_GPA_INFO "清华大学 GPA 版 2048 小游戏"

void taskTwo(void *parameter)
{
    while (1)
    {
        // LVGL任务主函数
        LVGL_OPERATE_LOCK(lv_task_handler();)
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    Serial.println("Ending lv_task_handler");
    vTaskDelete(NULL);
}

GameGpa game;

void GameGpa::init()
{
    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        this->board[i / 4][i % 4] = 0;
        this->previous[i / 4][i % 4] = 0;
        this->moveRecord[i / 4][i % 4] = 0;
    }
}

int GameGpa::addRandom(void)
{
    int randPos, randNum;
    while (1)
    {
        randPos = random(3200) % 16;
        randNum = random(114514) % 10;
        if (this->board[randPos / 4][randPos % 4] == 0)
        {
            if (randNum < 5)
            {
                this->board[randPos / 4][randPos % 4] = 2;
            }
            else if (randNum >= 5 && randNum < 7)
            {
                this->board[randPos / 4][randPos % 4] = 4;
            }
            else if (randNum == 7)
            {
                this->board[randPos / 4][randPos % 4] = -1;
            }
            else if (randNum == 8)
            {
                this->board[randPos / 4][randPos % 4] = -2;
            }
            else if (randNum == 9)
            {
                this->board[randPos / 4][randPos % 4] = -3;
            }

            break;
        }
    }
    return randPos;
}

void GameGpa::moveUp(void)
{
    recordLocation(1); // 记录位置
    recordBoard();     // 记录数值
    // 移动两次
    for (int x = 0; x < 2; x++)
    {
        for (int i = 0; i < SCALE_SIZE - 1; i++)
        {
            for (int j = 0; j < SCALE_SIZE; j++)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = board[i + 1][j];
                    board[i + 1][j] = 0;
                    // 动画轨迹记录
                    Location[i][j] = Location[i + 1][j];
                    Location[i + 1][j] = "";
                }
            }
        }
    }

    // 相加
    for (int i = 0; i < SCALE_SIZE - 1; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            if (board[i][j] == board[i + 1][j] && board[i][j] > 0)
            {
                board[i][j] *= 2;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] == -1 && board[i + 1][j] > 0)
            {
                board[i][j] = board[i + 1][j] * 2;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] > 0 && board[i + 1][j] == -1)
            {
                board[i][j] *= 2;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] == -2 && board[i + 1][j] > 2)
            {
                board[i][j] = board[i + 1][j] / 2;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] > 2 && board[i + 1][j] == -2)
            {
                board[i][j] /= 2;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] == -3 && board[i + 1][j] > 0)
            {
                board[i][j] = 0;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
            if (board[i][j] > 0 && board[i + 1][j] == -3)
            {
                board[i][j] = 0;
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j].append(Location[i + 1][j]);
                Location[i + 1][j] = "";
            }
        }
    }

    // 移动
    for (int i = 0; i < SCALE_SIZE - 1; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            if (board[i][j] == 0)
            {
                board[i][j] = board[i + 1][j];
                board[i + 1][j] = 0;
                // 动画轨迹记录
                Location[i][j] = Location[i + 1][j];
                Location[i + 1][j] = "";
            }
        }
    }
    countMoveRecord(1);
}

void GameGpa::moveDown(void)
{
    recordLocation(2); // 记录位置
    recordBoard();
    // 移动
    for (int x = 0; x < 2; x++)
    {
        for (int i = SCALE_SIZE - 1; i > 0; i--)
        {
            for (int j = SCALE_SIZE - 1; j >= 0; j--)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = board[i - 1][j];
                    board[i - 1][j] = 0;
                    // 动画移动轨迹记录
                    Location[i][j] = Location[i - 1][j];
                    Location[i - 1][j] = "";
                }
            }
        }
    }

    // 相加
    for (int i = SCALE_SIZE - 1; i > 0; i--)
    {
        for (int j = SCALE_SIZE - 1; j >= 0; j--)
        {
            if (board[i][j] == board[i - 1][j] && board[i][j] > 0)
            {
                board[i][j] *= 2;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] == -1 && board[i - 1][j] > 0)
            {
                board[i][j] = board[i - 1][j] * 2;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] > 0 && board[i - 1][j] == -1)
            {
                board[i][j] *= 2;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] == -2 && board[i - 1][j] > 2)
            {
                board[i][j] = board[i - 1][j] / 2;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] > 2 && board[i - 1][j] == -2)
            {
                board[i][j] /= 2;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] == -3 && board[i - 1][j] > 0)
            {
                board[i][j] = 0;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
            if (board[i][j] > 0 && board[i - 1][j] == -3)
            {
                board[i][j] = 0;
                board[i - 1][j] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i - 1][j]);
                Location[i - 1][j] = "";
            }
        }
    }

    // 移动
    for (int i = SCALE_SIZE - 1; i > 0; i--)
    {
        for (int j = SCALE_SIZE - 1; j >= 0; j--)
        {
            if (board[i][j] == 0)
            {
                board[i][j] = board[i - 1][j];
                board[i - 1][j] = 0;
                // 动画移动轨迹记录
                Location[i][j] = Location[i - 1][j];
                Location[i - 1][j] = "";
            }
        }
    }
    countMoveRecord(2);
}

void GameGpa::moveLeft(void)
{
    recordLocation(3); // 记录位置
    recordBoard();
    // 移动
    for (int x = 0; x < 2; x++)
    {
        for (int i = 0; i < SCALE_SIZE; i++)
        {
            for (int j = 0; j < SCALE_SIZE - 1; j++)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = board[i][j + 1];
                    board[i][j + 1] = 0;
                    // 动画移动轨迹记录
                    Location[i][j] = Location[i][j + 1];
                    Location[i][j + 1] = "";
                }
            }
        }
    }
    // 相加
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE - 1; j++)
        {
            if (board[i][j] == board[i][j + 1] && board[i][j] > 0)
            {
                board[i][j] *= 2;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] == -1 && board[i][j + 1] > 0)
            {
                board[i][j] = board[i][j + 1] * 2;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] > 0 && board[i][j + 1] == -1)
            {
                board[i][j] *= 2;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] == -2 && board[i][j + 1] > 2)
            {
                board[i][j] = board[i][j + 1] / 2;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] > 2 && board[i][j + 1] == -2)
            {
                board[i][j] /= 2;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] == -3 && board[i][j + 1] > 0)
            {
                board[i][j] = 0;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
            if (board[i][j] > 0 && board[i][j + 1] == -3)
            {
                board[i][j] = 0;
                board[i][j + 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j + 1]);
                Location[i][j + 1] = "";
            }
        }
    }
    // 移动
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE - 1; j++)
        {
            if (board[i][j] == 0)
            {
                board[i][j] = board[i][j + 1];
                board[i][j + 1] = 0;
                // 动画移动轨迹记录
                Location[i][j] = Location[i][j + 1];
                Location[i][j + 1] = "";
            }
        }
    }
    countMoveRecord(3);
}
void GameGpa::moveRight(void)
{
    recordLocation(4); // 记录位置
    recordBoard();
    // 移动两次
    for (int x = 0; x < 2; x++)
    {
        for (int i = SCALE_SIZE - 1; i >= 0; i--)
        {
            for (int j = SCALE_SIZE - 1; j > 0; j--)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = board[i][j - 1];
                    board[i][j - 1] = 0;
                    // 动画移动轨迹记录
                    Location[i][j] = Location[i][j - 1];
                    Location[i][j - 1] = "";
                }
            }
        }
    }

    // 相加
    for (int i = SCALE_SIZE - 1; i >= 0; i--)
    {
        for (int j = SCALE_SIZE - 1; j > 0; j--)
        {
            if (board[i][j] == board[i][j - 1] && board[i][j] > 0)
            {
                board[i][j] *= 2;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] == -1 && board[i][j - 1] > 0)
            {
                board[i][j] = board[i][j - 1] * 2;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] > 0 && board[i][j - 1] == -1)
            {
                board[i][j] *= 2;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] == -2 && board[i][j - 1] > 2)
            {
                board[i][j] = board[i][j - 1] / 2;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] > 2 && board[i][j - 1] == -2)
            {
                board[i][j] /= 2;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] == -3 && board[i][j - 1] > 0)
            {
                board[i][j] = 0;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
            if (board[i][j] > 0 && board[i][j - 1] == -3)
            {
                board[i][j] = 0;
                board[i][j - 1] = 0;
                // 动画合并轨迹记录
                Location[i][j].append(Location[i][j - 1]);
                Location[i][j - 1] = "";
            }
        }
    }

    // 移动
    for (int i = SCALE_SIZE - 1; i >= 0; i--)
    {
        for (int j = SCALE_SIZE - 1; j > 0; j--)
        {
            if (board[i][j] == 0)
            {
                board[i][j] = board[i][j - 1];
                board[i][j - 1] = 0;
                // 动画移动轨迹记录
                Location[i][j] = Location[i][j - 1];
                Location[i][j - 1] = "";
            }
        }
    }
    countMoveRecord(4);
}

int GameGpa::judge(void)
{
    // 判赢
    for (int i = 0; i <= SCALE_SIZE * SCALE_SIZE; i++)
    {
        if (board[i / 4][i % 4] >= WIN_SCORE)
        {
            return 1; // Win
        }
    }
    // 判空
    for (int i = 0; i <= SCALE_SIZE * SCALE_SIZE; i++)
    {
        if (board[i / 4][i % 4] == 0)
        {
            return 0;
        }
    }

    // 判相邻相同
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            if (i < 3)
            {
                if (board[i][j] == board[i + 1][j])
                {
                    return 0;
                }
            }
            if (j < 3)
            {
                if (board[i][j] == board[i][j + 1])
                {
                    return 0;
                }
            }
        }
    }

    return 2; // Defeatd
}

void GameGpa::recordLocation(int direction)
{
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            // 无数字则为空
            if (board[i][j] == 0)
            {
                Location[i][j] = "";
            }
            else
            {
                // 有数字根据方向填入ABCD
                switch (direction)
                {
                case 1: // 上
                case 2: // 下
                    switch (i)
                    {
                    case 0:
                        Location[i][j] = "A";
                        break;
                    case 1:
                        Location[i][j] = "B";
                        break;
                    case 2:
                        Location[i][j] = "C";
                        break;
                    case 3:
                        Location[i][j] = "D";
                        break;
                    }
                    break;
                case 3: // 左
                case 4: // 右
                    switch (j)
                    {
                    case 0:
                        Location[i][j] = "A";
                        break;
                    case 1:
                        Location[i][j] = "B";
                        break;
                    case 2:
                        Location[i][j] = "C";
                        break;
                    case 3:
                        Location[i][j] = "D";
                        break;
                    }
                    break;
                }
            }
        }
    }
}

void GameGpa::countMoveRecord(int direction)
{

    // 清空
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            moveRecord[i][j] = 0;
        }
    }
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        for (int j = 0; j < SCALE_SIZE; j++)
        {
            switch (direction)
            {
            case 1:
            case 2:
                // 移动检测
                if (Location[i][j].find("A") != -1)
                {
                    moveRecord[0][j] += i;
                }
                if (Location[i][j].find("B") != -1)
                {
                    moveRecord[1][j] += i - 1;
                }
                if (Location[i][j].find("C") != -1)
                {
                    moveRecord[2][j] += i - 2;
                }
                if (Location[i][j].find("D") != -1)
                {
                    moveRecord[3][j] += i - 3;
                }
                break;
            case 3:
            case 4:
                // 移动检测
                if (Location[i][j].find("A") != -1)
                {
                    moveRecord[i][0] += j;
                }
                if (Location[i][j].find("B") != -1)
                {
                    moveRecord[i][1] += j - 1;
                }
                if (Location[i][j].find("C") != -1)
                {
                    moveRecord[i][2] += j - 2;
                }
                if (Location[i][j].find("D") != -1)
                {
                    moveRecord[i][3] += j - 3;
                }
                break;
            }
            // 合并检测
            if (Location[i][j].length() == 2)
            {
                moveRecord[i][j] += 8;
            }
        }
    }
}

void GameGpa::recordBoard()
{
    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        this->previous[i / 4][i % 4] = this->board[i / 4][i % 4];
    }
}

int GameGpa::comparePre()
{
    int x = 0;
    for (int i = 0; i < SCALE_SIZE * SCALE_SIZE; i++)
    {
        if (this->board[i / 4][i % 4] == this->previous[i / 4][i % 4])
            x++;
    }

    if (x >= 16)
    {
        return 1;
    }
    return 0;
}

int *GameGpa::getBoard()
{
    return &this->board[0][0];
}

std::string *GameGpa::getLocation()
{
    return &this->Location[0][0];
}

int *GameGpa::getMoveRecord()
{
    return &this->moveRecord[0][0];
}

struct GpaAppRunData
{
    int Normal = 0;       // 记录移动的方向
    int BornLocation = 0; // 记录新棋子的位置
    int *pBoard;
    int *moveRecord;

    BaseType_t xReturned_task_two = pdFALSE;
    TaskHandle_t xHandle_task_two = NULL;
};

static GpaAppRunData *gpaAppRunData = NULL;

static int gpaInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appGpaUiInit();

    randomSeed(m_timestamp);

    gpaAppRunData = (GpaAppRunData *)malloc(sizeof(GpaAppRunData));
    game.init();
    gpaAppRunData->pBoard = game.getBoard();
    gpaAppRunData->moveRecord = game.getMoveRecord();

    gpaAppRunData->xReturned_task_two = xTaskCreate(
        taskTwo,                           /*任务函数*/
        "TaskTwo",                         /*带任务名称的字符串*/
        8 * 1024,                          /*堆栈大小，单位为字节*/
        NULL,                              /*作为任务输入传递的参数*/
        1,                                 /*任务的优先级*/
        &gpaAppRunData->xHandle_task_two); /*任务句柄*/

    int new1 = game.addRandom();
    int new2 = game.addRandom();
    LVGL_OPERATE_LOCK(showBoard(gpaAppRunData->pBoard));
    LVGL_OPERATE_LOCK(born(new1));
    LVGL_OPERATE_LOCK(born(new2));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return 0;
}

static void gpaRoutine(AppCenter *appCenter, const Action *action)
{
    if (action->action == ACT_BACK)
    {
        appCenter->exitApp();
        return;
    }

    if (action->action == ACT_RIGHT)
    {
        game.moveRight();
        if (game.comparePre() == 0)
        {
            LVGL_OPERATE_LOCK(showAnim(gpaAppRunData->moveRecord, 4));
            delay(700);
            LVGL_OPERATE_LOCK(showNewBorn(game.addRandom(), gpaAppRunData->pBoard));
        }
    }
    else if (action->action == ACT_LEFT)
    {
        game.moveLeft();
        if (game.comparePre() == 0)
        {
            LVGL_OPERATE_LOCK(showAnim(gpaAppRunData->moveRecord, 3));
            delay(700);
            LVGL_OPERATE_LOCK(showNewBorn(game.addRandom(), gpaAppRunData->pBoard));
        }
    }
    else if (action->action == ACT_UP)
    {
        game.moveUp();
        if (game.comparePre() == 0)
        {
            LVGL_OPERATE_LOCK(showAnim(gpaAppRunData->moveRecord, 1));
            delay(700);
            LVGL_OPERATE_LOCK(showNewBorn(game.addRandom(), gpaAppRunData->pBoard));
        }
    }
    else if (action->action == ACT_DOWN)
    {
        game.moveDown();
        if (game.comparePre() == 0)
        {
            LVGL_OPERATE_LOCK(showAnim(gpaAppRunData->moveRecord, 2));
            delay(700);
            LVGL_OPERATE_LOCK(showNewBorn(game.addRandom(), gpaAppRunData->pBoard));
        }
    }

    if (game.judge() == 1)
    {
        Serial.println("You Win!");
    }
    else if (game.judge() == 2)
    {
        Serial.println("You Lose!");
    }

    delay(100);
}

static void gpaBackground(AppCenter *appCenter, const Action *action)
{
}

static int gpaExit(void *param)
{
    if (gpaAppRunData->xReturned_task_two == pdPASS)
    {
        vTaskDelete(gpaAppRunData->xHandle_task_two);
    }

    xSemaphoreGive(lvgl_mutex);

    appGpaUiDelete();

    if (gpaAppRunData != NULL)
    {
        free(gpaAppRunData);
        gpaAppRunData = NULL;
    }

    return 0;
}

static void gpaOnMessage(const char *from, const char *to,
                         AppMsgType type, void *msg, void *info)
{
}

App gpaApp = {
    APP_GPA_NAME,
    &icon_game,
    APP_GPA_INFO,
    gpaInit,
    gpaRoutine,
    gpaBackground,
    gpaExit,
    gpaOnMessage,
};