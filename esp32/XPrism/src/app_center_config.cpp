#include "app_center.h"
#include "common.h"
#include "app.h"
#include "Arduino.h"

#define APP_CTRL_CONFIG_PATH "/sys.cfg"
#define MPU_CONFIG_PATH "/mpu.cfg"
#define RGB_CONFIG_PATH "/rgb.cfg"

void AppCenter::read_config(SysUtilConfig *cfg)
{
    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    uint16_t size = m_flashCfg.readFile(APP_CTRL_CONFIG_PATH, (uint8_t *)info);
    info[size] = 0;
    if (size == 0)
    {
        // 默认值
        cfg->power_mode = 0;           // 功耗模式（0为节能模式 1为性能模式）
        cfg->backLight = 100;          // 屏幕亮度（1-100）
        cfg->rotation = 0;             // 屏幕旋转方向
        cfg->auto_calibration_mpu = 1; // 是否自动校准陀螺仪 0关闭自动校准 1打开自动校准
        cfg->mpu_order = 0;            // 操作方向
        cfg->auto_start_app = "天气";  // 无指定开机自启APP
        this->write_config(cfg);
    }
    else
    {
        // 解析数据
        char *param[12] = {0};
        analyseParam(info, 12, param);
        cfg->ssid_0 = param[0];
        cfg->password_0 = param[1];
        cfg->ssid_1 = param[2];
        cfg->password_1 = param[3];
        cfg->ssid_2 = param[4];
        cfg->password_2 = param[5];
        cfg->power_mode = atol(param[6]);
        cfg->backLight = atol(param[7]);
        cfg->rotation = atol(param[8]);
        cfg->auto_calibration_mpu = atol(param[9]);
        cfg->mpu_order = atol(param[10]);
        cfg->auto_start_app = param[11]; // 开机自启APP的name
    }
}

void AppCenter::write_config(SysUtilConfig *cfg)
{
    char tmp[25];
    // 将配置数据保存在文件中（持久化）
    String w_data;
    w_data = w_data + cfg->ssid_0 + "\n";
    w_data = w_data + cfg->password_0 + "\n";
    w_data = w_data + cfg->ssid_1 + "\n";
    w_data = w_data + cfg->password_1 + "\n";
    w_data = w_data + cfg->ssid_2 + "\n";
    w_data = w_data + cfg->password_2 + "\n";
    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->power_mode);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->backLight);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->rotation);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->auto_calibration_mpu);
    w_data += tmp;

    memset(tmp, 0, 25);
    snprintf(tmp, 25, "%u\n", cfg->mpu_order);
    w_data += tmp;

    w_data = w_data + cfg->auto_start_app + "\n";

    m_flashCfg.writeFile(APP_CTRL_CONFIG_PATH, w_data.c_str());

    // 立即生效相关配置
    m_screen.setBackLight(cfg->backLight / 100.0);
    m_tft->setRotation(cfg->rotation);
}

void AppCenter::deal_config(AppMsgType type,
                            const char *key, char *value)
{
    switch (type)
    {

    case APP_MESSAGE_GET_PARAM:
    {
        if (!strcmp(key, "ssid_0"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_0.c_str());
        }
        else if (!strcmp(key, "password_0"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_0.c_str());
        }
        else if (!strcmp(key, "ssid_1"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_1.c_str());
        }
        else if (!strcmp(key, "password_1"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_1.c_str());
        }
        if (!strcmp(key, "ssid_2"))
        {
            snprintf(value, 32, "%s", sys_cfg.ssid_2.c_str());
        }
        else if (!strcmp(key, "password_2"))
        {
            snprintf(value, 32, "%s", sys_cfg.password_2.c_str());
        }
        else if (!strcmp(key, "power_mode"))
        {
            snprintf(value, 32, "%u", sys_cfg.power_mode);
        }
        else if (!strcmp(key, "backLight"))
        {
            snprintf(value, 32, "%u", sys_cfg.backLight);
        }
        else if (!strcmp(key, "rotation"))
        {
            snprintf(value, 32, "%u", sys_cfg.rotation);
        }
        else if (!strcmp(key, "auto_calibration_mpu"))
        {
            snprintf(value, 32, "%u", sys_cfg.auto_calibration_mpu);
        }
        else if (!strcmp(key, "mpu_order"))
        {
            snprintf(value, 32, "%u", sys_cfg.mpu_order);
        }
        else if (!strcmp(key, "auto_start_app"))
        {
            snprintf(value, 32, "%s", sys_cfg.auto_start_app.c_str());
        }
    }
    break;
    case APP_MESSAGE_SET_PARAM:
    {
        if (!strcmp(key, "ssid_0"))
        {
            sys_cfg.ssid_0 = value;
        }
        else if (!strcmp(key, "password_0"))
        {
            sys_cfg.password_0 = value;
        }
        else if (!strcmp(key, "ssid_1"))
        {
            sys_cfg.ssid_1 = value;
        }
        else if (!strcmp(key, "password_1"))
        {
            sys_cfg.password_1 = value;
        }
        else if (!strcmp(key, "ssid_2"))
        {
            sys_cfg.ssid_2 = value;
        }
        else if (!strcmp(key, "password_2"))
        {
            sys_cfg.password_2 = value;
        }
        else if (!strcmp(key, "power_mode"))
        {
            sys_cfg.power_mode = atol(value);
        }
        else if (!strcmp(key, "backLight"))
        {
            sys_cfg.backLight = atol(value);
        }
        else if (!strcmp(key, "rotation"))
        {
            sys_cfg.rotation = atol(value);
        }
    }
    break;
    case APP_MESSAGE_READ_CFG:
    {
        read_config(&sys_cfg);
        // read_config(&mpu_cfg);
    }
    break;
    case APP_MESSAGE_WRITE_CFG:
    {
        write_config(&sys_cfg);
        // write_config(&mpu_cfg);  // 在取消自动校准的时候已经写过一次了
    }
    break;
    default:
        break;
    }
}