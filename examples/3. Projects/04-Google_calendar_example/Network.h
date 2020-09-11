#include "Arduino.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// Wifi ssid and password
extern char *ssid;
extern char *pass;

extern char *calendarURL;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
public:
    // Functions we can access in main file
    void begin();
    void getTime(char *timeStr, long offset = 0);
    bool getData(char *data);

private:
    // Functions called from within our class
    void setTime();
};

#endif
