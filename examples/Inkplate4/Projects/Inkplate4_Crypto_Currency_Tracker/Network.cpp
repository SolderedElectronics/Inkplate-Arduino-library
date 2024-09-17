/**
 * @file            Network.cpp
 *
 * @brief           Functions for utilizing an internet connection via WiFi to run the Inkplate 7 Crypto tracker project
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

#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>

/**
 * @brief       Connect to WiFi with given SSID and password
 * 
 * @param       char * ssid - the WiFi's SSID
 * 
 * @param       char * pass - the WiFi's password
 * 
 * @note        This function will restart the ESP if it can't connect to WiFi after some time
 * 
 * @returns     none
*/
void Network::begin(char * ssid, char * pass)
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int cnt = 0;
    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(F("."));
        delay(1000);
        ++cnt;

        if (cnt == 20)
        {
            Serial.println("Can't connect to WIFI, restarting");
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));
}

/**
 * @brief       Get current time from NTP server
 * 
 * @param       char * timeStr - where to store the recieved time
 * 
 * @param       int timeZone - set the timezone relative to UTC
 * 
 * @returns     none
*/
void Network::getTime(char *timeStr, int timeZone)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    // Copies time string into timeStr
    strncpy(timeStr, asctime(&timeinfo) + 4, 12);

    // Setting time string timezone
    int hr = 10 * timeStr[7] + timeStr[8] + timeZone;

    // Better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // Adding time to '0' char makes it into whatever time char, for both digits
    timeStr[7] = hr / 10 + '0';
    timeStr[8] = hr % 10 + '0';
}

/**
 * @brief       Get the data for the selected coin
 * 
 * @param       double * data - where to store the fetched data
 * 
 * @param       char * currency - the ID of the cryptocurrency of interest
 * 
 * @returns     True if successful, false if not
*/
bool Network::getData(double *data, char * currency)
{
    // Dynamic Json from ArduinoJson library
    DynamicJsonDocument doc(30000);

    bool f = 0;

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Initiate http
    char temp[128];
    sprintf(temp, "https://api.coingecko.com/api/v3/coins/%s/market_chart?vs_currency=usd&days=92", currency);

    http.begin(temp);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        while (http.getStream().available() && http.getStream().peek() != '{')
            (void)http.getStream().read();

        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            f = 1;
        }
        else if (doc["prices"].size() > 31)
        {
            // Set all data got from internet using formatTemp and formatWind defined above
            // This part relies heavily on ArduinoJson library

            Serial.println("Success");

            // Save our data to data pointer from main file
            for (int i = 0; i < 31; ++i)
            {
                data[i] = doc["prices"][92 - 31 + i][1].as<double>();
                // Serial.println(data[i]);
            }
            f = 0;
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        Serial.println(F("Your entered coin does not exist!"));
        while (1)
            ;
    }
    else
    {
        f = 1;
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}