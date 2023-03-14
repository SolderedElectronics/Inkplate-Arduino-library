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

/**
 * @brief           Initialize the network and connect to a WiFi access point
 * 
 * @param           ssid: The network's ssid
 * 
 * @param           pass: The network's password
 * 
 * @returns         None
*/
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

    // Create a WiFi client for https, set the settings and connect
    timeApiClient = (WiFiClientSecure *)ps_malloc(sizeof(WiFiClientSecure));
    timeApiClient = new WiFiClientSecure();
    timeApiClient->setHandshakeTimeout(30);
    timeApiClient->setInsecure();
    timeApiClient->setTimeout(5);

    if (!timeApiClient->connect("timeapi.io", 443))
    {
        // WiFiClientSecure error!
        Serial.println(F("Time API connect Error!"));
    }
}

/**
 * @brief           Get the hours and minutes for a certain city
 * 
 * @param           hours: Pointer to where to store hours
 * 
 * @param           minutes: Pointer to where to store minutes
 * 
 * @returns         True if OK, false if there was an error
*/
bool Network::getData(char *city, int *hours, int *minutes)
{
    // Create the JSON doc to store time
    DynamicJsonDocument doc(1024);

    // If not connected to WiFi, Reconnect
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
        delay(5000);
        int cnt = 0;

        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            if (cnt == 7)
            {
                delay(100);
                Serial.println("WiFi disconnected!");
                ESP.restart();
            }
        }
    }

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Create the request which contains the city name
    bool cityFound = 0;
    char request[120];
    for (int i = 0; i < numCities; i++)
    {
        if (strstr(allFetchedCities[i], city))
        {
            sprintf(request, "/api/Time/current/zone?timeZone=%s", allFetchedCities[i]);
            cityFound = 1;
            break;
        }
    }
    if (cityFound == 0)
    {
        Serial.println("City not found!");
        return false;
    }

    // Create HTTPClient and try to connect
    HTTPClient http;
    if (!http.begin(*timeApiClient, "timeapi.io", 443, request, true))
    {
        Serial.println("Couldn't connect to timeAPI!");
        return false;
    }

    // Edit HTTP header and set long timeouts
    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(3000);
    http.setConnectTimeout(3000);

    // Do HTTP request
    int httpCode = http.GET();
    String response = http.getString();
    if (!(httpCode == 200 || httpCode == 201))
    {
        Serial.print("TimeAPI HTTP error: ");
        Serial.print(httpCode);
        Serial.println("!");
        return false;
    }

    // End the connection
    http.getStream().flush();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    // Deserialize the JSON and store the data in the given locations
    DeserializationError error = deserializeJson(doc, response);
    *hours = doc["hour"];
    *minutes = doc["minute"];

    return true;
}

/**
 * @brief           Get the full name of a city from a substring
 * 
 * @param           city: Char array of the substring of the city's name
 * 
 * @returns         The full name of the city, 0 if it's not found
*/
char *Network::getFullCityName(char *city)
{
    static char fullName[33];
    // Check the abbreviation for each city
    for (int i = 0; i < numCities; i++)
    {
        // Try to find it in the city's full name
        if (strstr(allFetchedCities[i], city))
        {
            // If it's found, return the abbreviation
            strcpy(fullName, allFetchedCities[i]);
            return fullName;
        }
    }

    // Return 0 if it's not found
    return 0;
}

/**
 * @brief           Get a list of all available cities on timeAPI and store it locally
 * 
 * @returns         True if successful, false if not
*/
bool Network::getAllCities()
{
    // Create the JSON doc to store time
    DynamicJsonDocument doc(24576);

    // If not connected to WiFi, Reconnect
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
        delay(5000);
        int cnt = 0;

        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            if (cnt == 7)
            {
                delay(100);
                Serial.println("WiFi disconnected!");
                ESP.restart();
            }
        }
    }

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // The URL where to get the timezones
    char *request = "/api/TimeZone/AvailableTimeZones";

    // Create HTTPClient and try to connect
    HTTPClient http;
    if (!http.begin(*timeApiClient, "timeapi.io", 443, request, true))
    {
        Serial.println("Couldn't connect to timeAPI!");
        return false;
    }

    // Edit HTTP header and set long timeouts
    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(3000);
    http.setConnectTimeout(3000);

    // Do HTTP request
    int httpCode = http.GET();
    String response = http.getString();

    if (!(httpCode == 200 || httpCode == 201))
    {
        Serial.print("TimeAPI HTTP error: ");
        Serial.print(httpCode);
        Serial.println("!");
        return false;
    }

    // End the connection
    http.getStream().flush();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    // Deserialize JSON
    DeserializationError error = deserializeJson(doc, response);

    // Parse to allFetchedCities array
    bool f = 0;
    int i = 0;
    const char *allCities[650];

    while (doc[i].as<const char *>() != 0)
    {
        // Store each city
        allCities[i] = doc[i].as<const char *>();
        strcpy(allFetchedCities[i], allCities[i]);
        ++i;
    }
    numCities = i;

    return true;
}
