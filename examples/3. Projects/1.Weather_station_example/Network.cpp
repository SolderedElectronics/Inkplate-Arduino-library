// defines in header for readability
#include "Network.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

// WiFiMulti object declaration
WiFiMulti WiFiMulti;

// Static Json from ArduinoJson library
StaticJsonDocument<6000> doc;

// declared week days
char weekDays[8][8] = {
    "Mon",
    "Tue",
    "Wed",
    "Thr",
    "Fri",
    "Sat",
    "Sun",
};

void Network::begin(char *city)
{
    // initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, pass);

    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFiMulti.run() != WL_CONNECTED))
    {
        Serial.print(F("."));
        delay(1000);
    }
    Serial.println(F(" connected"));

    // find internet time
    setTime();

    // search for given cities woeid
    findCity(city);
}

// gets time from ntp server
void Network::getTime(char *timeStr)
{
    // get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    //copies time string into timeStr
    strncpy(timeStr, asctime(&timeinfo) + 11, 5);

    // setting time string timezone
    int hr = 10 * timeStr[0] + timeStr[1] + timeZone;

    // better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // adding time to '0' char makes it into whatever time char, for both digits
    timeStr[0] = hr / 10 + '0';
    timeStr[1] = hr % 10 + '0';
}

void formatTemp(char *str, float temp)
{
    // built in function for float to char* conversion
    dtostrf(temp, 2, 0, str);

    // concat "C" to end
    strcat(str, "C");
}

void formatWind(char *str, float wind)
{
    // built in function for float to char* conversion
    dtostrf(14.0, 2, 0, str);

    // concat m/s to end
    strcat(str, "m/s");
}

void Network::getData(char *city, char *temp1, char *temp2, char *temp3, char *temp4, char *currentTemp, char *currentWind, char *currentTime, char *currentWeather, char *currentWeatherAbbr)
{
    // return if wifi isn't connected
    if (WiFi.status() != WL_CONNECTED)
        return;

    // wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // http object used to make get request
    HTTPClient http;

    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // add woeid to api call
    char url[256];
    sprintf(url, "https://www.metaweather.com/api/location/%d/", location);

    // initiate http
    http.begin(url);

    // actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        int32_t len = http.getSize();

        if (len > 0)
        {
            // try parsing JSON object
            DeserializationError error = deserializeJson(doc, http.getStream());

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            else
            {
                // set all data got from internet using formatTemp and formatWind defined above
                // this part relies heavily on ArduinoJson library
                formatTemp(currentTemp, doc["consolidated_weather"][0][F("the_temp")].as<float>());
                formatWind(currentWind, doc["consolidated_weather"][0][F("wind_speed")].as<float>());

                strcpy(city, doc["title"].as<char *>());
                strcpy(currentWeather, doc["consolidated_weather"][0]["weather_state_name"].as<char *>());
                strcpy(currentWeatherAbbr, doc["consolidated_weather"][0]["weather_state_abbr"].as<char *>());

                formatTemp(temp1, doc["consolidated_weather"][0][F("the_temp")].as<float>());
                formatTemp(temp2, doc["consolidated_weather"][1][F("the_temp")].as<float>());
                formatTemp(temp3, doc["consolidated_weather"][2][F("the_temp")].as<float>());
                formatTemp(temp4, doc["consolidated_weather"][3][F("the_temp")].as<float>());
            }
        }
    }

    // clear document and end http
    doc.clear();
    http.end();

    // return to initial state
    WiFi.setSleep(sleep);
}

void Network::setTime()
{
    // used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }

    Serial.println();

    // used to store time info
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

void Network::getDays(char *day, char *day1, char *day2, char *day3)
{
    // seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // find weekday

    // we get seconds since 1970, add 3600 (1 hour) times the time zone and add 3 to
    // make monday the first day of the week, as 1.1.1970. was a thursday
    // finally do mod 7 to insure our day is within [0, 6]
    int dayWeek = ((long)((nowSecs + 3600L * timeZone) / 86400L) + 3) % 7;

    // copy day data to globals in main file
    strncpy(day, weekDays[dayWeek], 3);
    strncpy(day1, weekDays[(dayWeek + 1) % 7], 3);
    strncpy(day2, weekDays[(dayWeek + 2) % 7], 3);
    strncpy(day3, weekDays[(dayWeek + 3) % 7], 3);
}

void Network::findCity(char *city)
{
    // if not connected to wifi, return
    if (WiFi.status() != WL_CONNECTED)
        return;

    // wake wifi module and save initial state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // http object
    HTTPClient http;

    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // add query param to url
    char url[256];
    strcpy(url, "https://www.metaweather.com/api/location/search/?query=");
    strcat(url, city);

    // initiate http
    http.begin(url);

    // do get request
    int httpCode = http.GET();
    if (httpCode == 200) // 200: http success
    {
        int32_t len = http.getSize();

        if (len > 0)
        {
            // try to parse JSON object
            DeserializationError error = deserializeJson(doc, http.getStream());

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            else
            {
                // empty list means no matches
                if (doc.size() == 0)
                {
                    Serial.println(F("City not found"));
                }
                else
                {
                    // woeid id used for fetching data later on
                    location = doc[0]["woeid"].as<int>();

                    Serial.println(F("Found city, woied:"));
                    Serial.println(location);
                }
            }
        }
    }

    // clear document and end http
    doc.clear();
    http.end();

    // return module to initial state
    WiFi.setSleep(sleep);
};