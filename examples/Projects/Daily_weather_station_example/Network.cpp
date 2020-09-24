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

// Network.cpp contains various functions and classes that enable Weather station
// They have been declared in seperate file to increase readability
#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>

#include <ArduinoJson.h>

// Static Json from ArduinoJson library
StaticJsonDocument<6000> doc;

// Declared week days
char weekDays[8][8] = {
    "Mon", "Tue", "Wed", "Thr", "Fri", "Sat", "Sun",
};

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

    // Search for given cities woeid
    findCity(city);
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
    int hr = 10 * timeStr[0] + timeStr[1] + timeZone;

    // Better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // Adding time to '0' char makes it into whatever time char, for both digits
    timeStr[0] = hr / 10 + '0';
    timeStr[1] = hr % 10 + '0';
}

void formatTemp(char *str, float temp)
{
    // Built in function for float to char* conversion
    dtostrf(temp, 2, 0, str);
}

void formatWind(char *str, float wind)
{
    // Built in function for float to char* conversion
    dtostrf(wind, 2, 0, str);
}

void Network::getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp,
                      char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr, char *abbr1,
                      char *abbr2, char *abbr3, char *abbr4)
{
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
    sprintf(url, "https://www.metaweather.com/api/location/%d/", location);

    // Initiate http
    http.begin(url);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        int32_t len = http.getSize();

        if (len > 0)
        {
            // Try parsing JSON object
            DeserializationError error = deserializeJson(doc, http.getStream());

            // If an error happens print it to Serial monitor
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            else
            {
                // Set all data got from internet using formatTemp and formatWind defined above
                // This part relies heavily on ArduinoJson library
                formatTemp(currentTemp, doc["consolidated_weather"][0][F("the_temp")].as<float>());
                formatWind(currentWind, doc["consolidated_weather"][0][F("wind_speed")].as<float>());

                strcpy(city, doc["title"].as<char *>());
                strcpy(currentWeather, doc["consolidated_weather"][0]["weather_state_name"].as<char *>());
                strcpy(currentWeatherAbbr, doc["consolidated_weather"][0]["weather_state_abbr"].as<char *>());

                strcpy(abbr1, doc["consolidated_weather"][0]["weather_state_abbr"].as<char *>());
                strcpy(abbr2, doc["consolidated_weather"][1]["weather_state_abbr"].as<char *>());
                strcpy(abbr3, doc["consolidated_weather"][2]["weather_state_abbr"].as<char *>());
                strcpy(abbr4, doc["consolidated_weather"][3]["weather_state_abbr"].as<char *>());

                formatTemp(temp1, doc["consolidated_weather"][0][F("the_temp")].as<float>());
                formatTemp(temp2, doc["consolidated_weather"][1][F("the_temp")].as<float>());
                formatTemp(temp3, doc["consolidated_weather"][2][F("the_temp")].as<float>());
                formatTemp(temp4, doc["consolidated_weather"][3][F("the_temp")].as<float>());
            }
        }
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);
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

void Network::getDays(char *day, char *day1, char *day2, char *day3)
{
    // Seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // Find weekday

    // We get seconds since 1970, add 3600 (1 hour) times the time zone and add 3 to
    // make monday the first day of the week, as 1.1.1970. was a thursday
    // finally do mod 7 to insure our day is within [0, 6]
    int dayWeek = ((long)((nowSecs + 3600L * timeZone) / 86400L) + 3) % 7;

    // Copy day data to globals in main file
    strncpy(day, weekDays[dayWeek], 3);
    strncpy(day1, weekDays[(dayWeek + 1) % 7], 3);
    strncpy(day2, weekDays[(dayWeek + 2) % 7], 3);
    strncpy(day3, weekDays[(dayWeek + 3) % 7], 3);
}

void Network::findCity(char *city)
{
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

    // Wake wifi module and save initial state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Http object
    HTTPClient http;

    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Add query param to url
    char url[256];
    strcpy(url, "https://www.metaweather.com/api/location/search/?query=");
    strcat(url, city);

    // Initiate http
    http.begin(url);

    // Do get request
    int httpCode = http.GET();
    if (httpCode == 200) // 200: http success
    {
        int32_t len = http.getSize();

        if (len > 0)
        {
            // Try to parse JSON object
            DeserializationError error = deserializeJson(doc, http.getStream());

            // Print error to Serial monitor if one exsists
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            else
            {
                // Empty list means no matches
                if (doc.size() == 0)
                {
                    Serial.println(F("City not found"));
                }
                else
                {
                    // Woeid id used for fetching data later on
                    location = doc[0]["woeid"].as<int>();

                    Serial.println(F("Found city, woied:"));
                    Serial.println(location);
                }
            }
        }
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return module to initial state
    WiFi.setSleep(sleep);
};