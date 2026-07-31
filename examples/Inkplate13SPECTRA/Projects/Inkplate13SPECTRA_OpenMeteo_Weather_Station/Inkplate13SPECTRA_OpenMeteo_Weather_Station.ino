/**
 **************************************************
 * @file        Inkplate13SPECTRA_OpenMeteo_Weather_Station.ino
 * @brief       Open-Meteo weather station dashboard for Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Demonstrates how to fetch and display weather data from the
 *              OpenMeteo API using the Inkplate 13SPECTRA e-paper display.
 *
 *              Before you start:
 *              - Enter your WiFi credentials carefully (they are case-sensitive).
 *              - Update the following variables for accurate local weather data:
 *                timeZone, latitude, longitude.
 *              - Set your username and city with myUsername and myCity (for
 *                display only, not essential for the API).
 *
 *              Units: by default the app uses the metric system. To switch to
 *              Imperial units, change metricUnits to
 *              "bool metricUnits = false;".
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      WiFi Internet connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials, timeZone, latitude and longitude in the
 *    sketch.
 * 3) Optionally set myUsername, myCity and metricUnits.
 * 4) Upload the sketch to Inkplate 13SPECTRA.
 * 5) The weather dashboard is rendered on the e-paper display.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "src/includes.h" // Include necessary libraries and dependencies for Inkplate and networking

// --- WiFi Configuration ---
const char *ssid = "";
const char *password = "";

// --- User and Location Info ---
String myUsername = "Soldered"; // User's name to be displayed on screen
String myCity = "Osijek";       // City name for weather data
int timeZone =
    1; // timeZone is the number in (UTC + number) in your time zone UTC + 1 for Osijek, UTC - 4 for New York City
float latitude =  45.5550;  // Latitude of the city
float longitude = 18.6955; // Longitude of the city

bool metricUnits = true;  // set this to false if you wish to use Imperial units

const char* ntpServer = "pool.ntp.org";  // in case you want to use a different one

// --- Device and Data Objects ---
Inkplate inkplate; // Create Inkplate display object (3-bit mode for partial grayscale)
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
        // Set local time via NTP server: ADJUST for your timezone
        configTime(timeZone * 3600, 0, ntpServer); 
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
