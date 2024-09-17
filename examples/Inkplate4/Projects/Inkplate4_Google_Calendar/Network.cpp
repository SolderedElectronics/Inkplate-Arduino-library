/*
Network.cpp
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

#include "Network.h"

// Connect Inkplate to the WiFi 
void Network::begin(char *ssid, char *pass)
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    // Waiting to WiFi connect
    int cnt = 0;
    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
        // Prints a dot every second that wifi isn't connected
        Serial.print(F("."));
        delay(1000);
        ++cnt;

        // If it doesn't connect to wifi in 10 seconds, reset the ESP
        if (cnt == 10)
        {
            Serial.println("Can't connect to WIFI, restarting");
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));
}

// Gets time from ntp server
void Network::getTime(char *timeStr, long offSet, int timeZone)
{
    // Get seconds since 1.1.1970. and add timezone
    time_t nowSecs = time(nullptr) + (long)timeZone * 3600L + offSet;

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    // Copies time string into timeStr
    strcpy(timeStr, asctime(&timeinfo));
}

// Function to get all raw data from the web
bool Network::getData(char *calendarURL, char *data)
{
    // Variable to store fail
    bool f = 0;

    // If not connected to WiFi, reconnect wifi
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        // Waiting to WiFi connect again
        int cnt = 0;
        Serial.println(F("Waiting for WiFi to reconnect..."));
        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            // If it doesn't connect to wifi in 10 seconds, reset the ESP
            if (cnt == 10)
            {
                Serial.println("Can't connect to WIFI, restart initiated.");
                delay(100);
                ESP.restart();
            }
        }
    }

    // Http object used to make get request
    HTTPClient http;
    http.getStream().setTimeout(10);
    http.getStream().flush();
    http.getStream().setNoDelay(true);

    // Begin http by passing url to it
    http.begin(calendarURL);

    delay(300);

    int attempts = 1;

    // Download data until it's a verified complete download
    // Actually do request
    int httpCode = http.GET();

    if (httpCode == 200)
    {
        long n = 0;

        long now = millis();

        while (millis() - now < DOWNLOAD_TIMEOUT)
        {
            while (http.getStream().available())
            {
                data[n++] = http.getStream().read();
                now = millis();
            }
        }

        data[n++] = 0;

        // If the calendar doesn't contain this string - it's invalid
        if(strstr(data, "END:VCALENDAR") == NULL) f = 1;
    }
    else
    {
        // In case there was another HTTP code, break from the function
        Serial.print("HTTP Code: ");
        Serial.print(httpCode);
        f = 1;
    }

    // end http
    http.end();

    return !f;
}
