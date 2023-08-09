#include "sd_card.h"
#include <SD_MMC.h>

static fs::FS *tfVfs = NULL;

#define TFVFS_IS_NULL(RET)                     \
    if (tfVfs == NULL)                         \
    {                                          \
        Serial.println("SDCard Mount Failed"); \
        return RET;                            \
    }

void releaseFileInfo(FileInfo *info)
{
    FileInfo *curr = NULL;
    if (info == NULL)
    {
        return;
    }
    for (curr = info->next; curr != NULL;)
    {
        if (curr == info->next)
        {
            break;
        }
        FileInfo *tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    delete info;
}

static const char *getFileName(const char *path)
{
    const char *p = path;
    const char *q = path;
    while (*p != '\0')
    {
        if (*p == '/')
        {
            q = p + 1;
        }
        p++;
    }
    return q;
}

void SDCard::init()
{
    SPIClass *spi = new SPIClass(HSPI);
    spi->begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    if (!SD.begin(PIN_SD_CS, *spi, 80000000))
    {
        Serial.println("SDCard Mount Failed");
        return;
    }
    tfVfs = &SD;
    Serial.println("SDCard Mount Success");
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void SDCard::listDir(const char *dirname, uint8_t levels)
{
    TFVFS_IS_NULL();
    Serial.printf("Listing directory: %s\n", dirname);

    File root = tfVfs->open(dirname);
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

FileInfo *SDCard::listDir(const char *dirname)
{
    TFVFS_IS_NULL(NULL);
    Serial.printf("Listing directory: %s\n", dirname);

    File root = tfVfs->open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return NULL;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return NULL;
    }

    FileInfo *head = NULL;
    FileInfo *tail = NULL;
    File file = root.openNextFile();
    while (file)
    {
        FileInfo *info = new FileInfo();
        if (head == NULL)
        {
            head = info;
        }
        if (tail != NULL)
        {
            tail->next = info;
            info->prev = tail;
        }
        tail = info;
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            info->type = FILE_TYPE_DIR;
            strcpy(info->name, getFileName(file.name()));
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
            info->type = FILE_TYPE_FILE;
            strcpy(info->name, getFileName(file.name()));
            info->size = file.size();
        }
        file = root.openNextFile();
    }
    return head;
}

void SDCard::createDir(const char *path)
{
    TFVFS_IS_NULL();
    Serial.printf("Creating Dir: %s\n", path);
    if (tfVfs->mkdir(path))
    {
        Serial.println("- Dir created");
    }
    else
    {
        Serial.println("- mkdir failed");
    }
}

void SDCard::removeDir(const char *path)
{
    TFVFS_IS_NULL();
    Serial.printf("Removing Dir: %s\n", path);
    if (tfVfs->rmdir(path))
    {
        Serial.println("- Dir removed");
    }
    else
    {
        Serial.println("- rmdir failed");
    }
}

void SDCard::readFile(const char *path)
{
    TFVFS_IS_NULL();
    Serial.printf("Reading file: %s\n", path);

    File file = tfVfs->open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

String SDCard::readFileLine(const char *path, uint32_t line)
{
    TFVFS_IS_NULL("");
    Serial.printf("Reading file: %s\n", path);

    File file = tfVfs->open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return "";
    }

    Serial.println("- read from file:");
    uint32_t i = 0;
    String str = "";
    while (file.available())
    {
        char c = file.read();
        if (c == '\n')
        {
            i++;
            if (i == line)
            {
                break;
            }
        }
        else
        {
            str += c;
        }
    }
    file.close();
    return str;
}

void SDCard::writeFile(const char *path, const char *message)
{
    TFVFS_IS_NULL();
    Serial.printf("Writing file: %s\n", path);

    File file = tfVfs->open(path, FILE_WRITE);
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
        Serial.println("- frite failed");
    }
    file.close();
}

void SDCard::appendFile(const char *path, const char *message)
{
    TFVFS_IS_NULL();
    Serial.printf("Appending to file: %s\n", path);

    File file = tfVfs->open(path, FILE_APPEND);
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

void SDCard::renameFile(const char *src, const char *dst)
{
    TFVFS_IS_NULL();
    Serial.printf("Renaming file %s to %s\n", src, dst);
    if (tfVfs->rename(src, dst))
    {
        Serial.println("- file renamed");
    }
    else
    {
        Serial.println("- rename failed");
    }
}

boolean SDCard::deleteFile(const char *path)
{
    TFVFS_IS_NULL(false);
    Serial.printf("Deleting file: %s\n", path);
    if (tfVfs->remove(path))
    {
        Serial.println("- file deleted");
        return true;
    }
    else
    {
        Serial.println("- delete failed");
        return false;
    }
}

File SDCard::openFile(const char *path, const char *mode)
{
    return tfVfs->open(path, mode);
}

void SDCard::readBinaryFile(const char *path, uint8_t *buf, uint32_t len)
{
    TFVFS_IS_NULL();
    Serial.printf("Reading file: %s\n", path);

    File file = tfVfs->open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    uint32_t i = 0;
    while (file.available())
    {
        if (i >= len)
        {
            break;
        }
        buf[i++] = file.read();
    }
    file.close();
}

void SDCard::writeBinaryFile(const char *path, uint8_t *buf, uint32_t len)
{
    TFVFS_IS_NULL();
    Serial.printf("Writing file: %s\n", path);

    File file = tfVfs->open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    uint32_t i = 0;
    while (i < len)
    {
        file.write(buf[i++]);
    }
    file.close();
}
