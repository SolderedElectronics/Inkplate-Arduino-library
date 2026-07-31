/**
 **************************************************
 * @file        Inkplate2_OpenAI_Text_Prompt.ino
 * @brief       Fetch local weather, generate a short snarky summary with OpenAI,
 *              display it, then deep sleep to save power.
 *
 * @details     This example connects Inkplate 2 to WiFi, queries current weather
 *              data from the Open-Meteo API (temperature, weather code, time),
 *              and uses that information to build a prompt for the OpenAI Chat
 *              Completions API. The returned text (a short, witty summary) is
 *              rendered on the e-paper using drawTextBox(), then the ESP32 goes
 *              into deep sleep and wakes periodically to refresh the content.
 *
 *              Weather is fetched via HTTPS using WiFiClientSecure. The sketch
 *              parses the JSON response with ArduinoJson, maps Open-Meteo
 *              weather codes to human-readable descriptions, and then sends an
 *              HTTPS POST request to OpenAI with the generated prompt.
 *
 *              Deep sleep resets the ESP32 on every wake, so all logic runs in
 *              setup(). The display retains the last image while asleep and this
 *              example uses a full refresh (display()) in 1-bit (BW) mode.
 *
 *              Security warning: client.setInsecure() disables TLS certificate
 *              validation for both Open-Meteo and OpenAI requests. This is
 *              acceptable for demos but not recommended for production; use
 *              proper CA certificates or pinning. API usage and quotas apply
 *              (OpenAI), so protect your API key and avoid committing it to
 *              public repositories. ArduinoJson documents and full HTTP responses
 *              can be large, so keep prompts and expected responses short to
 *              reduce memory pressure.
 *
 *              Expected output: a short AI-generated weather summary rendered via
 *              drawTextBox(); on failure "Failed to get weather data." or an
 *              empty/failed response depending on API errors; connection logs and
 *              optional raw API responses in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access, OpenAI API key
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials (ssid, password).
 * 3) Set location, latitude and longitude for your area.
 * 4) Create an OpenAI API key and paste it into openai_key. Optionally adjust
 *    SLEEP_DURATION_IN_MINS and uS_TO_S_FACTOR.
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) On boot, the device fetches weather, requests a short AI summary, and
 *    displays it inside a text box.
 * 7) The device enters deep sleep and repeats the process after the configured
 *    sleep interval.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/


#include <WiFiClientSecure.h>     // Secure WiFi client for HTTPS communication
#include <ArduinoJson.h>          // Library for parsing and generating JSON
#include "Inkplate.h"             // Inkplate display driver
#include "FreeMonoBold18pt7b.h"   // Font used for rendering text on the display

// How long the device will stay in deep sleep (in minutes)
#define SLEEP_DURATION_IN_MINS 30*60 
#define uS_TO_S_FACTOR 1000000ULL

// WiFi credentials
const char* ssid = "";
const char* password = "";

// OpenAI API key
const char* openai_key = ""; 

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
Inkplate display;

void setup() {
  display.begin(); // Initialize the Inkplate display

  display.setTextColor(INKPLATE2_BLACK);

  Serial.begin(115200); // Initialize serial for debugging

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  // Get current weather data
  if (getWeather(latitude, longitude, temperature, weatherDesc, timeStr)) {
    // Build prompt to send to OpenAI
    String prompt = "Give me a sarcastic 15-word max summary of the weather in " + location +
                    ". It's currently " + String(temperature, 1) + "C with " + weatherDesc +
                    " skies at " + timeStr +
                    " (Just take the Hour and minutes, without AM or PM). Make it witty and slightly condescending. Dont use '—' and  ' symbols, use - and ' respectively";

    // Get AI-generated snarky response
    String snarkySummary = getOpenAIResponse(prompt);

    // Display the response on screen
    display.drawTextBox(12, 5, 200, 100, snarkySummary.c_str(), 1);
    display.display(); // Push the buffer to the screen
  } else {
    // Handle weather data fetch failure
    display.println("Failed to get weather data.");
    display.display();
  }

  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_IN_MINS * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here

     
  esp_deep_sleep_start();// Put ESP32 into deep sleep. Program stops here.
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