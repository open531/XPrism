#include "gps.h"

HardwareSerial gpsSerial(1);

GPS::GPS()
{
}

void GPS::init()
{
    gpsSerial.begin(9600);
}

void GPS::update()
{
    while (gpsSerial.available() > 0)
    {
        char c = gpsSerial.read();
        Serial.print(c);
        if (gps.encode(c))
        {
            if (gps.location.isValid())
            {
                data.latitude = gps.location.lat();
                data.longitude = gps.location.lng();
                data.altitude = gps.altitude.meters();
                data.speed = gps.speed.kmph();
                data.satellites = gps.satellites.value();
            }
        }
    }
}

double GPS::getLatitude()
{
    return data.latitude;
}

double GPS::getLongitude()
{
    return data.longitude;
}

double GPS::getAltitude()
{
    return data.altitude;
}

double GPS::getSpeed()
{
    return data.speed;
}

int GPS::getSatellites()
{
    return data.satellites;
}
