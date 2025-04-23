#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "WeatherData.h"
#include <Inkplate.h>
#include <Arduino.h>

class Network {
public:

    // --- Struct UserInfo ---
    struct UserInfo {
      String lastUpdated;
      int batteryLevel;
      double voltage;
      int currentHour;
      String city;
      String username;
      String lastUpdatedDate;
      String lastUpdatedTime;
      bool apiError;
      bool useMetric;
      String temperatureLabel;
      String speedLabel;
    };

    // --- Public methods ---
    void fetchWeatherData(WeatherData* weatherData, UserInfo* userInfo, const float* latitude, const float* longitude);

private:
    // --- Private helper methods ---
    String getWeatherDescription(int code);
    String extractDate(String dateTime);
    String extractTime(String dateTime);
    String extractSun(String dateTime);
    int voltageToPercentage(double voltage);
    String getFormattedTime();
    int getCurrentHour();
    String getDayName(int dayIndex);
};

#endif
