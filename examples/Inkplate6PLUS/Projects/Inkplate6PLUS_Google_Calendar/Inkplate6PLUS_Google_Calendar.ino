/*
Inkplate6PLUS_Google_Calendar for Soldered Inkplate 6PLUS

Getting Started:
For setup and documentation, visit: https://inkplate.readthedocs.io/en/latest/

Before You Start:

  - Enter your WiFi credentials carefully (they are case-sensitive).

  - Update the timeZone variable according to your data

  - Get Google Calendar public calendar ID and API key:
      1. Calendar ID: Go to calendar.google.com > Settings > Select your calendar > "Integrate calendar" > Copy "Calendar ID" (e.g. random@group.calendar.google.com).
      2. API Key: Go to console.cloud.google.com > Select/create a project > "APIs & Services" > "Credentials" > "Create credentials" > API key.
      Make sure your calendar is public under "Access permissions" in calendar settings.
*/

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

const char *ssid = "Soldered-testingPurposes";
const char *password = "Testing443";

int timeZone = 2; // timeZone is the number in (UTC + number) in your time zone UTC + 2 for Osijek, UTC - 4 for New York City

String calendarID = "yourpublicgooglecalendarid@group.calendar.google.com";
String apiKey = "yourapikey";

// String ntpServer = "pool.ntp.org";  // in case you want to use a different one

Inkplate inkplate(INKPLATE_3BIT);
calendarData calendar;
Network network(calendarID, apiKey);
Gui gui(inkplate);

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000 // Convert microseconds to seconds
#define TIME_TO_SLEEP 600      // Sleep time: 600 seconds = 10 minutes

void setup()
{
  Serial.begin(115200);    // Initialize serial monitor for debugging
  inkplate.begin();        // Start the Inkplate display
  inkplate.clearDisplay(); // Clear the screen
  inkplate.setRotation(1); // Portrait mode | if it's upside down do setRotation(3);

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
    configTime(timeZone * 3600, 0, "pool.ntp.org");
    // Fetch and display calendar
    if (network.fetchCalendar(&calendar))
    {
      Serial.println("Calendar loaded.");
      gui.showCalendar(&calendar);
    }
    else
    {
      Serial.println("Failed to load calendar.");
      gui.showError("Failed to load calendar.");
    }
  }
  // Sleep to save power; wakes every 10 minutes
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 30mins here
  esp_deep_sleep_start();                                        // Put ESP32 into deep sleep.
}

void loop()
{
  // Should remain empty, main logic is in the setup();
}