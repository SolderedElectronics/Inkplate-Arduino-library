#include "Arduino.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// Wifi ssid and password
extern char *ssid;
extern char *pass;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
public:
    // Functions we can access in main file
    void begin();
    void getTime(char *timeStr);
    bool getData(float *data);

private:
    // Functions called from within our class
    void setTime();
};

#endif
