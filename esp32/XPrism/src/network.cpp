#include "network.h"

void Network::init(String ssid, String password)
{
	Serial.println("scan start");
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0)
	{
		Serial.println("no networks found");
	}
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
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
	Serial.println("");
	Serial.print("Connecting: ");
	Serial.print(ssid.c_str());
	Serial.print(" @");
	Serial.println(password.c_str());

	WiFi.begin(ssid.c_str(), password.c_str());
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

Weather Network::getWeather(double lat, double lon, String api)
{
	Weather weather;
	HTTPClient http;
	http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + String(lat) + "," + String(lon) + "&appid=" + api + "&units=metric&lang=zh_cn");
	int httpCode = http.GET();
	if (httpCode > 0)
	{
		if (httpCode == HTTP_CODE_OK)
		{
			String payload = http.getString();
			int payloadName = payload.indexOf("\"name\":\"");
			if (payloadName != -1)
			{
				weather.name = payload.substring(payloadName + 8, payload.indexOf("\"", payloadName + 8));
			}
			int payloadDescription = payload.indexOf("\"description\":\"");
			if (payloadDescription != -1)
			{
				weather.description = payload.substring(payloadDescription + 15, payload.indexOf("\"", payloadDescription + 15));
			}
			int payloadTemp = payload.indexOf("\"temp\":");
			if (payloadTemp != -1)
			{
				weather.temp = payload.substring(payloadTemp + 7, payload.indexOf(",", payloadTemp + 7)).toDouble();
			}
			int payloadFeelsLike = payload.indexOf("\"feels_like\":");
			if (payloadFeelsLike != -1)
			{
				weather.feels_like = payload.substring(payloadFeelsLike + 13, payload.indexOf(",", payloadFeelsLike + 13)).toDouble();
			}
			int payloadPressure = payload.indexOf("\"pressure\":");
			if (payloadPressure != -1)
			{
				weather.pressure = payload.substring(payloadPressure + 11, payload.indexOf(",", payloadPressure + 11)).toInt();
			}
			int payloadHumidity = payload.indexOf("\"humidity\":");
			if (payloadHumidity != -1)
			{
				weather.humidity = payload.substring(payloadHumidity + 11, payload.indexOf(",", payloadHumidity + 11)).toInt();
			}
			int payloadWindSpeed = payload.indexOf("\"speed\":");
			if (payloadWindSpeed != -1)
			{
				weather.wind_speed = payload.substring(payloadWindSpeed + 8, payload.indexOf(",", payloadWindSpeed + 8)).toDouble();
			}
			int payloadWindDeg = payload.indexOf("\"deg\":");
			if (payloadWindDeg != -1)
			{
				weather.wind_deg = payload.substring(payloadWindDeg + 6, payload.indexOf(",", payloadWindDeg + 6)).toInt();
			}
			int payloadClouds = payload.indexOf("\"clouds\":");
			if (payloadClouds != -1)
			{
				weather.clouds = payload.substring(payloadClouds + 10, payload.indexOf(",", payloadClouds + 10)).toInt();
			}
			int payloadVisibility = payload.indexOf("\"visibility\":");
			if (payloadVisibility != -1)
			{
				weather.visibility = payload.substring(payloadVisibility + 13, payload.indexOf(",", payloadVisibility + 13)).toInt();
			}
			int payloadDt = payload.indexOf("\"dt\":");
			if (payloadDt != -1)
			{
				weather.dt = payload.substring(payloadDt + 6, payload.indexOf(",", payloadDt + 6)).toInt();
			}
			int payloadSunrise = payload.indexOf("\"sunrise\":");
			if (payloadSunrise != -1)
			{
				weather.sunrise = payload.substring(payloadSunrise + 11, payload.indexOf(",", payloadSunrise + 11)).toInt();
			}
			int payloadSunset = payload.indexOf("\"sunset\":");
			if (payloadSunset != -1)
			{
				weather.sunset = payload.substring(payloadSunset + 10, payload.indexOf(",", payloadSunset + 10)).toInt();
			}
		}
		else
		{
			Serial.println("Error on HTTP request");
		}
	}
	else
	{
		Serial.println("Error on HTTP request");
	}
	http.end();
}