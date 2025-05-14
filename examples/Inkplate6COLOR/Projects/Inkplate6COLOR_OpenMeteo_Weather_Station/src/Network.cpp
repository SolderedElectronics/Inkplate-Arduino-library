#include "Network.h"
#include <WiFi.h>          
#include <HTTPClient.h>     
#include "WeatherData.h"     

#include <ArduinoJson.h>     

#include <Inkplate.h>


// Function to get weather description based on the weather code
String Network::getWeatherDescription(int code) {
  switch (code) {
      case 0: return "Clear sky";
      case 1: case 2: case 3: return "Mainly clear, partly cloudy";
      case 45: case 48: return "Fog";
      case 51: case 53: case 55: return "Drizzle";
      case 56: case 57: return "Freezing Drizzle";
      case 61: case 63: case 65: return "Rain";
      case 66: case 67: return "Freezing Rain";
      case 71: case 73: case 75: return "Snowfall";
      case 77: return "Snow grains";
      case 80: case 81: case 82: return "Rain showers";
      case 85: case 86: return "Snow showers";
      case 95: return "Thunderstorm";
      case 96: case 99: return "Thunderstorm, hail";
      default: return "Unknown condition";
  }
}

String Network::extractDate(String dateTime) {
  int spaceIndex = dateTime.indexOf(' ');
  if (spaceIndex != -1) {
      return dateTime.substring(0, spaceIndex);  // e.g., "2025-04-08"
  }
  return "????-??-??";
}


String Network::extractTime(String dateTime) {
  int tIndex = dateTime.indexOf(' ');
  if (tIndex != -1 && tIndex + 5 < dateTime.length()) {
      return dateTime.substring(tIndex + 1, tIndex + 6);  // e.g., "06:11"
  }
  return "??:??";
}

String Network::getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
      return "Time N/A";
  }

  char timeStr[20];
  snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d",
           timeinfo.tm_year + 1900,
           timeinfo.tm_mon + 1,
           timeinfo.tm_mday,
           timeinfo.tm_hour,
           timeinfo.tm_min);
  return String(timeStr);
}

int Network::getCurrentHour() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
      return -1;  // Return -1 if time is not available (error)
  }

  return timeinfo.tm_hour;
}

String Network::extractSun(String dateTime) {
  int tIndex = dateTime.indexOf('T');
  if (tIndex != -1 && tIndex + 5 < dateTime.length()) {
      return dateTime.substring(tIndex + 1, tIndex + 6);  // e.g., "06:11"
  }
  return "??:??";
}

// Function to get the current day of the week and return the next 7 days dynamically
String Network::getDayName(int dayIndex) {
  // Array of days, starting from Sunday (index 0)
  String daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  
  // Get the current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
      return "Error";  // Return error if time can't be fetched
  }

  // Get the current day index (0 = Sunday, 1 = Monday, etc.)
  int currentDay = timeinfo.tm_wday;

  // Calculate the day of the week for the given index
  int dayOfWeekIndex = (currentDay + dayIndex) % 7;

  return daysOfWeek[dayOfWeekIndex];
}

// Function to fetch weather data from Open-Meteo API
void Network::fetchWeatherData(WeatherData* weatherData, UserInfo* userInfo, const float* latitude, const float* longitude) {
    // Construct URL for the Open-Meteo API request
    String units = "&temperature_unit=fahrenheit";

    String urlMetric = "https://api.open-meteo.com/v1/forecast?latitude=" + String(*latitude) + "&longitude=" + String(*longitude) +
                 "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,wind_speed_10m_max,wind_direction_10m_dominant,precipitation_probability_max,weather_code,uv_index_max" +
                 "&hourly=temperature_2m,precipitation_probability" +
                 "&current=temperature_2m,precipitation,wind_speed_10m,weather_code,apparent_temperature,relative_humidity_2m,is_day" +
                 "&timezone=auto";
    
    String urlImperial = "https://api.open-meteo.com/v1/forecast?latitude=" + String(*latitude) + "&longitude=" + String(*longitude) +
                 "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,wind_speed_10m_max,wind_direction_10m_dominant,precipitation_probability_max,weather_code,uv_index_max" +
                 "&hourly=temperature_2m,precipitation_probability" +
                 "&current=temperature_2m,precipitation,wind_speed_10m,weather_code,apparent_temperature,relative_humidity_2m,is_day" +
                 "&timezone=auto" +
                 "&temperature_unit=fahrenheit&wind_speed_unit=mph&precipitation_unit=inch";  // standard api settings use C, km/h, milimeter
    
    HTTPClient http;

    if (userInfo->useMetric == true){
      userInfo->temperatureLabel = " C";
      userInfo->speedLabel = " km/h";
      http.begin(urlMetric);
    }
    else{
      userInfo->temperatureLabel = " F";
      userInfo->speedLabel = " mph";
      http.begin(urlImperial);
    }
        
    int httpCode = http.GET();  // Send the GET request
    
    if (httpCode > 0) {  // Check if the request was successful
        String payload = http.getString();  // Get the response as a string
        Serial.println(payload);  // Print the full response for debugging

        userInfo->apiError = false;

        // Parse the JSON response
        DynamicJsonDocument doc(2048);  // Create a JSON document with a buffer size of 2048 bytes
        deserializeJson(doc, payload);  // Deserialize the JSON payload

        // UserInfo Data
        userInfo->lastUpdated = getFormattedTime();
        userInfo->currentHour = getCurrentHour();
        userInfo->lastUpdatedDate = extractDate(userInfo->lastUpdated);
        userInfo->lastUpdatedTime = extractTime(userInfo->lastUpdated);

        // Store the parsed data into the WeatherData structure
        weatherData->currentTemp = doc["current"]["temperature_2m"].as<float>();   
        weatherData->feelsLike = doc["current"]["apparent_temperature"].as<float>();  
        weatherData->sunrise = extractSun(doc["daily"]["sunrise"][0].as<String>());
        weatherData->sunset = extractSun(doc["daily"]["sunset"][0].as<String>());
        weatherData->uvIndex = doc["daily"]["uv_index_max"][0].as<float>();   
        weatherData->precipitation = doc["current"]["precipitation"].as<float>();  
        weatherData->windSpeed = doc["current"]["wind_speed_10m"].as<float>();
        weatherData->isDay = doc["current"]["is_day"].as<bool>();
        weatherData->weatherCode = doc["current"]["weather_code"].as<int>(); 
        weatherData->weatherDescription = getWeatherDescription(weatherData->weatherCode);

        // Fetch daily forecast for the next 7 days
        for (int i = 0; i < 7; i++) {
          weatherData->dailyMinTemp[i] = doc["daily"]["temperature_2m_min"][i].as<float>();  
          weatherData->dailyMaxTemp[i] = doc["daily"]["temperature_2m_max"][i].as<float>();
          weatherData->dailyWeatherCodes[i] = doc["daily"]["weather_code"][i].as<int>();  
          weatherData->dailyNames[i] = getDayName(i);  // Get day name dynamically
        }

        
        // Fetch data for the graph
        for (int i = 0; i < 6; i++) {
          int sourceIndex = userInfo->currentHour + i;
          weatherData->hourlyTemps[i] = doc["hourly"]["temperature_2m"][sourceIndex].as<float>();
          weatherData->hourlyPrecip[i] = doc["hourly"]["precipitation_probability"][sourceIndex].as<float>();
          weatherData->hourlyTimes[i] = extractSun(doc["hourly"]["time"][sourceIndex].as<String>());
        }

    } else {
        Serial.println("Error on HTTP request");
        userInfo->apiError = true;
    }

    http.end();  // End the HTTP request
}