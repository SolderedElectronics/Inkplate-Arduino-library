/**
 **************************************************
 * @file        Inkplate5V2_OpenMeteo_Weather_Station.ino
 * @brief       Fetches weather data from the Open-Meteo API and displays a
 *              weather-station style dashboard, then deep-sleeps.
 *
 * @details     This example demonstrates how to build a low-power weather
 *              dashboard on Inkplate 5v2 using Open-Meteo as the data source.
 *              On boot, the ESP32 attempts to connect to WiFi (with a timeout).
 *              If the connection succeeds, the device sets local time via NTP
 *              (configTime) using a configurable UTC offset, then requests
 *              current/forecast weather data for the configured latitude and
 *              longitude.
 *
 *              The fetched data is rendered using a GUI helper (Gui) that draws
 *              a background and places weather values (and metadata such as
 *              username/city and battery voltage) into a formatted layout. If
 *              WiFi or API access fails, the GUI displays a dedicated error
 *              screen instead of the dashboard.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT) to
 *              support richer UI elements; partial updates are not supported in
 *              grayscale mode, so updates are full refreshes. After drawing, the
 *              ESP32 enters deep sleep for a fixed interval to save power. Deep
 *              sleep resets the ESP32, so the sketch reruns setup() and fetches
 *              fresh data after every wake-up.
 *
 *              The WiFi connection uses a fixed timeout, so poor signal or
 *              captive portals can prevent connection and trigger the WiFi error
 *              screen. NTP time uses a simple UTC offset (timeZone) and daylight
 *              saving time is not automatically handled unless your
 *              configuration accounts for it. API and JSON handling is
 *              implemented in the provided src/ modules; very large responses or
 *              connectivity issues may require tuning timeouts/buffers.
 *
 *              Expected output: a weather dashboard UI rendered in grayscale,
 *              showing weather values for the configured coordinates plus
 *              city/username and battery voltage; a WiFi error screen if WiFi
 *              fails; an API error screen if the API request fails.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable (battery recommended for deployment)
 * - Extra:      WiFi Internet connection
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password and set your location (timeZone UTC offset,
 *    latitude, longitude). Optionally set myUsername and myCity (display only),
 *    metricUnits (true for metric, false for Imperial) and ntpServer.
 * 3) Upload the sketch to Inkplate 5v2 and open Serial Monitor (115200) for logs.
 * 4) On boot, the device connects to WiFi, syncs time via NTP, fetches weather,
 *    and renders the dashboard.
 * 5) The board deep-sleeps for ~30 minutes, then wakes and refreshes the data.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "your ssid";
const char *password = "your password";

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

