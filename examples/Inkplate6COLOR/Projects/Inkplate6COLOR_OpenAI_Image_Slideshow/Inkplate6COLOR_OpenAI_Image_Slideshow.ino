/*
  Inkplate6COLOR OpenAI Image slideshow
  Compatible with Soldered Inkplate 6COLOR

  For this example you will need only USB cable and Inkplate 6COLOR.
  Select "Soldered Inkplate6COLOR" from Tools -> Board menu.
  Don't have "Soldered Inkplate6COLOR" option? Follow our tutorial and add it:https://soldered.com/documentation/inkplate/6color/quick-start-guide/

  Overview:
  This example demonstrates how to send a prompt to the GPT DALL-E API in order to generate an image which will be displayed on the Inkplate
  It generates an image every 30mins, after which it goes to deep sleep.

  Before You Start:
  - Enter your WiFi credentials carefully (they are case-sensitive).
  - After creating an OpenAI API key, enter it in the openai_key variable
*/

#include <WiFiClientSecure.h>     // Enables secure (HTTPS) communication over WiFi
#include <ArduinoJson.h>          // Library for JSON parsing and creation
#include "Inkplate.h"             // Driver for the Inkplate e-paper display

// Time in minutes the device will sleep between updates (30 mins in this case)
#define SLEEP_DURATION_IN_MINS 30*60 

// Format of the image to be downloaded and displayed (PNG is recommended for best compatibility)
Image::Format imageFormat = Image::PNG;

// WiFi credentials (replace with your own SSID and password)
const char* ssid = "YOUR_SSID_HERE";
const char* password = "YOUR_PASSWORD_HERE";

// OpenAI API key (replace with your OpenAI API key)
const char* openai_key = "YOUR_API_KEY_HERE"; 


// Create a secure WiFi client to communicate with OpenAI over HTTPS
WiFiClientSecure client;

// Create an Inkplate display object (1-bit mode = faster, lower-power updates)
Inkplate display;

// Set the image prompt to send to OpenAI
String imagePrompt = "Generate a lot of flowers with blue, red, yellow, orange and green colors. They should be on a green hill with a clear blue sky in the background.";

void setup() {
  display.begin(); // Initialize the Inkplate hardware and begin communication

  Serial.begin(115200); // Start serial communication for debugging

  display.setTextSize(2); // Set text size for on-screen messages

  // Display message while attempting to connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait and retry
    Serial.print("."); // Show progress
  }

  // WiFi connection successful
  Serial.println("\nConnected!");

  // Notify the user on the display
  Serial.println("Generating prompt...");

  // Call function to get image URL from OpenAI based on the prompt
  String imageUrl = getImageUrlFromPrompt(imagePrompt);

  Serial.println("Prompt generated!");

  display.setTextColor(BLACK);
  Serial.println("Downloading and displaying image (May take a while...)");
  display.clearDisplay();     // Clear screen before showing the image

  if (imageUrl != "") {
    Serial.println("Image URL: " + imageUrl);
    
    // Draw the image centered on the screen
    // Image assumed to be 512x512; offset to center it
    bool result = display.drawImage(imageUrl, imageFormat, display.width()/2 - 256, display.height()/2 - 256, true, false);
    
    if(result == 0) {
      // Show an error if image decoding fails
      display.println("Image decode error.");
    }
    
    display.display(); // Push image to display
  }
  else {
    // Show an error message if the image URL wasn't retrieved
    display.println("Failed to get image URL.");
    display.display();
  }

  // Schedule the next wakeup time using the real-time clock
  display.rtcSetAlarmEpoch(display.rtcGetEpoch() + SLEEP_DURATION_IN_MINS, RTC_ALARM_MATCH_DHHMMSS);

  // Enable external wakeup on GPIO 39 (typically tied to RTC alarm)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

  // Enter deep sleep mode to conserve power
  esp_deep_sleep_start();
}

void loop() {
  // This loop will never run because device goes into deep sleep after setup()
}

// Sends the image prompt to OpenAI's DALL·E endpoint and extracts the image URL
String getImageUrlFromPrompt(String prompt) 
{
  client.setInsecure(); // Disable certificate validation (OK for dev/test, not secure for prod)

  // Attempt to connect to OpenAI's server
  if (!client.connect("api.openai.com", 443)) {
    Serial.println("Connection to OpenAI for image failed!");
    return "";
  }

  // Create JSON request body
  DynamicJsonDocument doc(1024);
  doc["prompt"] = prompt;
  doc["n"] = 1;
  doc["size"] = "512x512"; 

  String requestBody;
  serializeJson(doc, requestBody); // Convert JSON object to string

  // Compose and send HTTPS POST request
  client.println("POST /v1/images/generations HTTP/1.1");
  client.println("Host: api.openai.com");
  client.println("Authorization: Bearer " + String(openai_key)); // Add API key to header
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(requestBody.length());
  client.println("Connection: close");
  client.println();
  client.println(requestBody);

  // Wait up to 40 seconds for a response
  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 40000) {
    delay(100);
  }

  // Timeout handling
  if (!client.available()) {
    Serial.println("OpenAI image request timed out.");
    return "";
  }

  // Skip over HTTP response headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // End of headers
  }

  // Read the HTTP response body
  String response = client.readString();
  Serial.println("Response:");
  Serial.println(response);

  // Parse JSON response to extract image URL
  DynamicJsonDocument resDoc(4096);
  DeserializationError err = deserializeJson(resDoc, response);
  if (err) {
    Serial.print("Image JSON parse failed: ");
    Serial.println(err.c_str());
    return "";
  }

  // Extract and return the first image URL from the response
  if (resDoc["data"] && resDoc["data"][0]["url"]) {
    String url = resDoc["data"][0]["url"].as<String>();
    return url;
  }

  Serial.println("No image URL found in OpenAI response");
  return ""; // Return empty string if no image URL was found
}
