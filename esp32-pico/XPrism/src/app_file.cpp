#include "app_file.h"
#include "app_file_ui.h"
#include "icons.h"
#include "app_center.h"

#define APP_FILE_NAME "文件"
#define APP_FILE_INFO "文件信息"

#define FILE_CONFIG_PATH "/file.cfg"

struct FileCfg
{
};

static void writefileCfg(FileCfg *cfg)
{
}

static void readfileCfg(FileCfg *cfg)
{
}

struct FileAppRunData
{
};

static FileCfg fileCfg;
static FileAppRunData *fileAppRunData = NULL;

static int fileInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appFileUiInit();
    fileAppRunData = (FileAppRunData *)malloc(sizeof(FileAppRunData));
    readfileCfg(&fileCfg);
    return 0;
}

static void fileRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    if (action->active == BTN_BACK)
    {
        appCenter->app_exit();
        return;
    }

    // appFileUiDisplayExplorer(animType);
    delay(100);
}

static void fileBackground(AppCenter *appCenter, const Action *action)
{
}

static int fileExit(void *param)
{
    appFileUiDelete();

    if (fileAppRunData != NULL)
    {
        free(fileAppRunData);
        fileAppRunData = NULL;
    }

    return 0;
}

static void fileOnMessage(const char *from, const char *to,
                          AppMsgType type, void *msg, void *info)
{
}

App fileApp = {
    APP_FILE_NAME,
    &icon_file,
    APP_FILE_INFO,
    fileInit,
    fileRoutine,
    fileBackground,
    fileExit,
    fileOnMessage,
};