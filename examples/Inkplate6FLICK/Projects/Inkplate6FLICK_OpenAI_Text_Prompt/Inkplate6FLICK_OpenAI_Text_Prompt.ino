/*
  Inkplate6FLICK OpenAI text prompt generator
  Compatible with Soldered Inkplate 6FLICK

  For this example you will need only USB cable and Inkplate 6FLICK.
  Select "e-radionica Inkplate6FLICK" or "Soldered Inkplate6FLICK" from Tools -> Board menu.
  Don't have "e-radionica Inkplate6FLICK" or "Soldered Inkplate6FLICK" option? Follow our tutorial and add it:https://soldered.com/documentation/inkplate/6flick/quick-start-guide/

  Overview:
  This example demonstrates how to fetch the temperature and weather, then with that information it creates a snarky prompt which is displayed 
  on the Inkplate

  Before You Start:
  - Enter your WiFi credentials carefully (they are case-sensitive).
  - Update the following variables for accurate local weather data:
      • location
      • latitude
      • longitude
  - After creating an OpenAI API key, enter it in the openai_key variable
*/


#include <WiFiClientSecure.h>     // Secure WiFi client for HTTPS communication
#include <ArduinoJson.h>          // Library for parsing and generating JSON
#include "Inkplate.h"             // Inkplate display driver
#include "FreeMonoBold18pt7b.h"   // Font used for rendering text on the display

// How long the device will stay in deep sleep (in minutes)
#define SLEEP_DURATION_IN_MINS 30*60 

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// OpenAI API key
const char* openai_key = "YOUR_API_KEY_HERE"; 

// Location and coordinates for weather query
String location = "Osijek";
const float latitude = 45.5600;
const float longitude = 18.6750;

// Variables to hold weather data
float temperature;
String weatherDesc, timeStr;

// HTTPS client instance
WiFiClientSecure client;

// Inkplate display instance (1-bit mode for faster update and lower power usage)
Inkplate display(INKPLATE_1BIT);

void setup() {
  display.begin(); // Initialize the Inkplate display

  Serial.begin(115200); // Initialize serial for debugging

  // Connect to WiFi
  display.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.partialUpdate(); // Refresh only changed parts of display for faster performance
  }

  display.println("\nConnected!");
  display.display();         // Show connection message
  display.clearDisplay();    // Clear display for next content

  // Get current weather data
  if (getWeather(latitude, longitude, temperature, weatherDesc, timeStr)) {
    // Build prompt to send to OpenAI
    String prompt = "Give me a sarcastic 90-word max summary of the weather in " + location +
                    ". It's currently " + String(temperature, 1) + "C with " + weatherDesc +
                    " skies at " + timeStr +
                    " (Just take the Hour and minutes, without AM or PM). Make it witty and slightly condescending. Dont use '—' and  ' symbols, use - and ' respectively";

    // Get AI-generated snarky response
    String snarkySummary = getOpenAIResponse(prompt);

    // Display the response on screen
    display.drawTextBox(100, 200, 924, 658, snarkySummary.c_str(), 1, &FreeMonoBold18pt7b, NULL, false, 28);
    display.display(); // Push the buffer to the screen
  } else {
    // Handle weather data fetch failure
    display.println("Failed to get weather data.");
    display.display();
  }

  // Set a wakeup alarm 30 seconds from now (RTC-based wakeup)
  display.rtcSetAlarmEpoch(display.rtcGetEpoch() + SLEEP_DURATION_IN_MINS, RTC_ALARM_MATCH_DHHMMSS);

  // Configure ESP32 to wake up from deep sleep using RTC interrupt on GPIO 39
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

  // Enter deep sleep to save power
  esp_deep_sleep_start();
}

void loop() {
  // Empty loop – never runs due to deep sleep
}

/**
 * Fetch current weather from Open-Meteo API
 * 
 * @param lat Latitude of the location
 * @param lon Longitude of the location
 * @param temperature Output: current temperature
 * @param weatherDesc Output: human-readable weather description
 * @param timeStr Output: current time string
 * @return true if successful, false on error
 */
bool getWeather(float lat, float lon, float& temperature, String& weatherDesc, String& timeStr) {
  client.setInsecure(); // Skip TLS verification (not recommended for production)

  // Construct the API URL
  String url = "/v1/forecast?latitude=" + String(lat, 4) + "&longitude=" + String(lon, 4) +
               "&current_weather=true&timezone=auto";

  // Connect to Open-Meteo API server
  if (!client.connect("api.open-meteo.com", 443)) {
    Serial.println("Connection to Open-Meteo failed!");
    return false;
  }

  // Send HTTP GET request
  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: api.open-meteo.com");
  client.println("Connection: close");
  client.println();

  // Wait for response or timeout after 10s
  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 10000) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("Open-Meteo timeout");
    return false;
  }

  // Skip HTTP headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  // Read response body, ignoring chunked encoding markers
  String jsonPayload = "";
  while (client.available()) {
    String line = client.readStringUntil('\n');
    line.trim(); // Remove trailing whitespace and \r

    // Skip lines that are chunk size indicators in hex
    bool isHex = true;
    for (size_t i = 0; i < line.length(); i++) {
      if (!isHexadecimalDigit(line[i])) {
        isHex = false;
        break;
      }
    }
    if (isHex) continue;

    jsonPayload += line;
  }

  // Parse JSON payload
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, jsonPayload);

  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return false;
  }

  // Extract weather info
  temperature = doc["current_weather"]["temperature"];
  int weatherCode = doc["current_weather"]["weathercode"];
  timeStr = doc["current_weather"]["time"].as<String>();
  weatherDesc = weatherCodeToString(weatherCode);

  return true;
}

/**
 * Convert weather code from Open-Meteo into readable string
 * 
 * @param code Weather condition code
 * @return String description
 */
String weatherCodeToString(int code) {
  switch (code) {
    case 0: return "clear";
    case 1: return "mostly clear";
    case 2: return "partly cloudy";
    case 3: return "overcast";
    case 45: case 48: return "foggy";
    case 51: case 53: case 55: return "drizzling";
    case 61: case 63: case 65: return "raining";
    case 80: case 81: case 82: return "showery";
    default: return "weirdly unclassifiable"; // fallback for unknown codes
  }
}

/**
 * Send prompt to OpenAI's chat API and return the generated response
 * 
 * @param prompt The question or request to send to OpenAI
 * @return OpenAI's textual response, or empty string on error
 */
String getOpenAIResponse(String prompt) {
  client.setInsecure(); // Skip TLS cert verification

  // Connect to OpenAI API server
  if (!client.connect("api.openai.com", 443)) {
    Serial.println("Connection to OpenAI failed!");
    return "";
  }

  // Prepare JSON request body
  DynamicJsonDocument doc(1024);
  doc["model"] = "o3-mini"; // OpenAI chat model
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject message = messages.createNestedObject();
  message["role"] = "user";
  message["content"] = prompt;

  // Serialize request body to string
  String requestBody;
  serializeJson(doc, requestBody);

  // Send HTTP POST request
  Serial.println("Sending request to OpenAI...");
  client.println("POST /v1/chat/completions HTTP/1.1");
  client.println("Host: api.openai.com");
  client.println("Authorization: Bearer " + String(openai_key));
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(requestBody.length());
  client.println("Connection: close");
  client.println();
  client.println(requestBody);

  // Wait for response with timeout
  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 30000) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("No response from OpenAI - timeout");
    return "";
  }

  // Skip response headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  // Read and parse the JSON response body
  String response = client.readString();
  Serial.println("Raw response: " + response); // For debugging

  DynamicJsonDocument jsonDoc(2048);
  DeserializationError error = deserializeJson(jsonDoc, response);

  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return "";
  }

  // Check and extract reply content
  if (!jsonDoc.containsKey("choices")) {
    Serial.println("Unexpected response format");
    if (jsonDoc.containsKey("error")) {
      Serial.println("Error: " + jsonDoc["error"]["message"].as<String>());
    }
    return "";
  }

  return jsonDoc["choices"][0]["message"]["content"].as<String>();
}