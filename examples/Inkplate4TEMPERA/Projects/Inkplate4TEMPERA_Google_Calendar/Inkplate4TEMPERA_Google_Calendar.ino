/**
 **************************************************
 * @file        Inkplate4TEMPERA_Google_Calendar.ino
 * @brief       Fetch and display events from a public Google Calendar using
 *              WiFi and the Google Calendar API.
 *
 * @details     This example connects Inkplate 4 TEMPERA to a WiFi network,
 *              synchronizes time using the configured timezone, and then
 *              requests event data from a public Google Calendar via the
 *              Google Calendar API using a Calendar ID and API key.
 *
 *              The sketch is intended as a practical template for building
 *              calendar dashboards on e-paper. Typical workflow is:
 *              connect to WiFi -> obtain correct time -> perform HTTPS request
 *              -> parse the API response -> render events on the display.
 *
 *              This example is typically used in 1-bit (BW) mode unless the
 *              sketch selects a different mode; partial update behavior depends
 *              on the chosen mode (partial updates are BW-only). API keys can be
 *              restricted by Google Cloud policies, and requests may fail if the
 *              Calendar API is not enabled or the calendar is not public. Parsing
 *              API responses (JSON) can be memory-intensive, so keep the number
 *              of requested events reasonable if you encounter instability. If the
 *              sketch uses insecure TLS settings (e.g. setInsecure()), treat it as
 *              demo-only and use proper certificate validation/pinning in
 *              production.
 *
 *              Expected output: a calendar view showing fetched event data (event
 *              titles/times as implemented by the sketch) on the e-paper, plus
 *              connection/status logs and any HTTP/API error messages on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      WiFi (2.4 GHz), Google Calendar public calendar ID, Google API
 *               key with the Google Calendar API enabled
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Make your calendar public:
 *    Google Calendar -> Settings -> your calendar -> Access permissions ->
 *    enable public access (otherwise API requests may return 404).
 * 3) Get the Calendar ID:
 *    Google Calendar -> Settings -> your calendar -> Integrate calendar ->
 *    copy "Calendar ID" (e.g. ...@group.calendar.google.com).
 * 4) Create an API key and enable the Google Calendar API:
 *    Google Cloud Console -> APIs & Services -> enable "Google Calendar API",
 *    then Credentials -> Create credentials -> API key (otherwise you may see
 *    403 errors).
 * 5) Enter your WiFi SSID and password (case-sensitive), the timeZone value for
 *    your location, the Calendar ID and the API key in the sketch.
 * 6) Upload the sketch, then open Serial Monitor (optional) to verify network
 *    connection and API responses.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
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