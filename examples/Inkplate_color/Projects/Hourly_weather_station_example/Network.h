/*
Network.h
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"

#include "Inkplate.h"
#include <WiFi.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// wifi ssid and password
extern char *ssid;
extern char *pass;

extern char *lon;
extern char *lat;

extern char *apiKey;

extern Inkplate display;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *city);
    void getTime(char *timeStr);
    bool getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp, char *currentWind,
                 char *currentTime, char *currentWeather, char *currentWeatherAbbr, char *abbr1, char *abbr2,
                 char *abbr3, char *abbr4);
    void getHours(char *hour1, char *hour2, char *hour3, char *hour4);

    // Used for storing retrieved data timestamp
    time_t dataEpoch = 0;

  private:
    // Functions called from within our class
    void setTime();
};

#endif
