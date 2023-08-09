#include "network.h"
#include "system.h"

IPAddress local_IP(192, 168, 4, 2);
IPAddress gateway(192, 168, 4, 2);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 4, 1);

Network::Network()
{
}

void Network::searchNetwork()
{
    Serial.println("Searching for networks...");
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0)
    {
        Serial.println("No networks found");
    }
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
}

boolean Network::connectNetwork(const char *ssid, const char *password)
{
    Serial.println("Connecting to WiFi network: " + String(ssid));

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long currentMillis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if (millis() - currentMillis > 10000)
        {
            Serial.println("Connection timed out");
            return false;
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}

boolean Network::closeNetwork()
{
    WiFi.disconnect();
    return true;
}
