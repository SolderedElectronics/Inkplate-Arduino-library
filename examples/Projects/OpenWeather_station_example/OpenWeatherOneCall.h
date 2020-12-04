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
//#define DARKSKY


class OpenWeatherOneCall
{
  public:
    //Constructor
    OpenWeatherOneCall();

    //Methods
    int parseWeather(char* DKEY, char* GKEY, float SEEK_LATITUDE, float SEEK_LONGITUDE, bool SET_UNITS, int CITY_ID);


#ifdef CURRENT
    //Variables
    struct nowData {
      long dt; // 1582151288
      long sunrise; // 1582112760
      long sunset; // 1582151880
      float temp; // 46.38
      float feels_like; // 41.49
      float pressure; // 1026.4
      float humidity; // 0.31
      float dewPoint; // 17.18
      float dew_point; // 17.18
      int clouds; // 97
      float uvi; // 0
      int visibility; // 10
      float wind_speed; // 10.22
      float wind_gust; // 10.22
      int wind_deg; // 348
      float rain_1h; // 3.18
      float snow_1h; // 3.18
      int id; // 201
      const char* main; // "moderate rain"
      const char* description; // "moderate rain"
      const char* icon; // "clear-day"
      int timezone_offset;
#ifdef DARKSKY
      // Not supported by OpenWeatherOneCall or legacy names
      long dayTime; // 1582151288
      long sunriseTime; // 1582112760
      long sunsetTime; // 1582151880
      float temperature; // 46.38
      float apparentTemperature; // 41.49
      float cloudCover; // 0.02
      float uvIndex; // 0
      float windSpeed; // 10.22
      float windGust; // 10.22
      int windBearing; // 348
      const char* summary; // "Clear"
      int precipIntensity; // 0
      int precipProbability; // 0
      float ozone; // 323.4
      int nearestStormDistance; // 50
      int nearestStormBearing; // 4
#endif
    } current ;
#endif

#ifdef MINUTELY
    struct minutelyData {
      long dt; // 1582088400
      float precipitation; // 0.0009
    } minutely[60];
#endif

#ifdef HOURLY
    struct hourlyData {
      long dt; // 1582088400
      float temp; // 46.38
      float feels_like; // 41.49
      float pressure; // 1026.4
      float humidity; // 0.31
      float dew_point; // 17.18
      int clouds; // 97
      int visibility; // 10
      float wind_speed; // 10.22
      float wind_gust; // 10.22
      int wind_deg; // 348
      float pop; // 0.52
      float rain_1h; // 3.18
      float snow_1h; // 3.18
      int id; // 348
      const char* main; // "moderate rain"
      const char* description; // "moderate rain"
      const char* icon; // "10d"
      // Not supported by OpenWeatherOneCall or legacy names
    } hourly[48];
#endif

#ifdef DAILY
    struct futureData {
      long dt; // 1582088400
      long sunrise; // 1582112760
      long sunset; // 1582151880

      float temp_morn; // 51.24
      float temp_day; // 51.24
      float temp_eve; // 51.24
      float temp_night; // 26.83
      float temp_min; // 30.17
      float temp_max; // 51.24

      float feels_like_morn; // 50.76
      float feels_like_day; // 50.76
      float feels_like_eve; // 50.76
      float feels_like_night; // 19.9

      float pressure; // 1024.1
      float humidity; // 0.54
      float dew_point; // 26.79
      float wind_speed; // 6.49
      float wind_gust; // 24.15
      int wind_deg; // 324
      int clouds; // 0.53
      float uvi; // 1.3
      int visibility; // 10
      float pop; // 0.57
      float rain; // 0.12
      float snow; // 5.14
      int id; // 201
      const char* main; // "moderate rain"
      const char* description; // "moderate rain"
      const char* icon; // "clear-day"
#ifdef DARKSKY
      // Not supported by OpenWeatherOneCall or legacy names
      long dayTime; // 1582151288
      long sunriseTime; // 1582112760
      long sunsetTime; // 1582151880
      float moonPhase; // 0.89
      float temperatureHigh; // 51.24
      long temperatureHighTime; // 1582139280
      float temperatureLow; // 26.83
      float temperatureMin; // 30.17
      long temperatureLowTime; // 1582199760
      float temperatureMax; // 51.24
      float apparentTemperatureHigh; // 50.76
      long apparentTemperatureHighTime; // 1582139100
      float apparentTemperatureLow; // 19.9
      long apparentTemperatureLowTime; // 1582199940
      float dewPoint; // 26.79
      float windSpeed; // 6.49
      float windGust; // 24.15
      long windGustTime; // 1582131600
      int windBearing; // 324
      float cloudCover; // 0.53
      float uvIndex; // 3
      long uvIndexTime; // 1.3
      const char* precipType; // "rain"
      float precipProbability; // 0.3
      float precipIntensity; // 0.0009
      float precipIntensityMax; // 0.0028
      long precipIntensityMaxTime; // 1582105560
      float ozone; // 322.6
      long temperatureMinTime; // 1582174800
      long temperatureMaxTime; // 1582139280
      float apparentTemperatureMin; // 24.39
      long apparentTemperatureMinTime; // 1582174800
      float apparentTemperatureMax; // 50.76
      long apparentTemperatureMaxTime; // 1582139100
      const char* summary; // "heavy intensity rain"
#endif
    } forecast[8];
#endif

#ifdef ALERTS
    struct alertData {
      const char* sender_name; // "NWS Tulsa (Eastern Oklahoma)"
      const char* event; // "Heat Advisory"
      long start; // 1582088400
      long end; // 1582088400
      const char* description; // "...HEAT ADVISORY REMAINS IN EFFECT FROM 1 PM THIS AFTERNOON TO\n8 PM CDT THIS EVENING...\n* WHAT...Heat index values of 105 to 109 degrees expected.\n* WHERE...Creek, Okfuskee, Okmulgee, McIntosh, Pittsburg,\nLatimer, Pushmataha, and Choctaw Counties.\n* WHEN...From 1 PM to 8 PM CDT Thursday.\n* IMPACTS...The combination of hot temperatures and high\nhumidity will combine to create a dangerous situation in which\nheat illnesses are possible."
    } alerts;
#endif

    String short_names[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    float latitude;
    float longitude;


  private:

    //Methods
    int getCoordinates(String googleKey);

    //Variables

    char units[10];

};

#endif
