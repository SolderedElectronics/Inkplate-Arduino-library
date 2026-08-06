/**
 **************************************************
 * @file        Inkplate6PLUS_OpenMeteo_Weather_Station.ino
 * @brief       Fetch weather data from the Open-Meteo API over WiFi, render a
 *              weather-station style UI in 3-bit grayscale, then deep-sleep.
 *
 * @details     This example connects Inkplate 6PLUS to WiFi, synchronizes time
 *              using NTP, and retrieves current weather data from the Open-Meteo
 *              API for a configured latitude/longitude. The fetched data is
 *              formatted into a weather dashboard UI (city/user label, last
 *              update time, battery voltage, and weather metrics) using helper
 *              classes from the project source (NetworkFunctions, WeatherData,
 *              Gui).
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT) to allow
 *              shaded UI elements; grayscale updates are slower and consume more
 *              energy than 1-bit BW. After drawing, the ESP32 enters deep sleep
 *              to conserve power and wakes periodically using the timer. Deep
 *              sleep restarts the ESP32 on wake, so all initialization and
 *              network calls repeat each refresh cycle. Open-Meteo is accessed
 *              over the network, so availability, latency and API changes can
 *              affect results.
 *
 *              Expected output: a weather dashboard UI showing current
 *              conditions for the configured coordinates plus battery voltage
 *              and update metadata; a WiFi error screen if the connection fails
 *              or an API error screen if the Open-Meteo request fails; optional
 *              debug output on the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      WiFi access (internet connection required)
 * - Serial:     115200 baud (optional; used for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password and set your location variables (timeZone,
 *    latitude, longitude). Optionally set myUsername/myCity (display only),
 *    metricUnits (true = metric, false = imperial), ntpServer and
 *    TIME_TO_SLEEP (default: 30 minutes).
 * 3) Upload the sketch to Inkplate 6PLUS.
 * 4) On boot, the device connects to WiFi, syncs time, fetches weather data,
 *    renders the dashboard, then deep-sleeps.
 * 5) The device wakes automatically after TIME_TO_SLEEP and refreshes the data.
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

// --- User and Location Info ---
String myUsername = "Username"; // User's name to be displayed on screen
String myCity = "Osijek";       // City name for weather data
int timeZone =
    2; // timeZone is the number in (UTC + number) in your time zone UTC + 2 for Osijek, UTC - 4 for New York City
float latitude = 45.5550;  // Latitude of the city
float longitude = 18.6955; // Longitude of the city

bool metricUnits = true;  // set this to false if you wish to use Imperial units

const char* ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate(INKPLATE_3BIT); // Create Inkplate display object (3-bit mode for partial grayscale)
NetworkFunctions network;                  // Network utility for weather fetching
NetworkFunctions::UserInfo userInfo;       // Structure to hold user and device info (battery, last updated, etc.)
WeatherData weatherData;          // Structure to hold fetched weather data
Gui gui(inkplate);                // Drawing visuals and info

// --- Deep Sleep Configuration ---
#define uS_TO_S_FACTOR 1000000ULL // Convert microseconds to seconds
#define TIME_TO_SLEEP  1800    // Sleep time: 1800 seconds = 30 minutes

// --- Main Setup: Runs Once on Boot ---
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
        configTime(timeZone * 3600, 0, ntpServer); // Set local time via NTP server
        // Gather battery and city info
        gui.voltage = inkplate.readBattery();
        userInfo.city = myCity;
        userInfo.username = myUsername;
        userInfo.useMetric = metricUnits;

        // Fetch weather data for specified coordinates
        network.fetchWeatherData(&weatherData, &userInfo, &latitude, &longitude);

        // Display if weather API call fails
        if (userInfo.apiError)
        {
            gui.apiError();
        }
        else
        {
            // Success: draw UI and data
            gui.drawBackground();
            gui.displayWeatherData(&weatherData, &userInfo);
        }
    }

    // Sleep to save power; wakes every 30 minutes
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *
                                  uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 30mins here
    esp_deep_sleep_start();                        // Put ESP32 into deep sleep.
}

void loop()
{
    // Nothing to do here - main logic runs once in setup()
}

