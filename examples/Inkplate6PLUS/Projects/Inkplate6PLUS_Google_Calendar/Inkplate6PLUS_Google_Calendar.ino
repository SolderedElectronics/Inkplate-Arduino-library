/**
 **************************************************
 * @file        Inkplate6PLUS_Google_Calendar.ino
 * @brief       Fetch and display a public Google Calendar on Inkplate 6PLUS
 *              over WiFi, then deep-sleep between updates.
 *
 * @details     This example connects Inkplate 6PLUS to WiFi, synchronizes time
 *              using NTP, downloads events from a public Google Calendar using
 *              the Google Calendar API (API key + public calendar ID), and
 *              renders the agenda using a simple GUI.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT), which
 *              is suitable for UI layouts with icons/boxes/shading but refreshes
 *              slower and consumes more energy per update than 1-bit BW. After
 *              updating the screen, the ESP32 enters deep sleep for a fixed
 *              interval to save power. Deep sleep restarts the ESP32 on wake, so
 *              all variables are reinitialized and the WiFi/NTP sync plus
 *              calendar fetch repeat each cycle.
 *
 *              Common API errors: 403 Forbidden means the Google Calendar API is
 *              not enabled for the API key/project; 404 Not Found means the
 *              calendar is not public or the Calendar ID is incorrect. Keep API
 *              keys private in real projects and avoid committing them to public
 *              repos.
 *
 *              Expected output: a calendar/agenda view with events for the
 *              configured public calendar, or an on-screen error message if the
 *              WiFi/API fetch fails, with status messages (WiFi / calendar
 *              loaded / failed) on the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access, Google API key with Google Calendar API enabled,
 *               public Google Calendar ID
 * - Serial:     115200 baud (optional; used for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Make your Google Calendar public: Google Calendar settings -> your calendar
 *    -> "Access permissions for events" -> enable public access.
 * 3) Get the Calendar ID: Google Calendar settings -> your calendar ->
 *    "Integrate calendar" -> copy "Calendar ID".
 * 4) Create a Google Cloud API key and enable the Google Calendar API for the
 *    project.
 * 5) Enter ssid/password, calendarID, apiKey, timeZone (UTC offset hours) and
 *    optionally ntpServer in the sketch.
 * 6) Upload the sketch. The device connects, syncs time, fetches events, draws
 *    the calendar view, then deep-sleeps and refreshes periodically.
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


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "Soldered-testingPurposes";
const char *password = "Testing443";

// --- User Info ---
String calendarID = "yourpublicgooglecalid@group.calendar.google.com";
String apiKey = "yourapikey";

int timeZone = 2; // timeZone is the number in (UTC + number) in your time zone | UTC + 2 for Osijek, UTC - 4 for New York City
const char  *ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate(INKPLATE_3BIT);
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
    configTime(timeZone * 3600, 0, ntpServer);
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
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer
  esp_deep_sleep_start();                                        // Put ESP32 into deep sleep.
}

void loop()
{
  // Should remain empty, main logic is in the setup();
}