#include "imu.h"
#include "common.h"

const char *imuActionTypeInfo[] = {"IMU_NONE",
                                   "IMU_TURN_LEFT",
                                   "IMU_TURN_RIGHT",
                                   "IMU_SHRUG_LEFT",
                                   "IMU_SHRUG_RIGHT",
                                   "IMU_UP",
                                   "IMU_DOWN",
                                   "IMU_SHAKE"};

IMU::IMU()
{
    imuActionInfo.isValid = false;
    imuActionInfo.imuAction = IMU_NONE;
    imuActionInfo.isLong = true;
    // 初始化数据
    for (int pos = 0; pos < IMU_ACTION_HISTORY_BUF_LEN; ++pos)
    {
        imuActionInfoHistory[pos] = IMU_NONE;
    }
    imuActionInfoHistoryInd = IMU_ACTION_HISTORY_BUF_LEN - 1;
    this->order = 0;
}

void IMU::init(uint8_t order, uint8_t autoCalibration, IMUCfg *cfg)
{
    this->setOrder(order);
    Wire.begin(MPU_SDA, MPU_SCL);
    Wire.setClock(400000);
    unsigned long timeout = 5000;
    unsigned long preMillis = GET_SYS_MILLIS();
    mpu = MPU6050(0x68);
    while (!mpu.testConnection() && !doDelayMillisTime(timeout, &preMillis, false))
        ;

    if (!mpu.testConnection())
    {
        Serial.print(F("Unable to connect to MPU6050.\n"));
        return;
    }

    Serial.print(F("Initializing MPU6050, Please don't move.\n"));
    mpu.initialize();

    if (autoCalibration == 0)
    {
        // supply your own gyro offsets here, scaled for min sensitivity
        mpu.setXGyroOffset(cfg->xGyroOffset);
        mpu.setYGyroOffset(cfg->yGyroOffset);
        mpu.setZGyroOffset(cfg->zGyroOffset);
        mpu.setXAccelOffset(cfg->xAccelOffset);
        mpu.setYAccelOffset(cfg->yAccelOffset);
        mpu.setZAccelOffset(cfg->zAccelOffset);
    }
    else
    {
        // 启动自动校准
        // 7次循环自动校正
        mpu.CalibrateAccel(7);
        mpu.CalibrateGyro(7);
        mpu.PrintActiveOffsets();

        cfg->xGyroOffset = mpu.getXGyroOffset();
        cfg->yGyroOffset = mpu.getYGyroOffset();
        cfg->zGyroOffset = mpu.getZGyroOffset();
        cfg->xAccelOffset = mpu.getXAccelOffset();
        cfg->yAccelOffset = mpu.getYAccelOffset();
        cfg->zAccelOffset = mpu.getZAccelOffset();
    }

    Serial.print(F("Initialize MPU6050 success.\n"));
}

IMUAction *IMU::update(int interval)
{
    getVirtualMotion(&imuActionInfo);
    imuActionInfo.imuAction = IMU_NONE;
    if (GET_SYS_MILLIS() - lastUpdate > interval)
    {
        if (!imuActionInfo.isValid)
        {
            if (imuActionInfo.vGz > 14000)
            {
                imuActionInfo.imuAction = IMU_TURN_LEFT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vGz < -14000 )
            {
                imuActionInfo.imuAction = IMU_TURN_RIGHT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vGy > 4000)
            {
                imuActionInfo.imuAction = IMU_SHRUG_RIGHT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vGy < -4000)
            {
                imuActionInfo.imuAction = IMU_SHRUG_LEFT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vGx > 4000)
            {
                imuActionInfo.imuAction = IMU_UP;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vGx < -4000)
            {
                imuActionInfo.imuAction = IMU_DOWN;
                imuActionInfo.isValid = true;
            }
            else
            {
                imuActionInfo.isValid = false;
            }
        }
        else
        {
            if (imuActionInfo.vAx > -1000 && imuActionInfo.vAx < 1000 &&
                imuActionInfo.vAy > -4000 && imuActionInfo.vAy < 1000 &&
                imuActionInfo.vAz > 15000)
            {
                imuActionInfo.imuAction = IMU_NONE;
                imuActionInfo.isValid = false;
            }
        }

        if (imuActionInfo.imuAction != IMU_NONE)
        {
            Serial.printf("imuAction:%s\n", imuActionTypeInfo[imuActionInfo.imuAction]);
        }

        // Serial.printf("vAx:%d vAy:%d vAz:%d vGx:%d vGy:%d vGz:%d\n",
        //               imuActionInfo.vAx,
        //               imuActionInfo.vAy,
        //               imuActionInfo.vAz,
        //               imuActionInfo.vGx,
        //               imuActionInfo.vGy,
        //               imuActionInfo.vGz);

        lastUpdate = GET_SYS_MILLIS();
    }
    return &imuActionInfo;
    // IMUAction tmp;
    // if (GET_SYS_MILLIS() - lastUpdate > interval)
    // {
    //     getVirtualMotion(&tmp);

    //     Serial.printf("vAx:%d vAy:%d vAz:%d vGx:%d vGy:%d vGz:%d\n",
    //                   tmp.vAx,
    //                   tmp.vAy,
    //                   tmp.vAz,
    //                   tmp.vGx,
    //                   tmp.vGy,
    //                   tmp.vGz);

    //     lastUpdate = GET_SYS_MILLIS();
    // }
    // return &tmp;
}

void IMU::setOrder(uint8_t order)
{
    this->order = order;
}

void IMU::getVirtualMotion(IMUAction *action)
{
    mpu.getMotion6(&(action->vAx), &(action->vAy),
                   &(action->vAz), &(action->vGx),
                   &(action->vGy), &(action->vGz));
}