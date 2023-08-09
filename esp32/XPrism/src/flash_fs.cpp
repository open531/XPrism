#include "flash_fs.h"
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

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
            Serial.println(file.name());
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
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

uint16_t FlashFS::readFile(const char *path, uint8_t *info)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return 0;
    }

    Serial.println("- read from file:");
    uint16_t i = 0;
    while (file.available())
    {
        info[i++] = file.read();
        Serial.write(info[i - 1]);
    }
    Serial.println();
    return i;
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

void FlashFS::testFileIO(const char *path)
{
    File file = SPIFFS.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\r\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("- failed to open file for reading");
    }

    file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\r\n", 2048 * 512, end);
    file.close();
}

bool analyseParam(char *info, int argc, char **argv)
{
    if (argc == 0)
    {
        return false;
    }
    if (strcmp(argv[0], "list") == 0)
    {
        if (argc == 1)
        {
            strcpy(info, "list");
            return true;
        }
        else if (argc == 2)
        {
            strcpy(info, "list ");
            strcat(info, argv[1]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "read") == 0)
    {
        if (argc == 2)
        {
            strcpy(info, "read ");
            strcat(info, argv[1]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "write") == 0)
    {
        if (argc == 3)
        {
            strcpy(info, "write ");
            strcat(info, argv[1]);
            strcat(info, " ");
            strcat(info, argv[2]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "append") == 0)
    {
        if (argc == 3)
        {
            strcpy(info, "append ");
            strcat(info, argv[1]);
            strcat(info, " ");
            strcat(info, argv[2]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "rename") == 0)
    {
        if (argc == 3)
        {
            strcpy(info, "rename ");
            strcat(info, argv[1]);
            strcat(info, " ");
            strcat(info, argv[2]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "delete") == 0)
    {
        if (argc == 2)
        {
            strcpy(info, "delete ");
            strcat(info, argv[1]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (strcmp(argv[0], "test") == 0)
    {
        if (argc == 2)
        {
            strcpy(info, "test ");
            strcat(info, argv[1]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
