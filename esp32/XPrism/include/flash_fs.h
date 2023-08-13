#include <Arduino.h>
#include "FS.h"

class FlashFS
{
private:
    fs::FS *m_fs;

public:
    FlashFS();

    ~FlashFS();

    void listDir(const char *dirname, uint8_t levels);

    uint16_t readFile(const char *path, uint8_t *info);

    void writeFile(const char *path, const char *message);

    void appendFile(const char *path, const char *message);

    void renameFile(const char *src, const char *dst);

    void deleteFile(const char *path);
};

bool analyseParam(char *info, int argc, char **argv);