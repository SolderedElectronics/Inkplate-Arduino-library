/*
    Network.cpp
    Inkplate 5 Arduino library
    David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
    29 March 2023
    https://github.com/SolderedElectronics/Inkplate-Arduino-library

    For support, please reach over forums: https://forum.soldered.com/
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0:
   https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file included with this example. If you have
   any questions about licensing, please contact hello@soldered.com Distributed as-is; no warranty is given.
*/

#include "Network.h"

// Connect Inkplate to the WiFi
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

        if (cnt == 10)
        {
            Serial.println("Can't connect to WIFI, restarting");
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));
}

// Get data from the News API
struct news *Network::getData(char *apiKey)
{
    // Pointer to the struct where will be stored the news data
    struct news *ent = NULL;

    // Allocate memory for data from API
    uint32_t n = 0;
    char *buf = (char *)ps_malloc(20000);
    if (buf == NULL)
    {
        Serial.println("Memory allocation failed");
        return NULL;
    }

    // Making URL for GET request by adding the API key at the end
    char url[128];
    sprintf(url, "https://newsapi.org/v2/top-headlines?country=us&apiKey=%s", apiKey);

    // Begin HTTP connection and send get request to the news api
    Serial.println("Loading buffer...");
    HTTPClient http;
    if (http.begin(url) && http.GET() > 0)
    {
        while (http.getStreamPtr()->available())
        {
            char c = http.getStreamPtr()->read();
            buf[n++] = c;
            delayMicroseconds(10);
        }
        buf[n] = '\0';
    }
    Serial.println("The buffer is loaded completely");

    // To proper deserialization, the buffer must start with {
    char *start = strstr(buf, "{");

    // Dynamic Json from ArduinoJson library
    DynamicJsonDocument doc(20000);

    // Deserialize JSON document
    DeserializationError error = deserializeJson(doc, start);

    // Deserialize function takes data from the buffer so the buffer is no needed anymore
    free(buf);

    // If there is no error and the status is ok, allocate memory for the news and store it
    if (!error && strcmp(doc["status"], "ok") == 0)
    {
        // Get the number of articles to fetch
        int n = doc["articles"].size();

        // Allocate memory for n articles
        ent = (struct news *)ps_malloc(n * sizeof(struct news));
        if (ent == NULL)
        {
            // Return NULL to the main program if the memory isn't allocated successfully
            return NULL;
        }

        // Go through each article and store information about it
        for (int i = 0; i < n; i++)
        {
            // Temporary pointers
            const char *temp_title = doc["articles"][i]["title"].as<const char *>();
            const char *temp_description = doc["articles"][i]["description"].as<const char *>();

            // Copy values from temporary pointers to the main structure for news
            // If there is no title or description, store  "\r\n"
            if (temp_title != NULL)
            {
                strcpy(ent[i].title, temp_title);
            }
            else
            {
                strcpy(ent[i].title, "\r\n");
            }

            if (temp_description != NULL)
            {
                strcpy(ent[i].description, temp_description);
            }
            else
            {
                strcpy(ent[i].description, "\r\n");
            }
        }

        // Print the message on the Serial Monitor
        Serial.print("Fetched ");
        Serial.print(n);
        Serial.println(" news");
    }
    else
    {
        // Print a message and return NULL if a deserialization error occurs
        Serial.println("Deserialization error");
        return NULL;
    }

    // Clear the document and end http
    doc.clear();
    http.end();

    // Return the pointer to the struct with the news
    return ent;
}
