#ifndef ESP32_INFO_H
#define ESP32_INFO_H

struct ESP32Info
{
    char *picoIP;
    char *picoMAC;
    char *picoHostname;
    char *camIP;
    char *camMAC;
    char *camHostname;
    char *androidIP;
    char *androidMAC;
    char *androidHostname;
};

#endif