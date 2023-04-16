/*
   OpenWeatherOneCall.cpp v1.2.0 (Added new routine for City ID usage)
   copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   Open Weather Seven Day Forecast Library
   Seven Day and Current Weather Conditions
   Viva La Resistance

   Modified by Jon Fuge to get additional information
*/

#include "OpenWeatherOneCall.h"

// Constructor for make object of OpenWeatherOneCall class
OpenWeatherOneCall::OpenWeatherOneCall()
{
}

#define DS_URL1 "https://api.openweathermap.org/data/2.5/onecall"
#define DS_URL2 "&units="
#define DS_URL3 "&appid="

#define GEOLOCATIONURL "https://www.googleapis.com/geolocation/v1/geolocate"

int OpenWeatherOneCall::getCoordinates(String googleKey)
{
    HTTPClient http;

    String jsonSend; // This will be the json we send to Google with nearby MAC ADDRESSES

    // Scan for nearby WiFi networks
    int numNetworks = WiFi.scanNetworks();

    /*
       Add 3 nearest WiFi MAC Addresses in this JSON form:
      {
      "wifiAccessPoints": [
      "macAddress":"4A:5D:36:35:BA:6D",
      "macAddress":"48:5D:36:35:BA:6C",
      "macAddress":"B0:B9:8A:EF:D2:D8"
      ]
      }
    */

    // Creating the json with some string operations
    jsonSend = "{\n";
    jsonSend += "\"wifiAccessPoints\": [\n";
    for (int j = 0; j < numNetworks; ++j)
    {
        jsonSend += "{\n";
        jsonSend += "\"macAddress\" : \"";
        jsonSend += (WiFi.BSSIDstr(j));
        jsonSend += "\",\n";
        jsonSend += "\"signalStrength\": ";
        jsonSend += WiFi.RSSI(j);
        jsonSend += "\n";
        if (j < numNetworks - 1)
        {
            jsonSend += "},\n";
        }
        else
        {
            jsonSend += "}\n";
        }
    }
    jsonSend += ("]\n");
    jsonSend += ("}\n");

    // Setting up the url for the POST
    String geoLocURL = GEOLOCATIONURL;
    geoLocURL.concat("?key=");
    geoLocURL.concat(googleKey);

    // Lets send the json in the POST and get back an answer with getString(),
    // add header for application/json
    http.begin(geoLocURL);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(jsonSend);

    if (httpCode > 399)
    {
        return httpCode;
    }

    // Setting the parsing routine
    const size_t capacity = 2 * JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument geo(capacity);

    // Parsing the return from Google into geo Document
    deserializeJson(geo, http.getString());

    // Variables in parse
    latitude = geo["location"]["lat"];
    longitude = geo["location"]["lng"];

    geo.clear();
    http.end();
    return 0;
} // End of Coordinates Routine

int OpenWeatherOneCall::parseWeather(char *DKEY, char *GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS,
                                     int CITY_ID)
{
    // Clear the struct for current weather
    memset(&current, 0, sizeof current);

    // Array for url
    char getURL[200] = {0};

    // F/C units
    if (SET_UNITS)
    {
        strcpy(units, "metric");
    }
    else
    {
        strcpy(units, "imperial");
    }

    // HTTP object for GET request
    HTTPClient http;

    OpenWeatherOneCall::getCoordinates(GKEY);
    sprintf(getURL, "%s?lat=%.6f&lon=%.6f%s%s%s%s", DS_URL1, SEEK_LATITUDE, SEEK_LONGITUDE, DS_URL2, units, DS_URL3,
            DKEY);

    // Do a request
    http.begin(getURL);
    int httpCode = http.GET();

    // Check the response
    if (httpCode > 399)
    {
        return httpCode;
    }

    // Create dynamic JSON document for storing data
    const long long capacity = 32000;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, http.getString()); // Streams JSON

    // Get information about current weather
    current.timezone_offset = doc["timezone_offset"];

    JsonObject currently = doc["current"];
    current.dt = (long)currently["dt"] + current.timezone_offset;
    current.sunrise = (long)currently["sunrise"] + current.timezone_offset; // 1586773262
    current.sunset = (long)currently["sunset"] + current.timezone_offset;   // 1586820773
    current.temp = currently["temp"];                                       // 287.59
    current.pressure = currently["pressure"];                               // 1011
    current.humidity = currently["humidity"];                               // 93

    // Copy icon
    strlcpy(current.icon, currently["weather"][0]["icon"] | "01d", sizeof(current.icon) - 1);


    // Get information about hourly weather
    JsonArray everyhour = doc["hourly"];

    //===================================
    // FOR loop to fill the hourly
    //===================================

    for (int x = 0; x < (sizeof(hourly) / sizeof(hourly[0])); x++)
    {
        hourly[x].dt = (long)everyhour[x]["dt"] + current.timezone_offset;                               // 1586793600
        hourly[x].temp = everyhour[x]["temp"];                                                           // 13.45
        hourly[x].rain_1h = everyhour[x]["rain"]["1h"];                                                  // 13.45
        hourly[x].snow_1h = everyhour[x]["snow"]["1h"];                                                  // 13.45
        strlcpy(hourly[x].icon, everyhour[x]["weather"][0]["icon"] | "01d", sizeof(hourly[x].icon) - 1); // "10d"
    }

    // Get information about daily weather
    JsonArray daily = doc["daily"];

    //===================================
    // FOR loop to fill the 4 day struct
    //===================================

    for (int x = 0; x < (sizeof(forecast) / sizeof(forecast[0])); x++)
    {
        // Store data for each day
        forecast[x].dt = (long)daily[x]["dt"] + current.timezone_offset;                                 // 1586793600
        forecast[x].sunrise = (long)daily[x]["sunrise"] + current.timezone_offset;                       // 1586773262
        forecast[x].sunset = (long)daily[x]["sunset"] + current.timezone_offset;                         // 1586820773
        forecast[x].temp_min = daily[x]["temp"]["min"];                                                  // 286.56
        forecast[x].temp_max = daily[x]["temp"]["max"];                                                  // 293.23
        forecast[x].pressure = daily[x]["pressure"];                                                     // 1006
        forecast[x].humidity = daily[x]["humidity"];                                                     // 91
        forecast[x].pop = daily[x]["pop"];                                                               // 0.57
        forecast[x].rain = daily[x]["rain"];                                                             // 23.31
        forecast[x].snow = daily[x]["snow"];                                                             // 23.31
        strlcpy(forecast[x].icon, daily[x]["weather"][0]["icon"] | "01d", sizeof(forecast[x].icon) - 1); // "10d"
    }

    doc.clear();
    http.end();
    return 0;
}
