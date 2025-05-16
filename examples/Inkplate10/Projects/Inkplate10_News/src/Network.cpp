/*
    Network.cpp
    Inkplate 10 Arduino library
    Matej Andračić @ Soldered
    May 5, 2025
    https://github.com/SolderedElectronics/Inkplate-Arduino-library/tree/master/examples/Inkplate10

    For support, please reach over forums: forum.e-radionica.com/en
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
    Please review the LICENSE file included with this example.
    If you have any questions about licensing, please contact techsupport@e-radionica.com
    Distributed as-is; no warranty is given.
*/

#include "Network.h"
#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>

// Static Json from ArduinoJson library
StaticJsonDocument<35000> doc;

void Network::setCredentials(const char *wifiSSID, const char *wifiPass, const char *apiKey)
{
    strncpy(ssid, wifiSSID, sizeof(ssid) - 1);
    strncpy(pass, wifiPass, sizeof(pass) - 1);
    strncpy(api_key_news, apiKey, sizeof(api_key_news) - 1);
}

void Network::setTimeZone(int tz)
{
    timeZone = tz;
}

void Network::begin()
{
    WiFi.begin(ssid, pass);
    Serial.println(F("Connecting to WiFi..."));

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(F("."));
    }

    Serial.println(F("\nWiFi connected."));
}

struct news* Network::getData(Inkplate &inkplate)
{
    struct news *ent = nullptr;

    // If not connected to WiFi, reconnect
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
        delay(5000);

        int cnt = 0;
        Serial.println(F("Waiting for WiFi to reconnect..."));
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            if (cnt == 7)
            {
                Serial.println(F("Can't connect to WiFi, restarting..."));
                delay(100);
                ESP.restart();
            }
        }
    }

    // Disable WiFi sleep mode temporarily
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // HTTP object for GET request
    HTTPClient http;
    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Prepare the API URL
    char temp[128];
    sprintf(temp, "https://newsapi.org/v2/top-headlines?country=us&apiKey=%s", this->api_key_news);
    Serial.printf("API URL: %s\n", temp); // Debugging: Print the API URL
    http.begin(temp);

    // Perform the GET request
    int httpCode = http.GET();
    Serial.printf("HTTP GET Response Code: %d\n", httpCode); // Debugging: Print HTTP response code

    if (httpCode == 200)
    {
        Serial.println(F("HTTP GET request successful. Parsing JSON response..."));

        while (http.getStream().available() && http.getStream().peek() != '{')
        {
            (void)http.getStream().read();
        }

        // Parse the JSON response
        DeserializationError error = deserializeJson(doc, http.getStream());
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return nullptr;
        }

        if (doc["status"])
        {
            int n = doc["articles"].size();
            Serial.printf("Number of articles fetched: %d\n", n);

            // Allocate memory for the news array
            ent = (struct news *)ps_malloc(n * sizeof(struct news));
            if (!ent)
            {
                Serial.println(F("Memory allocation failed for news array!"));
                return nullptr;
            }

            int i = 0;
            for (JsonObject article : doc["articles"].as<JsonArray>())
            {
                const char *temp_title = article["title"];
                const char *temp_description = article["description"];

                // Debugging: Print the title and description of each article
                Serial.printf("Article %d Title: %s\n", i + 1, temp_title ? temp_title : "NULL");
                Serial.printf("Article %d Description: %s\n", i + 1, temp_description ? temp_description : "NULL");

                // Allocate memory for title and copy the string
                if (temp_title)
                {
                    size_t title_len = strlen(temp_title) + 1; // Include null terminator
                    ent[i].title = (char *)malloc(title_len);
                    if (ent[i].title)
                    {
                        memcpy(ent[i].title, temp_title, title_len);
                    }
                    else
                    {
                        Serial.printf("Memory allocation failed for title of article %d.\n", i + 1);
                        ent[i].title = nullptr;
                    }
                }
                else
                {
                    ent[i].title = nullptr;
                }

                // Allocate memory for description and copy the string
                if (temp_description)
                {
                    size_t description_len = strlen(temp_description) + 1; // Include null terminator
                    ent[i].description = (char *)malloc(description_len);
                    if (ent[i].description)
                    {
                        memcpy(ent[i].description, temp_description, description_len);
                    }
                    else
                    {
                        Serial.printf("Memory allocation failed for description of article %d.\n", i + 1);
                        ent[i].description = nullptr;
                    }
                }
                else
                {
                    ent[i].description = nullptr;
                }

                i++;
            }
        }
    }
    else if (httpCode == 404)
    {
        Serial.println(F("HTTP 404: No news found."));
        // Handle case where no news is found
        inkplate.clearDisplay();
        inkplate.setCursor(50, 230);
        inkplate.setTextSize(2);
        inkplate.println(F("No news"));
        inkplate.display();
        while (1)
            ;
    }
    else
    {
        Serial.printf("HTTP GET request failed with code: %d\n", httpCode);
    }

    // Clear the JSON document and end the HTTP request
    doc.clear();
    http.end();

    // Restore WiFi sleep mode
    WiFi.setSleep(sleep);

    return ent;
}


