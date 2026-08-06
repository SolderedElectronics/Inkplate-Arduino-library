/**
 **************************************************
 * @file        Inkplate6PLUS_OpenAI_Text_Prompt.ino
 * @brief       Fetch current weather over HTTPS, generate a witty summary via
 *              OpenAI, show it on Inkplate 6PLUS, then deep-sleep.
 *
 * @details     This example connects Inkplate 6PLUS to WiFi, downloads current
 *              weather data from the Open-Meteo API (temperature, weather code,
 *              timestamp), converts the weather code to a short description, and
 *              builds a prompt that asks OpenAI to produce a sarcastic, ~80-word
 *              weather summary for the chosen location.
 *
 *              The generated text is rendered to the e-paper display using a
 *              fixed font and drawTextBox() for wrapping. The display runs in
 *              1-bit (BW) mode (INKPLATE_1BIT) to keep refresh time and power
 *              usage low. During WiFi connection feedback, partial updates are
 *              used to update only the changed screen area, while the final
 *              content is shown with a full refresh.
 *
 *              After drawing the result, the sketch schedules an RTC alarm and
 *              enters ESP32 deep sleep. Deep sleep restarts the ESP32 on wake,
 *              so setup() runs again each cycle. RTC alarm scheduling uses the
 *              onboard RTC (PCF85063(A) on Inkplate) and ext0 wakeup (GPIO 39).
 *
 *              HTTPS security: this example calls client.setInsecure() for both
 *              Open-Meteo and OpenAI, which disables TLS certificate validation
 *              and is intended for demos only - for production, use proper
 *              certificate validation/pinning that matches the target hosts.
 *              Timeouts, rate limits and API key issues can cause empty or
 *              failed responses, and OpenAI usage may incur cost and is subject
 *              to account limits. Long replies may not fit well in the chosen
 *              text box and font, so the prompt asks for ~80 words to keep
 *              rendering predictable.
 *
 *              Expected output: a text block with an AI-generated "snarky"
 *              weather summary on the display, plus connection and JSON
 *              parsing/debug messages and the raw OpenAI response on the Serial
 *              Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access, internet connection, OpenAI API key
 * - Serial:     115200 baud (optional; used for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password.
 * 3) Set the location name plus latitude/longitude for your area.
 * 4) Create an OpenAI API key and paste it into openai_key (keep it private).
 *    Adjust SLEEP_DURATION_IN_MINS (RTC alarm interval) if needed.
 * 5) Upload the sketch and open Serial Monitor (optional) for debug logs.
 * 6) After boot, the device fetches weather, requests a short snarky summary
 *    from OpenAI, displays it, then sleeps until the next RTC alarm.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
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
    String prompt = "Give me a sarcastic 80-word max summary of the weather in " + location +
                    ". It's currently " + String(temperature, 1) + "C with " + weatherDesc +
                    " skies at " + timeStr +
                    " (Just take the Hour and minutes, without AM or PM). Make it witty and slightly condescending. Dont use '—' and  ' symbols, use - and ' respectively";

    // Get AI-generated snarky response
    String snarkySummary = getOpenAIResponse(prompt);

    // Display the response on screen
    display.drawTextBox(100, 200, 924, 658, snarkySummary.c_str(), 1, &FreeMonoBold18pt7b, 38, false, 28);
    display.display(); // Push the buffer to the screen
  } else {
    // Handle weather data fetch failure
    display.println("Failed to get weather data.");
    display.display();
  }

  // Set a wakeup alarm 30 seconds from now (RTC-based wakeup)
  display.rtc.setAlarmEpoch(display.rtc.getEpoch() + SLEEP_DURATION_IN_MINS, RTC_ALARM_MATCH_DHHMMSS);

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