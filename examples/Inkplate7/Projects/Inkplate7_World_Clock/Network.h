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

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#ifndef NETWORK_H
#define NETWORK_H

/**
 * @brief           Gelper class for all the network operations
 * 
*/
class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    char *getFullCityName(char *city);
    bool getData(char *city, int * hours, int * minutes);
    bool getAllCities();

  private:
    char allFetchedCities[650][33];
    int numCities = 0;
    WiFiClientSecure *timeApiClient;
};

#endif
