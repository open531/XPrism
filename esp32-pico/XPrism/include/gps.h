#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>

extern HardwareSerial gpsSerial;

struct GPSData
{
    double latitude;
    double longitude;
    double altitude;
    double speed;
    int satellites;
};

class GPS
{
private:
    TinyGPSPlus gps;
    int flag;
    long last_update_time;

public:
    GPSData data;

public:
    GPS();
    void init();
    void update();
    double getLatitude();
    double getLongitude();
    double getAltitude();
    double getSpeed();
    int getSatellites();
};

#endif