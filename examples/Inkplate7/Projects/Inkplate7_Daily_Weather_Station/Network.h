/*
Network.h
Inkplate 7 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
April 11, 2023
https://github.com/SolderedElectronics/Inkplate-Arduino-library

For support, please reach over forums: https://forum.soldered.com/
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact hello@soldered.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"
#include "Inkplate.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

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

  private:
    // Functions called from within our class
    void setTime();
    int timeZone = 0;
};

#endif
