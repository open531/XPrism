#ifndef IMU_H
#define IMU_H

#include <I2Cdev.h>
#include <MPU6050.h>

#define IMU_ACTION_HISTORY_BUF_LEN 5

extern const char *imuActionTypeInfo[];

enum IMUActionType
{
    IMU_NONE,
    IMU_TURN_LEFT,
    IMU_TURN_RIGHT,
    IMU_SHRUG_LEFT,
    IMU_SHRUG_RIGHT,
    IMU_UP,
    IMU_DOWN,
    IMU_SHAKE
};

struct IMUCfg
{
    int16_t xGyroOffset;
    int16_t yGyroOffset;
    int16_t zGyroOffset;

    int16_t xAccelOffset;
    int16_t yAccelOffset;
    int16_t zAccelOffset;
};

struct IMUAction
{
    volatile IMUActionType imuAction;
    boolean isValid;
    boolean isLong;
    int16_t vAx; // v表示虚拟参数（用于调整6050的初始方位）
    int16_t vAy;
    int16_t vAz;
    int16_t vGx;
    int16_t vGy;
    int16_t vGz;
};

class IMU
{
private:
    MPU6050 mpu;
    int flag;
    long lastUpdate;
    uint8_t order;

public:
    IMUAction imuActionInfo;
    IMUActionType imuActionInfoHistory[IMU_ACTION_HISTORY_BUF_LEN];
    int imuActionInfoHistoryInd;

public:
    IMU();
    void init(uint8_t order, uint8_t autoCalibration, IMUCfg *cfg);
    IMUAction *update(int interval);
    // IMUAction *getAction(void);
    void setOrder(uint8_t order);
    void getVirtualMotion(IMUAction *action);
};

#endif