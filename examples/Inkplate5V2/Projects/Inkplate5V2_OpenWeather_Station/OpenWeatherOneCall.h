/*
   OpenWeatherOnecall.h
   Minor Upgrade v1.11 (Added new parameter for City ID future usage)

   copyright 2020 - Jessica Hershey
   www.github.com/jHershey69
   7 day and current weather forecast based on current location
   NO GPS is required, uses WiFi triangulation to get location
   Modified by Jon Fuge to get additional information
*/

#ifndef DS
#define DS

// Include needed libraries
#include "Arduino.h"
#include "ArduinoJson.h" // Version 6 Required
#include "HTTPClient.h" 

class OpenWeatherOneCall
{
  public:
    // Constructor
    OpenWeatherOneCall();

    // Methods
    int parseWeather(char *DKEY, char *GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID);

    // Current weather
    struct nowData
    {
        long dt;        // 1582151288
        long sunrise;   // 1582112760
        long sunset;    // 1582151880
        float temp;     // 46.38
        float pressure; // 1026.4
        float humidity; // 0.31
        char icon[6];   // "01d"
        int timezone_offset;
    } current;

    // Hourly weather
    struct hourlyData
    {
        long dt;       // 1582088400
        float temp;    // 46.38
        float rain_1h; // 3.18
        float snow_1h; // 3.18
        char icon[6];  // "10d"
                       // Not supported by OpenWeatherOneCall or legacy names
    } hourly[48];

    // Daily weather for the next 4 days, first is current so we need 4
    struct futureData
    {
        long dt;        // 1582088400
        long sunrise;   // 1582112760
        long sunset;    // 1582151880
        float temp_min; // 30.17
        float temp_max; // 51.24
        float pressure; // 1024.1
        float humidity; // 0.54
        float pop;      // 0.57
        float rain;     // 0.12
        float snow;     // 5.14
        char icon[6];   // "01d"
    } forecast[4];

    String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    float latitude;
    float longitude;

  private:
    // Methods
    int getCoordinates(String googleKey);

    // Variables
    char units[10];
};

#endif
