#include "Arduino.h"

#include <WiFi.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// wifi ssid and password
extern char *ssid;
extern char *pass;
extern char *weatherStackKey;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
public:
    // Functions we can access in main file
    void begin(char *city);
    void getTime(char *timeStr);
    bool getCurrentData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp, char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr);
    void getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp, char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr);
    void getDays(char *day, char *day1, char *day2, char *day3);

    // Used to store loaction woeid (world id), set in findCity()
    int location = -1;

private:
    // Functions called from within our class
    void setTime();
    void findCity(char *city);
};

#endif
