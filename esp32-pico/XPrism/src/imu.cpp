#include "imu.h"
#include "common.h"

const char *imu_action_type_info[] = {"IMU_NONE",
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
    imuActionInfo.imuAction = IMUActionType::IMU_NONE;
    imuActionInfo.isLong = true;
    // 初始化数据
    for (int pos = 0; pos < IMU_ACTION_HISTORY_BUF_LEN; ++pos)
    {
        imuActionInfoHistory[pos] = IMUActionType::IMU_NONE;
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

    Serial.print(F("Initialization MPU6050 now, Please don't move.\n"));
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

    Serial.print(F("Initialization MPU6050 success.\n"));
}

IMUAction *IMU::update(int interval)
{
    getVirtualMotion(&imuActionInfo);
    if (GET_SYS_MILLIS() - lastUpdate > interval)
    {
        if (!imuActionInfo.isValid)
        {
            if (imuActionInfo.vAy > 4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_TURN_LEFT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAy < -4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_TURN_RIGHT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAx > 4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_SHRUG_LEFT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAx < -4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_SHRUG_RIGHT;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAz > 4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_UP;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAz < -4000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_DOWN;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAx > 1000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_SHAKE;
                imuActionInfo.isValid = true;
            }
            else if (imuActionInfo.vAx < -1000)
            {
                imuActionInfo.imuAction = IMUActionType::IMU_SHAKE;
                imuActionInfo.isValid = true;
            }
            else
            {
                imuActionInfo.isValid = false;
            }
        }
        lastUpdate = GET_SYS_MILLIS();
    }
    return &imuActionInfo;
}

void IMU::setOrder(uint8_t order)
{
    this->order = order;
}

void IMU::getVirtualMotion(IMUAction *action)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    action->vAx = ax;
    action->vAy = ay;
    action->vAz = az;
    action->vGx = gx;
    action->vGy = gy;
    action->vGz = gz;
}