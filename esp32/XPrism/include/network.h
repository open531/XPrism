#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "info.h"

class Network
{
private:
public:
	void init(String ssid, String password);
	Weather getWeather(double lat, double lon, String api);
};

#endif