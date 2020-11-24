/*
   OpenWeatherOneCall.cpp v1.2.0 (Added new routine for City ID usage)
   copyright 2020 - Jessica Hershey
   www.github.com/JHershey69

   Open Weather Seven Day Forecast Library
   Seven Day and Current Weather Conditions
   Viva La Resistance

   Modified by Jon Fuge to get additional information
*/

#include "Arduino.h"            // Basic Arduino Library
#include "OpenWeatherOneCall.h"
#include <HTTPClient.h>         // Required
#include <ArduinoJson.h>        // Version 6 Required

OpenWeatherOneCall::OpenWeatherOneCall()
{

}


#define DS_URL1 "https://api.openweathermap.org/data/2.5/onecall"
#define DS_URL2 "&units="
#define DS_URL3 "&appid="

#define GEOLOCATIONURL "https://www.googleapis.com/geolocation/v1/geolocate"



String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

int OpenWeatherOneCall::getCoordinates(String googleKey)
{

  HTTPClient http;

  String jsonSend;    // This will be the json we send to Google with nearby MAC ADDRESSES

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


  deserializeJson(geo, http.getString());     // parsing the return from Google into geo Document

  // Variables in parse

  latitude = geo["location"]["lat"];
  longitude = geo["location"]["lng"];

  // int accuracy = geo["accuracy"]; // 37

  http.end();
  return 0;
} // End of Coordinates Routine

int OpenWeatherOneCall::parseWeather(char* DKEY, char* GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID)
{

  // Clear the struct for current weather
  memset(&current, 0, sizeof current);

  char getURL[200] = {0};

  // F/C/K units

  if (SET_UNITS)
  {
    strcpy(units, "metric");
  }
  else
    strcpy(units, "imperial");


  HTTPClient http;


  OpenWeatherOneCall::getCoordinates(GKEY);
  sprintf(getURL, "%s?lat=%.6f&lon=%.6f%s%s%s%s", DS_URL1, SEEK_LATITUDE, SEEK_LONGITUDE, DS_URL2, units, DS_URL3, DKEY);

  http.begin(getURL);
  int httpCode = http.GET();

  if (httpCode > 399)
  {
    return httpCode;
  }

  const size_t capacity = 56 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(8) + JSON_ARRAY_SIZE(48) + 14 * JSON_OBJECT_SIZE(1) + 66 * JSON_OBJECT_SIZE(4) + 9 * JSON_OBJECT_SIZE(6) + 35 * JSON_OBJECT_SIZE(10) + 13 * JSON_OBJECT_SIZE(11) + 4 * JSON_OBJECT_SIZE(13) + 4 * JSON_OBJECT_SIZE(14) + JSON_OBJECT_SIZE(16) + 9190;

  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, http.getString()); // <<---------------- - Streams JSON

#ifdef CURRENT
  current.timezone_offset = doc["timezone_offset"];

  JsonObject currently = doc["current"];
  current.dt = (long) currently["dt"] + current.timezone_offset;
  current.sunrise = (long) currently["sunrise"] + current.timezone_offset; // 1586773262
  current.sunset = (long) currently["sunset"] + current.timezone_offset; // 1586820773
  current.temp = currently["temp"]; // 287.59
  current.feels_like = currently["feels_like"]; // 281.42
  current.pressure = currently["pressure"]; // 1011
  current.humidity = currently["humidity"]; // 93
  current.dew_point = currently["dew_point"]; // 286.47
  current.clouds = currently["clouds"]; // 90
  current.uvi = currently["uvi"]; // 6.31
  current.visibility = currently["visibility"]; // 8047
  current.wind_speed = currently["wind_speed"]; // 10.3
  current.wind_gust = currently["wind_gust"]; // 14.4
  current.wind_deg = currently["wind_deg"]; // 170
  current.rain_1h = currently["rain"]["1h"]; // 14.4
  current.snow_1h = currently["snow"]["1h"]; // 14.4
  current.id = currently["weather"][0]["id"];  // 500
  current.main = currently["weather"][0]["main"];  // "rain"
  current.description = currently["weather"][0]["description"];  // "moderate rain"
#ifndef DARKSKY
  current.icon = currently["weather"][0]["icon"];
#endif
#ifdef DARKSKY
  // Legacy DarkSky compatibility
  current.dayTime = (long) currently["dt"] + current.timezone_offset; // 1586781931
  current.sunriseTime = (long) currently["sunrise"] + current.timezone_offset; // 1586773262
  current.sunsetTime = (long) currently["sunset"] + current.timezone_offset; // 1586820773
  current.temperature = currently["temp"]; // 287.59
  current.apparentTemperature = currently["feels_like"]; // 281.42
  current.cloudCover = currently["clouds"]; // 90
  current.uvIndex = currently["uvi"]; // 6.31
  current.windSpeed = currently["wind_speed"]; // 10.3
  current.windGust = currently["wind_gust"]; // 14.4
  current.windBearing = currently["wind_deg"]; // 170
  current.summary = currently["weather"][0]["description"];  // "moderate rain"
  current.icon = currently["weather"][0]["main"];
#endif
#endif

#ifdef MINUTELY
  // JsonObject minutely = minutely;
  JsonArray everyminute = doc["minutely"];

  //===================================
  // FOR loop to fill the minutely
  //===================================

  for (int x = 0; x < (sizeof(minutely) / sizeof(minutely[0])); x++)
  {
    minutely[x].dt = (long) everyminute[x]["dt"] + current.timezone_offset; // 1586793600
    minutely[x].precipitation = everyminute[x]["precipitation"]; // 1586793600
  }
  //*************************END OF FOR LOOP
  //****************************************
#endif

#ifdef HOURLY
  // JsonObject hourly = hourly;
  JsonArray everyhour = doc["hourly"];

  //===================================
  // FOR loop to fill the hourly
  //===================================

  for (int x = 0; x < (sizeof(hourly) / sizeof(hourly[0])); x++)
  {
    hourly[x].dt = (long) everyhour[x]["dt"] + current.timezone_offset; // 1586793600
    hourly[x].temp = everyhour[x]["temp"]; // 13.45
    hourly[x].feels_like = everyhour[x]["feels_like"]; // 13.45
    hourly[x].pressure = everyhour[x]["pressure"]; // 13.45
    hourly[x].humidity = everyhour[x]["humidity"]; // 13.45
    hourly[x].dew_point = everyhour[x]["dew_point"]; // 13.45
    hourly[x].clouds = everyhour[x]["clouds"]; // 13.45
    hourly[x].visibility = everyhour[x]["visibility"]; // 13.45
    hourly[x].wind_speed = everyhour[x]["wind_speed"]; // 13.45
    hourly[x].wind_gust = everyhour[x]["wind_gust"]; // 13.45
    hourly[x].wind_deg = everyhour[x]["wind_deg"]; // 13.45
    hourly[x].pop = everyhour[x]["pop"]; // 13.45
    hourly[x].rain_1h = everyhour[x]["rain"]["1h"]; // 13.45
    hourly[x].snow_1h = everyhour[x]["snow"]["1h"]; // 13.45
    hourly[x].wind_deg = everyhour[x]["wind_deg"]; // 13.45
    hourly[x].id = everyhour[x]["weather"][0]["id"]; // 500
    hourly[x].main = everyhour[x]["weather"][0]["main"]; // "rain"
    hourly[x].description = everyhour[x]["weather"][0]["description"]; // "moderate rain"
    hourly[x].icon = everyhour[x]["weather"][0]["icon"]; // "10d"
  }
  //*************************END OF FOR LOOP
  //****************************************
#endif

#ifdef DAILY
  // JsonObject daily_0 = daily[0];
  JsonArray daily = doc["daily"];

  //===================================
  // FOR loop to fill the 8 day struct
  //===================================

  for (int x = 0; x < (sizeof(forecast) / sizeof(forecast[0])); x++)
  {
    forecast[x].dt = (long) daily[x]["dt"] + current.timezone_offset; // 1586793600
    forecast[x].sunrise = (long) daily[x]["sunrise"] + current.timezone_offset; // 1586773262
    forecast[x].sunset = (long) daily[x]["sunset"] + current.timezone_offset; // 1586820773

    forecast[x].temp_morn = daily[x]["temp"]["morn"]; // 288.74
    forecast[x].temp_day = daily[x]["temp"]["day"]; // 288.74
    forecast[x].temp_eve = daily[x]["temp"]["eve"]; // 288.74
    forecast[x].temp_night = daily[x]["temp"]["night"]; // 286.56
    forecast[x].temp_min = daily[x]["temp"]["min"]; // 286.56
    forecast[x].temp_max = daily[x]["temp"]["max"]; // 293.23

    forecast[x].feels_like_morn = daily[x]["feels_like"]["morn"]; // 280.11
    forecast[x].feels_like_day = daily[x]["feels_like"]["day"]; // 280.11
    forecast[x].feels_like_eve = daily[x]["feels_like"]["eve"]; // 280.11
    forecast[x].feels_like_night = daily[x]["feels_like"]["night"]; // 280.29

    forecast[x].pressure = daily[x]["pressure"]; // 1006
    forecast[x].humidity = daily[x]["humidity"]; // 91
    forecast[x].dew_point = daily[x]["dew_point"]; // 287.28
    forecast[x].wind_speed = daily[x]["wind_speed"]; // 14.2
    forecast[x].wind_gust = daily[x]["wind_gust"]; // 14.2
    forecast[x].wind_deg = daily[x]["wind_deg"]; // 180
    forecast[x].clouds = daily[x]["clouds"]; // 95
    forecast[x].uvi = daily[x]["uvi"]; // 6.31
    forecast[x].visibility = daily[x]["visibility"]; // 10000
    forecast[x].pop = daily[x]["pop"]; // 0.57
    forecast[x].rain = daily[x]["rain"]; // 23.31
    forecast[x].snow = daily[x]["snow"]; // 23.31
    forecast[x].id = daily[x]["weather"][0]["id"];  // 500
    forecast[x].main = daily[x]["weather"][0]["main"];  // "rain"
    forecast[x].description = daily[x]["weather"][0]["description"];  // "moderate rain"
    forecast[x].icon = daily[x]["weather"][0]["icon"]; // "10d"
#ifdef DARKSKY
    // Legacy DarkSky compatibility
    forecast[x].dayTime = (long) daily[x]["dt"] + current.timezone_offset; // 1586793600
    forecast[x].sunriseTime = (long) daily[x]["sunrise"] + current.timezone_offset; // 1586773262
    forecast[x].sunsetTime = (long) daily[x]["sunset"] + current.timezone_offset; // 1586820773
    forecast[x].temperatureHigh = daily[x]["temp"]["day"]; // 288.74
    forecast[x].temperatureLow = daily[x]["temp"]["night"]; // 286.56
    forecast[x].temperatureMin = daily[x]["temp"]["min"]; // 286.56
    forecast[x].temperatureMax = daily[x]["temp"]["max"]; // 293.23
    forecast[x].apparentTemperatureHigh = daily[x]["feels_like"]["day"]; // 280.11
    forecast[x].apparentTemperatureLow = daily[x]["feels_like"]["night"]; // 280.29
    forecast[x].dewPoint = daily[x]["dew_point"]; // 287.28
    forecast[x].windSpeed = daily[x]["wind_speed"]; // 14.2
    forecast[x].windBearing = daily[x]["wind_deg"]; // 180
    forecast[x].cloudCover = daily[x]["clouds"]; // 95
    forecast[x].uvIndex = daily[x]["uvi"]; // 6.31
    forecast[x].precipType = daily[x]["weather"][0]["main"]; // "Rain"
    forecast[x].precipIntensity = daily[x]["rain"]; // 23.31
    forecast[x].summary = daily[x]["weather"][0]["description"];  // "moderate rain"
#endif
  }
  //*************************END OF FOR LOOP
  //****************************************
#endif

#ifdef ALERTS
  JsonObject alert = doc["alerts"];
  alerts.sender_name = alert["sender_name"]; // 1586781931
  alerts.event = alert["event"]; // 1586781931
  alerts.start = (long) alert["start"] + current.timezone_offset; // 1586781931
  alerts.end = (long) alert["end"] + current.timezone_offset; // 1586781931
  alerts.description = alert["description"]; // 1586781931
#endif

  http.end();
  return 0;
}
