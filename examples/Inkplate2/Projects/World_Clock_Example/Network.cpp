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

// Static Json from ArduinoJson library
DynamicJsonDocument doc(30000);

void Network::begin(char *ssid, char *pass)
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
    client.setInsecure(); // Use HTTPS but don't compare certificate
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
        // City not found
        Serial.println("Time has not been fetched!");
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

// Function to get the full city name from the abbreviation we pass in the function
char *Network::getFullCityName(char *city)
{
    static char fullName[33];
    // Check the abbreviation for each city
    for (int i = 0; i < sizeof(cities); i++)
    {
        // Try to find it in the city's full name
        if (strstr(cities[i], city))
        {
            // If it's found, return the abbreviation
            strcpy(fullName, cities[i]);
            return fullName;
        }
    }

    // Return 0 if it's not found
    return 0;
}
/*
bool Network::getAllCities(char *allCities)
{
    char *allCitiesRaw = NULL;
    allCitiesRaw = (char *)ps_malloc(15000);
    if (allCitiesRaw == NULL)
    {
        return 0;
    }

    WiFiClientSecure client;
    client.setInsecure(); // Use HTTPS but don't compare certificate
    client.flush();
    client.setTimeout(10);

    // Http object used to make get request
    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Initiate http
    http.begin(client, "https://www.timeapi.io/api/TimeZone/AvailableTimeZones");

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // pohrana podataka u niz za gradove
    }
    else if (httpCode == 404)
    {
        Serial.println("Cities has not been fetched!");
        while (1)
            ;
    }
    else
    {
        return 0;
    }
}
*/