/**
 **************************************************
 * @file        Inkplate13SPECTRA_Google_Calendar.ino
 * @brief       Google Calendar dashboard example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Fetches events from a public Google Calendar and renders them on
 *              the Inkplate 13SPECTRA e-paper display.
 *
 *              Before you start:
 *              - Enter your WiFi credentials carefully (they are case-sensitive).
 *              - Update the timeZone variable according to your data.
 *              - Get a Google Calendar public calendar ID and API key:
 *                1. Calendar ID: go to calendar.google.com > Settings > select
 *                   your calendar > "Integrate calendar" > copy "Calendar ID"
 *                   (e.g. random@group.calendar.google.com).
 *                2. API key: go to console.cloud.google.com > select/create a
 *                   project > "APIs & Services" > "Credentials" >
 *                   "Create credentials" > API key.
 *                Make sure your calendar is public under "Access permissions"
 *                in the calendar settings.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      WiFi Internet connection, public Google Calendar ID and Google
 *               API key
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials and timeZone in the sketch.
 * 3) Enter your public Google Calendar ID and API key.
 * 4) Upload the sketch to Inkplate 13SPECTRA.
 * 5) The calendar events are fetched and rendered on the display.
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

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "your ssid";
const char *password = "your password";

// --- User Info ---
String calendarID = "your calendar id";
String apiKey = "your apiKey";

int highlightColor = 4;  // this is used to highlight the currently ongoing event | change the integer to change the color (see below)

// 0 -> black
// 1 -> white
// 2 -> yellow
// 3 -> red
// 5 -> blue
// 6 -> green

int timeZone = 1; // timeZone is the number in (UTC + number) in your time zone | UTC + 2 for Osijek, UTC - 4 for New York City
const char  *ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate;
calendarData calendar;
NetworkFunctions network(calendarID, apiKey);
Gui gui(inkplate);

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000ULL // Convert microseconds to seconds
#define TIME_TO_SLEEP 600      // Sleep time: 600 seconds = 10 minutes

void setup()
{
  Serial.begin(115200);    // Initialize serial monitor for debugging
  inkplate.begin();        // Start the Inkplate display
  inkplate.clearDisplay(); // Clear the screen

  // Attempt to connect to WiFi
  const unsigned long timeout = 30000;
  unsigned long startTime = millis();
  bool connected = false;
  
  while (!connected && (millis() - startTime) < timeout)
  {
    connected = inkplate.connectWiFi(ssid, password, 10, true);
  }

  // If WiFi failed, display error message
  if (!connected)
  {
    gui.wifiError();
  }
  else
  {
    configTime(timeZone * 3600, 0, ntpServer);
    // Fetch and display calendar
    if (network.fetchCalendar(&calendar))
    {
      Serial.println("Calendar loaded.");
      gui.setHighlightColor(highlightColor);
      gui.showCalendar(&calendar);
    }
    else
    {
      Serial.println("Failed to load calendar.");
      gui.showError("Failed to load calendar.");
    }
  }
  // Sleep to save power; wakes every 10 minutes
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer
  esp_deep_sleep_start();                                        // Put ESP32 into deep sleep.
}

void loop()
{
  // Should remain empty, main logic is in the setup();
}
