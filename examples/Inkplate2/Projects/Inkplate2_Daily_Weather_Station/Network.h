/*
    Network.h
    Inkplate Arduino library
    Karlo Leksic for Soldered.com
    January 2, 2023
    https://github.com/e-radionicacom/Inkplate-6-Arduino-library

    For support, please reach over forums: forum.e-radionica.com/en
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0:
   https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file included with this example. If you have
   any questions about licensing, please contact techsupport@e-radionica.com Distributed as-is; no warranty is given.
*/

#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "WiFi.h"

#ifndef NETWORK_H
#define NETWORK_H

// Struct for store forecast data
struct forecastWeather
{
    time_t timestamp;
    int minTemp;
    int maxTemp;
    char icon[5];
};

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    int getData(struct forecastWeather *fw, String lat, String lon, String apiKey, int *timezone);
    void setTime(int timezone, struct tm *timeinfo);
};

#endif
