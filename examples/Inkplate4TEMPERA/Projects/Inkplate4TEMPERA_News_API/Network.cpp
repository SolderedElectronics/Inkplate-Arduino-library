/*
    Network.cpp
    Inkplate 6 Arduino library
    David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
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
extern char api_key_news[];
extern char api_key_kraken[];
extern char api_secret[];

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
StaticJsonDocument<35000> doc;

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

    // Find internet time
    setTime();
}

// Gets time from ntp server
void Network::getTime(char *timeStr)
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

struct news* Network::getData()
{

    struct news *ent;

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

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Initiate http
    char temp[128];
    sprintf(temp, "https://newsapi.org/v2/top-headlines?country=us&apiKey=%s", api_key_news);

    http.begin(temp);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        while (http.getStream().available() && http.getStream().peek() != '{')
            (void)http.getStream().read();

        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());
        Serial.println("Deserialized!");
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }

        else if (doc["status"])
        {
            int n = doc["articles"].size();
            Serial.println(n);
            ent = (struct news*)ps_malloc(n * sizeof(struct news));
            int i = 0;
            while (i < n)
            {
                Serial.println(i);
                const char *temp_author = doc["articles"][i]["author"];
                const char *temp_title = doc["articles"][i]["title"];
                const char *temp_description = doc["articles"][i]["description"];
                const char *temp_image = doc["articles"][i]["urlToImage"];
                const char *temp_content = doc["articles"][i]["content"];
                Serial.println("Fetched");
                if (temp_author != NULL)
                    strcpy(ent[i].author, temp_author);
                else
                    strcpy(ent[i].author, "\r\n");
                if (temp_title != NULL)
                    strcpy(ent[i].title, temp_title);
                else
                    strcpy(ent[i].title, "\r\n");
                if (temp_description != NULL)
                    strcpy(ent[i].description, temp_description);
                else
                    strcpy(ent[i].description, "\r\n");
                if (temp_image != NULL)
                    strcpy(ent[i].image, temp_image);
                else
                    strcpy(ent[i].image, "\r\n");
                if (temp_content != NULL)
                    strcpy(ent[i].content, temp_content);
                else
                    strcpy(ent[i].content, "\r\n");
                i++;
            }
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("No news"));
        display.display();
        while (1)
            ;
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    return ent;
}

// Function for initial time setting ovet the ntp server
void Network::setTime()
{
    // Used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    delay(3000);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
        break;
    }

    Serial.println();

    // Used to store time info
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}
