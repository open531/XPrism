#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define PIN_SD_CS 5
#define PIN_SD_MOSI 23
#define PIN_SD_MISO 19
#define PIN_SD_SCK 18

#define DIR_FILE_NUM 10
#define DIR_FILE_NAME_LEN 20
#define FILE_NAME_LEN 100

extern char dirFileNames[DIR_FILE_NUM][DIR_FILE_NAME_LEN];

enum FileType
{
    FILE_TYPE_DIR,
    FILE_TYPE_FILE,
    FILE_TYPE_NONE
};

struct FileInfo
{
    char name[FILE_NAME_LEN];
    FileType type;
    uint32_t size;
    FileInfo *prev;
    FileInfo *next;
};

void releaseFileInfo(FileInfo *info);

class SDCard
{
private:
    char buf[128];

public:
    void init();
    void listDir(const char *dirname, uint8_t levels);
    FileInfo *listDir(const char *dirname);
    void createDir(const char *path);
    void removeDir(const char *path);
    void readFile(const char *path);
    String readFileLine(const char *path, uint32_t line);
    void writeFile(const char *path, const char *message);
    void appendFile(const char *path, const char *message);
    void renameFile(const char *src, const char *dst);
    boolean deleteFile(const char *path);
    File openFile(const char *path, const char *mode);
    void readBinaryFile(const char *path, uint8_t *buf, uint32_t len);
    void writeBinaryFile(const char *path, uint8_t *buf, uint32_t len);
};

#endif
