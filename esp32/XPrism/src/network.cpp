#include "network.h"
#include "common.h"
#include <esp_wifi.h>
#include "HardwareSerial.h"

IPAddress local_ip(192, 168, 1, 184); // Set your server's fixed IP address here
IPAddress gateway(192, 168, 1, 1);    // Set your network Gateway usually your Router base address
IPAddress subnet(255, 255, 0, 0);     // Set your network sub-network mask here
IPAddress dns(223, 5, 5, 5);          // Set your network DNS usually your Router base address

const char *AP_SSID = "XPrism_AP"; // 热点名称
const char *HOST_NAME = "XPrism";  // 主机名

uint16_t ap_timeout = 0; // ap无连接的超时时间

TimerHandle_t xTimer_ap;

Network::Network()
{
    m_preDisWifiConnInfoMillis = 0;
    WiFi.enableSTA(false);
    WiFi.enableAP(false);
}

void Network::search_wifi(void)
{
    Serial.println("scan start");
    int wifi_num = WiFi.scanNetworks();
    Serial.println("scan done");
    if (0 == wifi_num)
    {
        Serial.println("no networks found");
    }
    else
    {
        Serial.print(wifi_num);
        Serial.println(" networks found");
        for (int cnt = 0; cnt < wifi_num; ++cnt)
        {
            Serial.print(cnt + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(cnt));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(cnt));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(cnt) == WIFI_AUTH_OPEN) ? " " : "*");
        }
    }
}

boolean Network::start_conn_wifi(const char *ssid, const char *password)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F("\nWiFi is OK.\n"));
        return false;
    }

    Serial.println("");
    Serial.print(F("Connecting: "));
    Serial.print(ssid);
    Serial.print(F(" @ "));
    Serial.println(password);

    WiFi.enableSTA(true);
    WiFi.setHostname(HOST_NAME);
    WiFi.begin(ssid, password);
    m_preDisWifiConnInfoMillis = GET_SYS_MILLIS();

    return true;
}

boolean Network::end_conn_wifi(void)
{
    if (WL_CONNECTED != WiFi.status())
    {
        if (doDelayMillisTime(10000, &m_preDisWifiConnInfoMillis, false))
        {
            // 这个if为了减少频繁的打印
            Serial.println(F("\nWiFi connect error.\n"));
        }
        return CONN_ERROR;
    }

    if (doDelayMillisTime(10000, &m_preDisWifiConnInfoMillis, false))
    {
        // 这个if为了减少频繁的打印
        Serial.println(F("\nWiFi connected"));
        Serial.print(F("IP address: "));
        Serial.println(WiFi.localIP());
    }
    return CONN_SUCC;
}

boolean Network::close_wifi(void)
{
    if (WiFi.getMode() & WIFI_MODE_AP)
    {
        WiFi.enableAP(false);
        Serial.println(F("AP shutdowm"));
    }

    if (!WiFi.disconnect())
    {
        return false;
    }
    WiFi.enableSTA(false);
    WiFi.mode(WIFI_MODE_NULL);
    Serial.println(F("WiFi disconnect"));
    return true;
}

boolean Network::open_ap(const char *ap_ssid, const char *ap_password)
{
    WiFi.enableAP(true); // 配置为AP模式
    // 修改主机名
    WiFi.setHostname(HOST_NAME);
    // WiFi.begin();
    boolean result = WiFi.softAP(ap_ssid, ap_password); // 开启WIFI热点
    if (result)
    {
        WiFi.softAPConfig(local_ip, gateway, subnet);
        IPAddress myIP = WiFi.softAPIP();

        // 打印相关信息
        Serial.print(F("\nSoft-AP IP address = "));
        Serial.println(myIP);
        Serial.println(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
        Serial.println(F("waiting ..."));
        ap_timeout = 300; // 开始计时
    }
    else
    {
        // 开启热点失败
        Serial.println(F("WiFiAP Failed"));
        return false;
        delay(1000);
        ESP.restart(); // 复位esp32
    }
    // 设置域名
    if (MDNS.begin(HOST_NAME))
    {
        Serial.println(F("MDNS responder started"));
    }
    return true;
}

void restCallback(TimerHandle_t xTimer)
{
    // 长时间不访问WIFI Config 将复位设备
    --ap_timeout;
    Serial.print(F("AP timeout: "));
    Serial.println(ap_timeout);
    if (ap_timeout < 1)
    {
        // todo
        WiFi.softAPdisconnect(true);
        // ESP.restart();
    }
}