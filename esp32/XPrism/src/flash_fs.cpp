#include <Arduino.h>
#include "FS.h"
#include <SPIFFS.h>
#include <time.h>
#include "flash_fs.h"

#define FORMAT_LITTLEFS_IF_FAILED true

FlashFS::FlashFS()
{
}

FlashFS::~FlashFS()
{
}

void FlashFS::listDir(const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");

            Serial.print(file.name());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

            if (levels)
            {
                listDir(file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");

            Serial.print(file.size());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

uint16_t FlashFS::readFile(const char *path, uint8_t *info)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    uint16_t ret_len = 0;
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return ret_len;
    }

    // Serial.println("- read from file:");
    while (file.available())
    {
        ret_len += file.read(info + ret_len, 15);
        // Serial.write(file.read());
    }
    file.close();
    return ret_len;
}

void FlashFS::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
    file.close();
}

void FlashFS::appendFile(const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- message appended");
    }
    else
    {
        Serial.println("- append failed");
    }
    file.close();
}

void FlashFS::renameFile(const char *src, const char *dst)
{
    Serial.printf("Renaming file %s to %s\r\n", src, dst);
    if (SPIFFS.rename(src, dst))
    {
        Serial.println("- file renamed");
    }
    else
    {
        Serial.println("- rename failed");
    }
}

void FlashFS::deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (SPIFFS.remove(path))
    {
        Serial.println("- file deleted");
    }
    else
    {
        Serial.println("- delete failed");
    }
}

bool analyseParam(char *info, int argc, char **argv)
{
    int cnt; // 记录解析到第几个参数
    for (cnt = 0; cnt < argc; ++cnt)
    {
        argv[cnt] = info;
        while (*info != '\n')
        {
            ++info;
        }
        *info = 0;
        ++info;
    }
    return true;
}