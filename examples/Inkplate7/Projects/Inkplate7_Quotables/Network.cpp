/*
    Network.cpp
    Inkplate Arduino library
    David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
    September 24, 2020
    https://github.com/e-radionicacom/Inkplate-6-Arduino-library

    For support, please reach over forums: forum.e-radionica.com/en
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0:
   https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file included with this example. If you have
   any questions about licensing, please contact techsupport@e-radionica.com Distributed as-is; no warranty is given.
*/

#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>
// external parameters from our main file
extern char ssid[];
extern char pass[];

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
StaticJsonDocument<30000> doc;

void Network::begin(char *ssid, char *pass)
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int cnt = 0;
    display.print(F("Waiting for WiFi to connect..."));
    display.partialUpdate(true);
    while ((WiFi.status() != WL_CONNECTED))
    {
        display.print(F("."));
        display.partialUpdate(true);
        delay(1000);
        ++cnt;

        if (cnt == 20)
        {
            display.println("Can't connect to WIFI, restarting");
            display.partialUpdate(true);
            delay(100);
            ESP.restart();
        }
    }
    display.println(F(" connected"));
    display.partialUpdate(true);

}

bool Network::getData(char* text, char* auth)
{
    bool f = 0;

    // If not connected to wifi reconnect wifi
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        delay(5000);

        int cnt = 0;
        display.println(F("Waiting for WiFi to reconnect..."));
        display.partialUpdate(true);
        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            if (cnt == 7)
            {
                Serial.println("Can't connect to WIFI, restart initiated.");
                delay(100);
                ESP.restart();
            }
        }
    }

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Initiate http
    char link[] = "https://api.quotable.kurokeita.dev/api/quotes/random";
    http.begin(link);

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
        else
        {
            // Set all data got from internet using formatTemp and formatWind defined above
            // This part relies heavily on ArduinoJson library
            if(strlen(doc["quote"]["content"])>128)
            {
                return false;
            }
            const char *buff2 = doc["quote"]["author"]["name"];
            strncpy(auth,buff2,35);

            Serial.println("Success");

            const char *buff1 = doc["quote"]["content"];
            strncpy(text,buff1,128);

           


            // Save our data to data pointer from main file
            f = 0;
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("Info has not been found!"));
        display.display();
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
