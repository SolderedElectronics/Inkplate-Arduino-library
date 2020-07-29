#include "Arduino.h"

#include <WiFi.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

// to get timeZone from main file
extern int timeZone;

// wifi ssid and password
extern char *ssid;
extern char *pass;

#ifndef NETWORK_H
#define NETWORK_H

// all functions defined in Network.cpp

class Network
{
public:
    // functions we can access in main file
    void begin(char *city);
    void getTime(char *timeStr);
    void getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp, char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr);
    void getDays(char *day, char *day1, char *day2, char *day3);

    // used to store loaction woeid (world id), set in findCity()
    int location = -1;

private:
    // functions called from within our class
    void setTime();
    void findCity(char *city);
};

#endif
