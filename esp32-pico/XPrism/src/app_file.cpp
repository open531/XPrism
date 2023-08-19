#include "app_file.h"
#include "app_file_ui.h"
#include "icons.h"
#include "app_center.h"

#define APP_FILE_NAME "文件"
#define APP_FILE_INFO "文件信息"

#define FILE_CONFIG_PATH "/file.cfg"

#define ROOT "/" // 根目录

static File_Info *file_info=NULL;


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
    String path;
};

static FileCfg fileCfg;
static FileAppRunData *fileAppRunData = NULL;

static int fileInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appFileUiInit();
    fileAppRunData = (FileAppRunData *)malloc(sizeof(FileAppRunData));
    readfileCfg(&fileCfg);
    file_ui_type uiType = file_ui_explorer;
    file_info= m_tf.listDir(ROOT);
    fileAppRunData->path=ROOT;
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
    

    

    const char* firstfile_name="..";
    appFileUiDisplayExplorerInit(firstfile_name,file_info->file_name,file_info->next_node->file_name,LV_SCR_LOAD_ANIM_NONE);

    if(action->active==BTN_LEFT)//向上滚动
    {
        appFileUiDisplayExplorer(path,firstfile_name,file_info->file_name,file_info->next_node->file_name,LV_SCR_LOAD_ANIM_MOVE_TOP,true);
    }

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