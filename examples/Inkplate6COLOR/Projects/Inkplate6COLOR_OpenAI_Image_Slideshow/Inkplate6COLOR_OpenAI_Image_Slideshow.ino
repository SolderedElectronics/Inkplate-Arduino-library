/**
 **************************************************
 * @file        Inkplate6COLOR_OpenAI_Image_Slideshow.ino
 * @brief       Generates an image from an OpenAI prompt (DALL-E), downloads it,
 *              shows it on Inkplate 6COLOR, then deep-sleeps between updates.
 *
 * @details     This example demonstrates an Internet-connected image slideshow
 *              powered by OpenAI image generation. On boot, Inkplate 6COLOR connects
 *              to WiFi, sends a JSON request to the OpenAI Images API using a
 *              text prompt, parses the returned JSON to extract the generated
 *              image URL, and then downloads and renders that image on the
 *              e-paper display.
 *
 *              Status messages ("Connecting...", "Generating prompt...", etc.)
 *              are shown using partial updates in 1-bit mode for speed. After
 *              an image URL is obtained, the sketch switches the display to
 *              3-bit grayscale (INKPLATE_3BIT) for better image quality and
 *              performs a full refresh after drawing the downloaded image.
 *              Partial updates are not supported in grayscale mode.
 *
 *              The sketch schedules the next wake-up using the on-board RTC
 *              (PCF85063(A)) by setting an alarm epoch time, then enters deep
 *              sleep. Deep sleep resets the ESP32, so the workflow repeats from
 *              setup() every time the device wakes and no state is preserved.
 *              Wake-up is configured via RTC alarm epoch and an external wake on
 *              GPIO 39 (typically tied to the RTC interrupt line), so make sure
 *              your hardware revision/wiring matches the expected wake behavior.
 *
 *              HTTPS security: this sketch uses client.setInsecure(), which
 *              disables TLS certificate validation. This is for demonstration
 *              only; for production use, validate certificates or pin the
 *              correct certificate chain/host. Downloading and decoding large
 *              PNGs can be slow and memory intensive - if decoding fails, reduce
 *              the image size or use a simpler format.
 *
 *              Expected output: short status messages on the display via partial
 *              updates during startup, the downloaded image rendered in 3-bit
 *              grayscale after generation, and the OpenAI response body plus
 *              resolved image URL on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable (battery optional)
 * - Extra:      WiFi Internet connection, OpenAI API key
 * - Library:    ArduinoJson (Arduino Library Manager)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6COLOR"
 *    from Tools -> Board.
 * 2) Create an OpenAI API key and paste it into openai_key.
 * 3) Enter your WiFi SSID and password (ssid, password).
 * 4) Optionally change imagePrompt, the image size/model settings in the
 *    request JSON, and the sleep interval (SLEEP_DURATION_IN_MINS).
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) The device connects, requests an image, downloads it, renders it, then
 *    deep-sleeps and wakes periodically to generate the next image.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
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
Inkplate display;

// Set the image prompt to send to OpenAI
String imagePrompt = "Generate a cyberpunk city with a lot of vertical layers";

void setup() {
  display.begin(); // Initialize the Inkplate hardware and begin communication

  Serial.begin(115200); // Start serial communication for debugging

  display.setTextSize(3); // Set text size for on-screen messages

  // Display message while attempting to connect to WiFi
  display.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait and retry
    Serial.print("."); // Show progress
  }

  // WiFi connection successful
  Serial.println("\nConnected!");
  display.display();         // Push display buffer to screen
  display.clearDisplay();    // Clear the screen for next display content

  // Notify the user on the display
  Serial.println("Generating prompt...");

  // Call function to get image URL from OpenAI based on the prompt
  String imageUrl = getImageUrlFromPrompt(imagePrompt);

  Serial.println("Prompt generated!");


  display.setTextColor(BLACK);
  display.println("Downloading and displaying image (May take a while...)");
  display.display();          // Show status
  display.clearDisplay();     // Clear screen before showing the image

  if (imageUrl != "") {
    Serial.println("Image URL: " + imageUrl);
    
    // Draw the image centered on the screen
    // Image assumed to be 1024x1024; offset to center it
    bool result = display.image.draw(imageUrl, imageFormat, display.width()/2 - 512, display.height()/2 - 512, true, false);
    
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
  display.rtc.setAlarmEpoch(display.rtc.getEpoch() + SLEEP_DURATION_IN_MINS, RTC_ALARM_MATCH_DHHMMSS);

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
  doc["model"] = "dall-e-3"; //GPT model we are using, delete this line to use DALL-E 2
  doc["prompt"] = prompt;
  doc["style"] = "vivid"; 
  doc["n"] = 1;
  doc["size"] = "1024x1024";  // or 1792x1024, 1024x1792

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
