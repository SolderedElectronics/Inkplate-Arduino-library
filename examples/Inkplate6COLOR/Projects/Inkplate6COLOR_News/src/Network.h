/*
    Network.h
    Inkplate 6COLOR Arduino library
    Matej Andračić @ Soldered
    May 5, 2025
    https://github.com/SolderedElectronics/Inkplate-Arduino-library/tree/master/examples/Inkplate6COLOR

    For support, please reach over forums: forum.e-radionica.com/en
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
    Please review the LICENSE file included with this example.
    If you have any questions about licensing, please contact techsupport@e-radionica.com
    Distributed as-is; no warranty is given.
*/

#ifndef NETWORK_H
#define NETWORK_H

#include "Arduino.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <cstring> // For std::memcpy
#include "Inkplate.h"

// Struct to hold news data
struct news
{
    char *title;       // Dynamically allocated memory for title
    char *description; // Dynamically allocated memory for description

    // Constructor to initialize pointers to nullptr
    news()
    {
        title = nullptr;
        description = nullptr;
    }

    // Destructor to free allocated memory
    ~news()
    {
        if (title)
            free(title);
        if (description)
            free(description);
    }
};

// Network class to handle WiFi and API interactions
class Network
{
  public:
    void setCredentials(const char *wifiSSID, const char *wifiPass, const char *apiKey);
    void setTimeZone(int tz);
    void begin();
    struct news* getData(Inkplate &inkplate); // Pass Inkplate object as a parameter

  private:
    char ssid[64];
    char pass[64];
    char api_key_news[64];
    int timeZone;
};

#endif
