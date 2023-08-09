#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiMulti.h>

extern IPAddress localIP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress dns;

// extern WiFiMulti wifiMulti;

enum NetworkStatus
{
    NETWORK_DISCONNECTED,
    NETWORK_CONNECTED,
    NETWORK_CONNECTING,
    NETWORK_DISCONNECTING
};

class Network
{
private:
    unsigned long lastTime = 0;

public:
    Network();
    void searchNetwork();
    boolean connectNetwork(const char *ssid, const char *password);
    boolean closeNetwork();
};

#endif