/*
  Network.cpp
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
extern const char *city1_name;
extern const char *city2_name;

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
StaticJsonDocument<30000> doc;

void Network::begin()
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

bool Network::getData(char *city, tm *t)
{
    bool f = 0;

    // If not connected to wifi reconnect wifi
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        delay(5000);

        int cnt = 0;
        Serial.println(F("Waiting for WiFi to reconnect..."));
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

    WiFiClientSecure client; 
    client.setInsecure();    // Use HTTPS but don't compare certificate
    client.flush();
    client.setTimeout(10);

    // Http object used to make get request
    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    char temp[120];
    for (int i = 0; i < sizeof(cities); i++)
    {
        if (strstr(cities[i], city))
        {
            sprintf(temp, "https://www.timeapi.io/api/Time/current/zone?timeZone=%s", cities[i]);
            Serial.println(cities[i]);
            if (city1_name == NULL)
                city1_name = cities[i];
            else
                city2_name = cities[i];
            break;
        }
    }
    if (sizeof(temp) < 20)
    {
        Serial.println("City not found");
        return 0;
    }

    // Initiate http
    http.begin(client, temp);

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

            Serial.println("Success");

            t->tm_hour = doc["hour"];
            t->tm_min = doc["minute"];

            // Save our data to data pointer from main file
            f = 0;
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        display.clearDisplay();
        display.setCursor(10, 10);
        display.setTextSize(2);
        display.println(F("Time has not been fetched!"));
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
    client.stop();

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}
