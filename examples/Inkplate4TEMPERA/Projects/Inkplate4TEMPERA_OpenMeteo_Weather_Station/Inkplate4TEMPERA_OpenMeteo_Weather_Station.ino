/**
 **************************************************
 * @file        Inkplate4TEMPERA_OpenMeteo_Weather_Station.ino
 * @brief       Open-Meteo weather station dashboard in 3-bit grayscale with
 *              periodic deep-sleep updates on Inkplate 4 TEMPERA.
 *
 * @details     This example connects Inkplate 4 TEMPERA to WiFi, sets local time
 *              using NTP (based on the configured UTC offset), fetches current
 *              weather data from the Open-Meteo API for the specified latitude
 *              and longitude, and renders a dashboard-style screen using a GUI
 *              helper layer (Gui / WeatherData / NetworkFunctions).
 *
 *              After drawing the UI, the ESP32 enters deep sleep for a fixed
 *              interval (default: 30 minutes). On wake-up, the ESP32 restarts
 *              and the sketch runs setup() again, updating the screen with fresh
 *              weather data; all runtime state is lost unless stored in RTC
 *              memory, and this sketch recomputes everything each wake cycle. The
 *              UI also displays user-facing info such as a username/city label
 *              and battery voltage (read from the device).
 *
 *              Display mode is 3-bit grayscale (INKPLATE_3BIT, 8 levels) and
 *              partial update is not available in grayscale mode, so the UI is
 *              updated using a full refresh. WiFi connection attempts are
 *              time-limited; if your network is slow to join, increase the timeout
 *              or retry settings. RAM usage depends on the UI assets/fonts used by
 *              src/includes.h and the GUI layer, so keep additional buffers small
 *              if extending the example.
 *
 *              Expected output: a weather dashboard UI (icons/text/values as
 *              implemented by Gui) including the city/user label and battery
 *              voltage; a WiFi error screen if the connection fails; an API error
 *              screen if the Open-Meteo fetch/parsing fails.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      WiFi (2.4 GHz), internet access
 * - Serial:     115200 baud (optional, for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password (ssid, password) and set timeZone (UTC
 *    offset, e.g. 2 for UTC+2), latitude and longitude.
 * 3) Optionally set myUsername and myCity for the on-screen header, set
 *    metricUnits = false for Imperial units, and change ntpServer if you need a
 *    different pool.
 * 4) Upload the sketch.
 * 5) The device connects to WiFi, fetches weather data, draws the dashboard,
 *    then sleeps. It wakes and refreshes automatically every TIME_TO_SLEEP.
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

// --- User and Location Info ---
String myUsername = "Username"; // User's name to be displayed on screen
String myCity = "Osijek";       // City name for weather data
int timeZone =
    2; // timeZone is the number in (UTC + number) in your time zone UTC + 2 for Osijek, UTC - 4 for New York City
float latitude =  45.5550;  // Latitude of the city
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
    inkplate.battery.begin();

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
        gui.voltage = inkplate.battery.voltage();
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

