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

// Uncomment for American output
// #define AMERICAN

// Network.cpp contains various functions and classes that enable Weather station
// They have been declared in seperate file to increase readability
#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

// Static Json from ArduinoJson library
StaticJsonDocument<32000> doc;

void Network::begin(char *city)
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

    // reduce power by making WiFi module sleep
    WiFi.setSleep(1);
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
    strncpy(timeStr, asctime(&timeinfo) + 11, 5);

    // Setting time string timezone
    int hr = 10 * (timeStr[0] - '0') + (timeStr[1] - '0') + timeZone;

    // Better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // Adding time to '0' char makes it into whatever time char, for both digits
    timeStr[0] = hr / 10 + '0';
    timeStr[1] = hr % 10 + '0';
}

void formatTemp(char *str, float temp)
{
    // Converting to Fahrenheit
#ifdef AMERICAN
    temp = (temp * 9.0 / 5.0 + 32.0);
#endif

    // Built in function for float to char* conversion
    dtostrf(temp, 2, 0, str);
}

void formatWind(char *str, float wind)
{
    // Converting to MPH
#ifdef AMERICAN
    wind = wind * 2.237;
#endif

    // Built in function for float to char* conversion
    dtostrf(wind, 2, 0, str);
}

bool Network::getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp,
                      char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr, char *abbr1,
                      char *abbr2, char *abbr3, char *abbr4)
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

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Add woeid to api call
    char url[256];
    sprintf(url, "https://api.openweathermap.org/data/2.5/onecall?lat=%s&lon=%s&appid=%s", lat, lon, apiKey);

    // Initiate http
    http.begin(url);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {

        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());

        // If an error happens print it to Serial monitor
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

            dataEpoch = doc["current"]["dt"].as<time_t>();

            formatTemp(currentTemp, doc["current"]["temp"].as<float>() - 273.15);
            formatWind(currentWind, doc["current"][F("wind_speed")].as<float>());

            strcpy(currentWeather, doc["current"]["weather"][0]["main"].as<const char *>());
            ;
            strcpy(currentWeatherAbbr, doc["current"]["weather"][0]["icon"].as<const char *>());
            ;

            formatTemp(temp1, doc["hourly"][0]["temp"].as<float>() - 273.15);
            formatTemp(temp2, doc["hourly"][1]["temp"].as<float>() - 273.15);
            formatTemp(temp3, doc["hourly"][2]["temp"].as<float>() - 273.15);
            formatTemp(temp4, doc["hourly"][3]["temp"].as<float>() - 273.15);

            strcpy(abbr1, doc["hourly"][0]["weather"][0]["icon"].as<const char *>());
            ;
            strcpy(abbr2, doc["hourly"][1]["weather"][0]["icon"].as<const char *>());
            ;
            strcpy(abbr3, doc["hourly"][2]["weather"][0]["icon"].as<const char *>());
            ;
            strcpy(abbr4, doc["hourly"][3]["weather"][0]["icon"].as<const char *>());
            ;

            Serial.println(abbr1);
            Serial.println(abbr2);
            Serial.println(abbr3);
            Serial.println(abbr4);
            Serial.println();

            f = 0;
        }
    }
    else if (httpCode == 401)
    {
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("Network error, probably wrong api key"));
        display.display();
        while (1)
            ;
    }

    // Stop http and clear document
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}

void Network::setTime()
{
    // Used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        // Print a dot every half a second while time is not set
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }

    Serial.println();

    // Used to store time info
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

void Network::getHours(char *hour1, char *hour2, char *hour3, char *hour4)
{
    // Format hours info
    sprintf(hour1, "%2ldh", (dataEpoch / 3600L + timeZone + 24) % 24);
    sprintf(hour2, "%2ldh", (dataEpoch / 3600L + 1 + timeZone + 24) % 24);
    sprintf(hour3, "%2ldh", (dataEpoch / 3600L + 2 + timeZone + 24) % 24);
    sprintf(hour4, "%2ldh", (dataEpoch / 3600L + 3 + timeZone + 24) % 24);
}
