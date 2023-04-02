/*
Network.h
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
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

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

// Declared week days
static char wDays[8][8] = {
    "Mon", "Tue", "Wed", "Thr", "Fri", "Sat", "Sun",
};

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    void getTime(char *timeStr);
    bool getData(char *lat, char *lon, char *apiKey, char *city, char *temp1, char *temp2, char *temp3, char *temp4,
                 char *currentTemp, char *currentWind, char *currentTime, char *currentWeather,
                 char *currentWeatherAbbr, char *abbr1, char *abbr2, char *abbr3, char *abbr4, uint8_t *hours);
    void getDays(char *day, char *day1, char *day2, char *day3);

    // Used to store loaction woeid (world id), set in findCity()
    int location = -1;

  private:
    // Functions called from within our class
    void setTime();
    int timeZone = 0;
};

#endif
