/**
 **************************************************
 * @file        Inkplate13SPECTRA_OpenAI_Image_Slideshow.ino
 * @brief       OpenAI (DALL-E) generated image slideshow for Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Demonstrates how to send a prompt to the GPT DALL-E API in order
 *              to generate an image which will be displayed on the Inkplate. It
 *              generates an image every 30 minutes, after which it goes to deep
 *              sleep.
 *
 *              Before you start:
 *              - Enter your WiFi credentials carefully (they are case-sensitive).
 *              - After creating an OpenAI API key, enter it in the openai_key
 *                variable.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      WiFi Internet connection, OpenAI API key
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials and OpenAI API key (openai_key) in the sketch.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) An image is generated, downloaded and shown on the display, then the board
 *    deep-sleeps for 30 minutes and repeats.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 **************************************************/

#include <WiFiClientSecure.h>     // Enables secure (HTTPS) communication over WiFi
#include <ArduinoJson.h>          // Library for JSON parsing and creation
#include "Inkplate.h"             // Driver for the Inkplate e-paper display

// Time in minutes the device will sleep between updates (30 mins in this case)
#define SLEEP_DURATION_IN_MINS 30*60 

// Format of the image to be downloaded and displayed (PNG is recommended for best compatibility)
ImageColor::Format imageFormat = ImageColor::PNG;

// WiFi credentials (replace with your own SSID and password)
const char* ssid = "";
const char* password = "";

// OpenAI API key (replace with your OpenAI API key)
const char* openai_key = ""; 

// Create a secure WiFi client to communicate with OpenAI over HTTPS
WiFiClientSecure client;

// Create an Inkplate display object (1-bit mode = faster, lower-power updates)
Inkplate inkplate;

// Set the image prompt to send to OpenAI
String imagePrompt = "Generate an image at 1024x1024 resolution with a lot of flowers including blue, red, yellow, orange and green colors. They should be on a green hill with a clear blue sky in the background.";

void setup() 
{
  inkplate.begin(); // Initialize the Inkplate hardware and begin communication

  Serial.begin(115200); // Start serial communication for debugging

  inkplate.setTextSize(2); // Set text size for on-screen messages

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

  inkplate.setTextColor(BLACK);
  Serial.println("Downloading and displaying image (May take a while...)");
  inkplate.clearDisplay();     // Clear screen before showing the image

  if (imageUrl != "") 
  {
    Serial.println("Image URL: " + imageUrl);

    // Draw the image centered on the screen
    bool result = inkplate.image.draw(imageUrl, imageFormat, 0, 0, true, false);
    
    if (result == 0) 
    {
      // Show an error if image decoding fails
      inkplate.println("Image decode error.");
    }
    
    inkplate.display(); // Push image to display
  }
  else 
  {
    // Show an error message if the image URL wasn't retrieved
    inkplate.println("Failed to get image URL.");
    inkplate.display();
  }

  // Schedule the next wakeup time using the real-time clock
  inkplate.rtc.setAlarmEpoch(inkplate.rtc.getEpoch() + SLEEP_DURATION_IN_MINS, RTC_ALARM_MATCH_DHHMMSS);

  // Enable external wakeup on GPIO 18 (typically tied to RTC alarm)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, 0);

  // Enter deep sleep mode to conserve power
  esp_deep_sleep_start();
}

void loop() 
{
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
  doc["size"] = "1024x1024"; 

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
  if (!client.available()) 
  {
    Serial.println("OpenAI image request timed out.");
    return "";
  }

  // Skip over HTTP response headers
  while (client.connected()) 
  {
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
  if (err) 
  {
    Serial.print("Image JSON parse failed: ");
    Serial.println(err.c_str());
    return "";
  }

  // Extract and return the first image URL from the response
  if (resDoc["data"] && resDoc["data"][0]["url"]) 
  {
    String url = resDoc["data"][0]["url"].as<String>();
    return url;
  }

  Serial.println("No image URL found in OpenAI response");
  return ""; // Return empty string if no image URL was found
}