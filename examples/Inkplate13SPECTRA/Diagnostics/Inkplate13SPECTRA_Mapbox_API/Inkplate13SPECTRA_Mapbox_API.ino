/*
  Inkplate13SPECTRA_Mapbox_API example for Inkplate 13SPECTRA
  For this example you will need only USB cable and an Inkplate 13SPECTRA.
  Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
  Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

  https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

  This example will show you how you can use Inkplate 13SPECTRA to display map data.
  This example gets html data from crowdsource campaing and displays them on Inkplate screen.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

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