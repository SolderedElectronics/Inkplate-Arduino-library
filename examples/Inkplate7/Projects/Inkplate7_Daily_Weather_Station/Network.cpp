/*
    Network.cpp
    Inkplate 7 Arduino library
    David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
    April 11, 2023
    https://github.com/SolderedElectronics/Inkplate-Arduino-library

    For support, please reach over forums: https://forum.soldered.com/
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0:
   https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file included with this example. If you have
   any questions about licensing, please contact hello@soldered.com
   Distributed as-is; no warranty is given.
*/

// Network.cpp contains various functions and classes that enable Weather station
// They have been declared in seperate file to increase readability
#include "Network.h"

void Network::begin(char *ssid, char *pass)
{
    // Initiating wifi
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

bool Network::getData(char *lat, char *lon, char *apiKey, char *city, char *temp1, char *temp2, char *temp3,
                      char *temp4, char *currentTemp, char *currentWind, char *currentTime, char *currentWeather,
                      char *currentWeatherAbbr, char *abbr1, char *abbr2, char *abbr3, char *abbr4, uint8_t *hours)
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
    sprintf(url, "https://api.openweathermap.org/data/2.5/forecast?lat=%s&lon=%s&appid=%s", lat, lon, apiKey);

    // Initiate http
    http.begin(url);

    // Dynamic Json from ArduinoJson library
    DynamicJsonDocument doc(30000);

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
                return 0;
            }
            else
            {
                // Set all data got from internet using formatTemp and formatWind defined above
                // This part relies heavily on ArduinoJson library
                uint8_t cnt = 0, i = 0;

                formatTemp(currentTemp, doc["list"][0]["main"]["temp"].as<float>() - 273.15);
                sprintf(currentWind, "%.0f", (float)(doc["list"][0]["wind"]["speed"]));
                strcpy(currentWeather, doc["list"][0]["weather"][0]["main"]);
                strcpy(currentWeatherAbbr, doc["list"][0]["weather"][0]["icon"]);
                timeZone = doc["city"]["timezone"].as<int>() / 3600;

                while (cnt < 4)
                {
                    char temp[48];
                    strcpy(temp, doc["list"][i]["dt_txt"]);
                    // Serial.println("AAAAAAAAAAAAA");
                    // Serial.println(temp);

                    if (strstr(temp, "15:00:00")) // Show time in 15:00 for every day
                    {
                        struct tm t;
                        time_t dataEpoch = doc["list"][i]["dt"].as<time_t>();
                        t = *gmtime(&dataEpoch);
                        if (cnt == 0)
                        {
                            *hours = t.tm_wday;
                            (*hours)--;
                            if (*hours == 255)
                            {
                                *hours = 6;
                            }
                            formatTemp(temp1, doc["list"][i]["main"]["temp"].as<float>() -
                                                  273.15); // Format temperature to make it readable
                            strcpy(abbr1, doc["list"][i]["weather"][0]["icon"]
                                              .as<const char *>()); // Copy name of icon for that day forecast
                        }
                        else if (cnt == 1)
                        {
                            formatTemp(temp2, doc["list"][i]["main"]["temp"].as<float>() -
                                                  273.15); // Format temperature to make it readable
                            strcpy(abbr2, doc["list"][i]["weather"][0]["icon"]
                                              .as<const char *>()); // Copy name of icon for that day forecast
                        }
                        else if (cnt == 2)
                        {
                            formatTemp(temp3, doc["list"][i]["main"]["temp"].as<float>() -
                                                  273.15); // Format temperature to make it readable
                            strcpy(abbr3, doc["list"][i]["weather"][0]["icon"]
                                              .as<const char *>()); // Copy name of icon for that day forecast
                        }
                        else
                        {
                            formatTemp(temp4, doc["list"][i]["main"]["temp"].as<float>() -
                                                  273.15); // Format temperature to make it readable
                            strcpy(abbr4, doc["list"][i]["weather"][0]["icon"]
                                              .as<const char *>()); // Copy name of icon for that day forecast
                        }
                        cnt++;
                    }
                    i++;
                }
            }
        }
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    return 1;
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
