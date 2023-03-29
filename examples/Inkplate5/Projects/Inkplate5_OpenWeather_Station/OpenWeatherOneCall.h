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

#include "Arduino.h"
// Include additional weather data
#define CURRENT
#define MINUTELY
#define HOURLY
#define DAILY
#define ALERTS

class OpenWeatherOneCall
{
  public:
    // Constructor
    OpenWeatherOneCall();

    // Methods
    int parseWeather(char *DKEY, char *GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID);

#ifdef CURRENT
    // Variables
    struct nowData
    {
        long dt;                 // 1582151288
        long sunrise;            // 1582112760
        long sunset;             // 1582151880
        float temp;              // 46.38
        float feels_like;        // 41.49
        float pressure;          // 1026.4
        float humidity;          // 0.31
        float dewPoint;          // 17.18
        float dew_point;         // 17.18
        int clouds;              // 97
        float uvi;               // 0
        int visibility;          // 10
        float wind_speed;        // 10.22
        float wind_gust;         // 10.22
        int wind_deg;            // 348
        float rain_1h;           // 3.18
        float snow_1h;           // 3.18
        int id;                  // 201
        const char *main;        // "moderate rain"
        const char *description; // "moderate rain"
        char icon[6];            // "01d"
        int timezone_offset;
    } current;
#endif

#ifdef MINUTELY
    struct minutelyData
    {
        long dt;             // 1582088400
        float precipitation; // 0.0009
    } minutely[60];
#endif

#ifdef HOURLY
    struct hourlyData
    {
        long dt;                 // 1582088400
        float temp;              // 46.38
        float feels_like;        // 41.49
        float pressure;          // 1026.4
        float humidity;          // 0.31
        float dew_point;         // 17.18
        int clouds;              // 97
        int visibility;          // 10
        float wind_speed;        // 10.22
        float wind_gust;         // 10.22
        int wind_deg;            // 348
        float pop;               // 0.52
        float rain_1h;           // 3.18
        float snow_1h;           // 3.18
        int id;                  // 348
        const char *main;        // "moderate rain"
        const char *description; // "moderate rain"
        char icon[6];            // "10d"
                                 // Not supported by OpenWeatherOneCall or legacy names
    } hourly[48];
#endif

#ifdef DAILY
    struct futureData
    {
        long dt;      // 1582088400
        long sunrise; // 1582112760
        long sunset;  // 1582151880

        float temp_morn;  // 51.24
        float temp_day;   // 51.24
        float temp_eve;   // 51.24
        float temp_night; // 26.83
        float temp_min;   // 30.17
        float temp_max;   // 51.24

        float feels_like_morn;  // 50.76
        float feels_like_day;   // 50.76
        float feels_like_eve;   // 50.76
        float feels_like_night; // 19.9

        float pressure;          // 1024.1
        float humidity;          // 0.54
        float dew_point;         // 26.79
        float wind_speed;        // 6.49
        float wind_gust;         // 24.15
        int wind_deg;            // 324
        int clouds;              // 0.53
        float uvi;               // 1.3
        int visibility;          // 10
        float pop;               // 0.57
        float rain;              // 0.12
        float snow;              // 5.14
        int id;                  // 201
        const char *main;        // "moderate rain"
        const char *description; // "moderate rain"
        char icon[6];            // "01d"
    } forecast[7];
#endif

#ifdef ALERTS
    struct alertData
    {
        const char *sender_name; // "NWS Tulsa (Eastern Oklahoma)"
        const char *event;       // "Heat Advisory"
        long start;              // 1582088400
        long end;                // 1582088400
        const char *
            description; // "...HEAT ADVISORY REMAINS IN EFFECT FROM 1 PM THIS AFTERNOON TO\n8 PM CDT THIS EVENING...\n*
                         // WHAT...Heat index values of 105 to 109 degrees expected.\n* WHERE...Creek, Okfuskee,
                         // Okmulgee, McIntosh, Pittsburg,\nLatimer, Pushmataha, and Choctaw Counties.\n* WHEN...From 1
                         // PM to 8 PM CDT Thursday.\n* IMPACTS...The combination of hot temperatures and high\nhumidity
                         // will combine to create a dangerous situation in which\nheat illnesses are possible."
    } alerts;
#endif

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
