/*
Network.h
Inkplate 5 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
23 March 2023
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: https://forum.soldered.com/
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact hello@soldered.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    void getTime(char *timeStr, int timeZone);
    bool getData(char *lat, char *lon, char *apiKey, char *temp1, char *temp2, char *temp3, char *temp4,
                 char *currentTemp, char *currentWind, char *currentTime, char *currentWeather,
                 char *currentWeatherAbbr, char *abbr1, char *abbr2, char *abbr3, char *abbr4, int *timeZone);
    void getHours(char *hour1, char *hour2, char *hour3, char *hour4, int timeZone);

  private:
    // Functions called from within our class
    void setTime();

    // Used for storing retrieved data timestamp
    time_t dataEpoch = 0;
};

#endif
