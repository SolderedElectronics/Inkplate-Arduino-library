/**
 **************************************************
 * @file        Inkplate13SPECTRA_Mapbox_API.ino
 * @brief       Mapbox API map display example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how you can use Inkplate 13SPECTRA to display map data.
 *              The example gets HTML data from a crowdsource campaign and
 *              displays it on the Inkplate screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      WiFi Internet connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials in the sketch.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The map data is fetched and shown on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

// Fill in these using api key from https://www.mapbox.com/ and coordinates you want to draw
// http://bboxfinder.com/ might help you :)
#define API_KEY ""
#define LAT1 18.679247
#define LON1 45.543870
#define LAT2 18.715210
#define LON2 45.562021

// Refresh time
#define DELAY_MS 5 * 60 * 1000

Inkplate inkplate;

void setup()
{
  Serial.begin(115200);   // Initialize serial
  inkplate.begin();       // Initialize display

  // Connect to WiFi
  while (!inkplate.connectWiFi(ssid, pass))
  {
    Serial.println("Connecting to wifi");
  }

  // Format URL string
  char URL[256];
  sprintf(URL, "https://api.mapbox.com/styles/v1/mapbox/navigation-day-v1/static/[%lf,%lf,%lf,%lf]/600x448?access_token=%s", LAT1, LON1, LAT2, LON2, API_KEY);

  Serial.println("Fetching:");
  Serial.println(URL);

  // Fetch and display image
  inkplate.image.draw(URL, 0, 0, true, false);
  inkplate.display();

  // Go to sleep for DELAY_MS
  esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);

  // Start deep sleep (this function does not return). Program stops here.
  esp_deep_sleep_start();
}

void loop()
{
  // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
  // time. loop() must be empty!
}