#include "app_file.h"
#include "app_file_ui.h"
#include "icons.h"
#include "app_center.h"

#define APP_FILE_NAME "文件"
#define APP_FILE_INFO "文件信息"

#define FILE_CONFIG_PATH "/file.cfg"

#define ROOT "/" // 根目录

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
    FileUiType uiType;

    String currPath;
    File_Info *fileInfo;
    File_Info *currFile;

    int temp;
};

static FileCfg fileCfg;
static FileAppRunData *fileAppRunData = NULL;

static void freeFileInfo()
{
    File_Info *temp = fileAppRunData->fileInfo->next_node;
    if (temp != NULL)
    {
        do
        {
            File_Info *next = temp->next_node;
            free(temp);
            Serial.print("freeFileInfo: ");
            Serial.println(temp->file_name);
            temp = next;
        } while (temp != fileAppRunData->fileInfo->next_node);
    }
    free(fileAppRunData->fileInfo);
    Serial.print("freeFileInfo: ");
    Serial.println(fileAppRunData->fileInfo->file_name);
    fileAppRunData->fileInfo = NULL;
}

static void openDir(String dirName)
{
    if (fileAppRunData->currPath != ROOT)
    {
        fileAppRunData->currPath += "/" + dirName;
    }
    else
    {
        fileAppRunData->currPath += dirName;
    }
    freeFileInfo();
    fileAppRunData->fileInfo = m_tf.listDir(fileAppRunData->currPath.c_str());
    fileAppRunData->currFile = fileAppRunData->fileInfo->next_node;
    if (fileAppRunData->currFile == NULL)
    {
        fileAppRunData->currFile = new File_Info;
        fileAppRunData->currFile->file_name = "空文件夹";
        fileAppRunData->currFile->file_type = FILE_TYPE_UNKNOWN;
        fileAppRunData->currFile->next_node = fileAppRunData->currFile;
        fileAppRunData->currFile->front_node = fileAppRunData->currFile;
    }
}

static void upDir()
{
    int index = fileAppRunData->currPath.lastIndexOf("/");
    if (index != -1 && index != 0)
    {
        fileAppRunData->currPath = fileAppRunData->currPath.substring(0, index);
    }
    else
    {
        fileAppRunData->currPath = ROOT;
    }
    freeFileInfo();
    fileAppRunData->fileInfo = m_tf.listDir(fileAppRunData->currPath.c_str());
    fileAppRunData->currFile = fileAppRunData->fileInfo->next_node;
}

static void openFile()
{
    if (fileAppRunData->currFile->file_type == FILE_TYPE_FOLDER)
    {
        openDir(fileAppRunData->currFile->file_name);
    }
    else
    {
        String fileName = fileAppRunData->currPath + "/" + fileAppRunData->currFile->file_name;
        int index = fileName.lastIndexOf(".");
        String suffix = fileName.substring(index + 1);
        if (suffix == "jpg" || suffix == "jpeg" || suffix == "bin")
        {
        }
        else if (suffix == "mjpeg")
        {
        }
        else if (suffix == "txt")
        {
        }
        else
        {
        }
    }
}

static int fileInit(AppCenter *appCenter)
{
    m_tft->setSwapBytes(true);
    appFileUiInit();
    fileAppRunData = (FileAppRunData *)malloc(sizeof(FileAppRunData));
    readfileCfg(&fileCfg);
    fileAppRunData->uiType = FILE_UI_TYPE_EXPLORER;
    fileAppRunData->currPath = ROOT;
    fileAppRunData->fileInfo = m_tf.listDir(fileAppRunData->currPath.c_str());
    fileAppRunData->currFile = fileAppRunData->fileInfo->next_node;
    fileAppRunData->temp = 0;
    appFileUiDisplayExplorer(fileAppRunData->currPath.c_str(),
                             fileAppRunData->currFile->file_name,
                             fileAppRunData->currFile->next_node->file_name,
                             fileAppRunData->currFile->next_node->next_node->file_name,
                             fileAppRunData->temp, LV_SCR_LOAD_ANIM_NONE, true);
    return 0;
}

static void fileRoutine(AppCenter *appCenter, const Action *action)
{
    lv_scr_load_anim_t animType = LV_SCR_LOAD_ANIM_NONE;
    bool force = false;

    if (fileAppRunData->uiType == FILE_UI_TYPE_EXPLORER)
    {
        if (action->active == BTN_BACK)
        {
            if (fileAppRunData->currPath == ROOT)
            {
                appCenter->app_exit();
                return;
            }
            else
            {
                upDir();
                force = true;
            }
        }
        else if (action->active == BTN_FORWARD)
        {
            if (fileAppRunData->currFile->file_type == FILE_TYPE_FOLDER)
            {
                openDir(fileAppRunData->currFile->file_name);
                force = true;
            }
        }
        else if (action->active == BTN_LEFT)
        {
            animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
            fileAppRunData->currFile = fileAppRunData->currFile->front_node;
            fileAppRunData->temp--;
        }
        else if (action->active == BTN_RIGHT)
        {
            animType = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
            fileAppRunData->currFile = fileAppRunData->currFile->next_node;
            fileAppRunData->temp++;
        }

        appFileUiDisplayExplorer(fileAppRunData->currPath.c_str(),
                                 fileAppRunData->currFile->file_name,
                                 fileAppRunData->currFile->next_node->file_name,
                                 fileAppRunData->currFile->next_node->next_node->file_name,
                                 fileAppRunData->temp, animType, force);
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