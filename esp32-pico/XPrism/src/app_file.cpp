#include "app_file.h"
#include "app_file_ui.h"
#include "icons.h"
#include "app_center.h"
#include <TJpg_Decoder.h>
#include "player.h"
#include "dmadrawer.h"

#define APP_FILE_NAME "文件"
#define APP_FILE_INFO "文件信息"

#define FILE_CONFIG_PATH "/file.cfg"

#define ROOT "/" // 根目录

#define TEXT_PAGE_SIZE 128 // 文本每页大小

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

    PlayDecoderBase *decoder;
    File currVideoFile;

    std::vector<int> textPageStartIndex;
    int textCurrPage;
    unsigned char *textBuffer;
    boolean textIsEnd;
};

static FileCfg fileCfg;
static FileAppRunData *fileAppRunData = NULL;

static boolean isFirstByte(unsigned char c)
{
    // 单字节字符的最高位为 0
    if ((c & 0x80) == 0)
        return 1;
    // 多字节字符的第一个字节的最高位为 1，并且有几个连续的 1 就表示该字符占用几个字节
    int count = 0;
    while (c & 0x80)
    {
        count++;
        c <<= 1;
    }
    return count >= 2 && count <= 4;
}

static boolean isLastByte(unsigned char c)
{
    // 多字节字符的最后一个字节的最高位为 10
    return (c & 0xC0) == 0x80;
}

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

static void videoStart()
{
    String fileName;
    if (fileAppRunData->currPath != ROOT)
    {
        fileName = fileAppRunData->currPath + "/" + fileAppRunData->currFile->file_name;
    }
    else
    {
        fileName = fileAppRunData->currPath + fileAppRunData->currFile->file_name;
    }
    int index = fileName.lastIndexOf(".");
    String suffix = fileName.substring(index + 1);

    if (suffix == "mjpeg")
    {
        fileAppRunData->currVideoFile = m_tf.open(fileName.c_str());
        fileAppRunData->decoder = new MjpegPlayDecoder(&fileAppRunData->currVideoFile, true);
    }
    else if (suffix == "rgb")
    {
        fileAppRunData->currVideoFile = m_tf.open(fileName.c_str());
        fileAppRunData->decoder = new RgbPlayDecoder(&fileAppRunData->currVideoFile, true);
    }
}

static void getTextPage()
{
    String fileName;

    if (fileAppRunData->currPath != ROOT)
    {
        fileName = fileAppRunData->currPath + "/" + fileAppRunData->currFile->file_name;
    }
    else
    {
        fileName = fileAppRunData->currPath + fileAppRunData->currFile->file_name;
    }
    File file = SD.open(fileName.c_str());

    fileAppRunData->textIsEnd = false;

    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    file.seek(fileAppRunData->textPageStartIndex[fileAppRunData->textCurrPage]);

    if (fileAppRunData->textBuffer != NULL)
    {
        free(fileAppRunData->textBuffer);
    }

    fileAppRunData->textBuffer = (unsigned char *)malloc(TEXT_PAGE_SIZE + 1);
    int length = file.read(fileAppRunData->textBuffer, TEXT_PAGE_SIZE);

    if (length == -1)
    {
        Serial.println("Failed to read file");
        return;
    }

    if (length < TEXT_PAGE_SIZE - 1)
    {
        fileAppRunData->textIsEnd = true;
    }

    fileAppRunData->textBuffer[length] = '\0';

    while (length > 0 && !isFirstByte(fileAppRunData->textBuffer[length - 1]))
    {
        length--;
        fileAppRunData->textBuffer[length - 1] = '\0';
    }

    if (length > 0 && isFirstByte(fileAppRunData->textBuffer[length - 1]) && (isLastByte(fileAppRunData->textBuffer[length - 2] || isFirstByte(fileAppRunData->textBuffer[length - 2]))))
    {
        length--;
        fileAppRunData->textBuffer[length - 1] = '\0';
    }

    // replace all \r\n to \n
    for (int i = 0; i < length; i++)
    {
        if (fileAppRunData->textBuffer[i] == '\r' && fileAppRunData->textBuffer[i + 1] == '\n')
        {
            fileAppRunData->textBuffer[i] = '\n';
            for (int j = i + 1; j < length; j++)
            {
                fileAppRunData->textBuffer[j] = fileAppRunData->textBuffer[j + 1];
            }
            length--;
        }
    }

    // replace all \n to space
    for (int i = 0; i < length; i++)
    {
        if (fileAppRunData->textBuffer[i] == '\n')
        {
            fileAppRunData->textBuffer[i] = ' ';
        }
    }

    if (fileAppRunData->textIsEnd == false)
    {
        if (fileAppRunData->textPageStartIndex.size() > fileAppRunData->textCurrPage + 1)
        {
            fileAppRunData->textPageStartIndex[fileAppRunData->textCurrPage + 1] = fileAppRunData->textPageStartIndex[fileAppRunData->textCurrPage] + length - 1;
        }
        else
        {
            fileAppRunData->textPageStartIndex.push_back(fileAppRunData->textPageStartIndex[fileAppRunData->textCurrPage] + length - 1);
        }
    }

    file.close();

    Serial.print("length: ");
    Serial.println(length);
    Serial.print("textPageStartIndex: ");
    Serial.println(fileAppRunData->textPageStartIndex[fileAppRunData->textCurrPage]);
    Serial.print("textBuffer: ");
    Serial.println((char *)fileAppRunData->textBuffer);
}

static void openFile()
{
    if (fileAppRunData->currFile->file_type == FILE_TYPE_FOLDER)
    {
        openDir(fileAppRunData->currFile->file_name);
    }
    else
    {
        String fileName;
        if (fileAppRunData->currPath != ROOT)
        {
            fileName = fileAppRunData->currPath + "/" + fileAppRunData->currFile->file_name;
        }
        else
        {
            fileName = fileAppRunData->currPath + fileAppRunData->currFile->file_name;
        }
        int index = fileName.lastIndexOf(".");
        String suffix = fileName.substring(index + 1);
        if (suffix == "jpg")
        {
            fileAppRunData->uiType = FILE_UI_TYPE_IMAGE;
            TJpgDec.drawSdJpg(0, 0, fileName);
        }
        else if (suffix == "bin")
        {
            fileAppRunData->uiType = FILE_UI_TYPE_IMAGE;
            appFileUiDisplayImage(fileName.c_str(), LV_SCR_LOAD_ANIM_NONE);
        }
        else if (suffix == "mjpeg")
        {
            fileAppRunData->uiType = FILE_UI_TYPE_VIDEO;
            appFileUiDisplayVideoInit(LV_SCR_LOAD_ANIM_NONE);
            videoStart();
        }
        else if (suffix == "rgb")
        {
            fileAppRunData->uiType = FILE_UI_TYPE_VIDEO;
            appFileUiDisplayVideoInit(LV_SCR_LOAD_ANIM_NONE);
            videoStart();
        }
        else if (suffix == "txt")
        {
            fileAppRunData->uiType = FILE_UI_TYPE_TEXT;
            fileAppRunData->textPageStartIndex.clear();
            fileAppRunData->textPageStartIndex.push_back(0);
            fileAppRunData->textCurrPage = 0;
            getTextPage();
            appFileUiDisplayText(fileAppRunData->textBuffer,
                                 fileAppRunData->textCurrPage,
                                 LV_SCR_LOAD_ANIM_NONE, true);
        }
        else
        {
            return;
        }
    }
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    if (y >= m_tft->height())
        return 0;

    m_tft->pushImage(x, y, w, h, bitmap);

    return 1;
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
    fileAppRunData->decoder = NULL;
    fileAppRunData->textBuffer = NULL;
    fileAppRunData->textIsEnd = false;
    fileAppRunData->textCurrPage = 0;
    fileAppRunData->textPageStartIndex.clear();
    fileAppRunData->textPageStartIndex.push_back(0);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(tft_output);
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
        if (action->action == ACT_BACK)
        {
            if (fileAppRunData->currPath == ROOT)
            {
                appCenter->exitApp();
                return;
            }
            else
            {
                upDir();
                force = true;
            }
        }
        else if (action->action == ACT_FORWARD)
        {
            if (fileAppRunData->currFile->file_type == FILE_TYPE_FOLDER)
            {
                openDir(fileAppRunData->currFile->file_name);
                force = true;
            }
            else
            {
                openFile();
                return;
            }
        }
        else if (action->action == ACT_LEFT || action->action == ACT_UP)
        {
            animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
            fileAppRunData->currFile = fileAppRunData->currFile->front_node;
            fileAppRunData->temp--;
        }
        else if (action->action == ACT_RIGHT || action->action == ACT_DOWN)
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
        // delay(100);
    }
    else if (fileAppRunData->uiType == FILE_UI_TYPE_IMAGE)
    {
        if (action->action == ACT_BACK)
        {
            fileAppRunData->uiType = FILE_UI_TYPE_EXPLORER;
            appFileUiDisplayExplorer(fileAppRunData->currPath.c_str(),
                                     fileAppRunData->currFile->file_name,
                                     fileAppRunData->currFile->next_node->file_name,
                                     fileAppRunData->currFile->next_node->next_node->file_name,
                                     fileAppRunData->temp, LV_SCR_LOAD_ANIM_MOVE_LEFT, true);
            return;
        }
    }
    else if (fileAppRunData->uiType == FILE_UI_TYPE_VIDEO)
    {
        if (action->action == ACT_BACK)
        {
            fileAppRunData->uiType = FILE_UI_TYPE_EXPLORER;
            setCpuFrequencyMhz(80);
            appFileUiDisplayExplorer(fileAppRunData->currPath.c_str(),
                                     fileAppRunData->currFile->file_name,
                                     fileAppRunData->currFile->next_node->file_name,
                                     fileAppRunData->currFile->next_node->next_node->file_name,
                                     fileAppRunData->temp, LV_SCR_LOAD_ANIM_MOVE_LEFT, true);
            return;
        }

        setCpuFrequencyMhz(240);

        if (fileAppRunData->currVideoFile.available())
        {
            fileAppRunData->decoder->video_play_screen();
        }
        else
        {
            if (fileAppRunData->decoder != NULL)
            {
                delete fileAppRunData->decoder;
                fileAppRunData->decoder = NULL;
            }
            fileAppRunData->currVideoFile.close();
            videoStart();
        }
    }
    else if (fileAppRunData->uiType == FILE_UI_TYPE_TEXT)
    {
        if (action->action == ACT_BACK)
        {
            fileAppRunData->uiType = FILE_UI_TYPE_EXPLORER;
            appFileUiDisplayExplorer(fileAppRunData->currPath.c_str(),
                                     fileAppRunData->currFile->file_name,
                                     fileAppRunData->currFile->next_node->file_name,
                                     fileAppRunData->currFile->next_node->next_node->file_name,
                                     fileAppRunData->temp, LV_SCR_LOAD_ANIM_MOVE_LEFT, true);
            return;
        }
        else if (action->action == ACT_FORWARD)
        {
        }
        else if (action->action == ACT_LEFT)
        {
            if (fileAppRunData->textCurrPage > 0)
            {
                fileAppRunData->textCurrPage--;
            }
            animType = LV_SCR_LOAD_ANIM_MOVE_LEFT;
        }
        else if (action->action == ACT_RIGHT)
        {
            if (fileAppRunData->textIsEnd == false)
            {
                fileAppRunData->textCurrPage++;
            }
            animType = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        }
        getTextPage();
        appFileUiDisplayText(fileAppRunData->textBuffer,
                             fileAppRunData->textCurrPage,
                             animType, false);
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