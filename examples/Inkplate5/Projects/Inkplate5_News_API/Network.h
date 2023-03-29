/*
Network.h
Inkplate 5 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
29 March 2023
https://github.com/SolderedElectronics/Inkplate-Arduino-library

For support, please reach over forums: https://forum.soldered.com/
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact hello@soldered.com
Distributed as-is; no warranty is given.
*/

// Include needed libraries
#include "Arduino.h"
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"

// Struct for each piece of news
struct news
{
    char title[128];
    char description[1000];
};

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp
class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    struct news* getData(char *apiKey);
};

#endif
