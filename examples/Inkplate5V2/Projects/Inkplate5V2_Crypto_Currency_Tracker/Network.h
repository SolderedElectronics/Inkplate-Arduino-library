/**
 * @file            Network.h
 *
 * @brief           Header file for utilizing an internet connection via WiFi to run the Inkplate 5 Crypto tracker project
 *                  Want to learn more about Inkplate? Visit www.inkplate.io
 *                  Looking to get support? Write on our forums: https://forum.soldered.com/
 *                  22 March 2023 by Soldered
 *
 *                  This code is released under the GNU Lesser General Public
 *                  License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *                  LICENSE file included with this example.
 * 
 *                  If you have any questions about licensing, please contact hello@soldered.com
 *                  Distributed as-is; no warranty is given.
 *
 * @authors         David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
 */

#include "Arduino.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin(char * ssid, char * pass);
    void getTime(char *timeStr, int timeZone);
    bool getData(double *data, char * currency);

  private:
};

#endif
