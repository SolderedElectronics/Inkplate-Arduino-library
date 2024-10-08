/*
    Inkplate4_Daily_Weather_Station example for Soldered Inkplate4
    For this example you will need only USB cable and Inkplate4.
    Select "Soldered Inkplate4" from Tools -> Board menu.
    Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 4 to display API data,
    e.g. OpenWeather public weather API for real time data. It shows the forecast
    weather for 4 days. What happens here is basically ESP32 connects to WiFi and
    sends an API call and the server returns data in JSON format containing data
    about weather, then using the library ArduinoJson we extract icons and temperatures
    from JSON data and show it on Inkplate 4. After displaying the weather,
    ESP32 goes to sleep and wakes up every DELAY_MS milliseconds to show new weather
    (you can change the time interval).

    IMPORTANT:
    Make sure to change your desired city, timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries
    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    11 April 2023 by Soldered

    In order to convert your images into a format compatible with Inkplate
    use the Soldered Image Converter available at:
    https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

// ---------- CHANGE HERE  -------------:

// City search query
char city[128] = "OSIJEK";

// Coordinates sent to the api
char lat[] = "45.5510548";
char lon[] = "18.695463";

// Change to your wifi ssid and password
char ssid[] = "";
char pass[] = "";

// Change to your api key, if you don't have one, head over to:
// https://openweathermap.org/guide , register and copy the key provided
char apiKey[] = "";

// ----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Header file for easier code readability
#include "Network.h"

// Including fonts used
#include "Fonts/Inter12pt7b.h"
#include "Fonts/Inter30pt7b.h"
#include "Fonts/Inter8pt7b.h"

// Including icons generated by the py file
#include "icons.h"

// Delay between API calls
#define DELAY_MS (uint32_t)10 * 60 * 1000 // 10 minute time
#define DELAY_WIFI_RETRY_SECONDS 10

// Inkplate object
Inkplate display;

// All our network functions are in this object, see Network.h
Network network;

// Constants used for drawing icons
char abbrs[9][16] = {"01d", "02d", "03d", "04d", "09d", "10d", "11d", "13d", "50d"};

const uint8_t *logos[9] = {icon_01d, icon_02d, icon_03d, icon_04d, icon_09d, icon_10d, icon_11d, icon_13d, icon_50d};

const uint8_t *s_logos[9] = {icon_s_01d, icon_s_02d, icon_s_03d, icon_s_04d, icon_s_09d,
                             icon_s_10d, icon_s_11d, icon_s_13d, icon_s_50d};

char abbr1[16];
char abbr2[16];
char abbr3[16];
char abbr4[16];

// Variables for storing temperature
char temps[8][4] = {
    "-",
    "-",
    "-",
    "-",
};

uint8_t hours = 0;

// Variables for storing current time and weather info
char currentTemp[16] = "-";
char currentWind[16] = "-";

char currentTime[16] = "--:--";

char currentWeather[32] = "-";
char currentWeatherAbbr[8] = "01d";

// Functions defined below
void drawWeather();
void drawCurrent();
void drawTemps();
void drawCity();
void drawTime();

void setup()
{
    // Begin serial and display
    Serial.begin(115200);
    display.begin();

    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        //Can't connect to netowrk
        // Clear display for the error message
        display.clearDisplay();
        // Set the font size;
        display.setTextSize(3);
        // Set the cursor positions and print the text.
        display.setCursor((display.width() / 2) - 200, display.height() / 2);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.setCursor((display.width() / 2) - 200, (display.height() / 2) + 30);
        display.println(F("Please check ssid and pass!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // After connecting to WiFi we need to get internet time from NTP server
    time_t nowSec;
    struct tm timeInfo;
    // Fetch current time in epoch format and store it
    display.getNTPEpoch(&nowSec);
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));

    // Get all relevant data, see Network.cpp for info
    Serial.print("Fetching data");
    while (!network.getData(lat, lon, apiKey, city, temps[0], temps[1], temps[2], temps[3], currentTemp, currentWind,
                            currentTime, currentWeather, currentWeatherAbbr, abbr1, abbr2, abbr3, abbr4, &hours))
    {
        Serial.print('.');
        delay(500);
    }
    Serial.println();
    network.getTime(currentTime);

    // Draw data, see functions below for info
    Serial.println("Drawing on the screen");
    drawWeather();
    drawCurrent();
    drawTemps();
    drawCity();
    drawTime();

    // Next line actually draw all on the screen
    display.display();

    Serial.println("Going to sleep, bye!");

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);

     

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Function for drawing weather info
void drawWeather()
{
    // Searching for weather state abbreviation
    for (int i = 0; i < 9; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbrs[i], currentWeatherAbbr) == 0)
            display.drawBitmap(10, 10, logos[i], 96, 96, INKPLATE4_BLACK);
    }

    // Draw weather state
    display.setTextColor(INKPLATE4_BLACK);
    display.setFont(&Inter12pt7b);
    display.setTextSize(1);
    display.setCursor(30, 110);
    display.println(currentWeather);
}

// Function for drawing current time
void drawTime()
{
    // Drawing current time
    display.setTextColor(INKPLATE4_BLACK);
    display.setFont(&Inter12pt7b);
    display.setTextSize(1);

    display.setCursor(E_INK_WIDTH - 14 * strlen(currentTime), 20);
    display.println(currentTime);
}

// Function for drawing city name
void drawCity()
{
    // Drawing city name
    display.setTextColor(INKPLATE4_BLACK);
    display.setFont(&Inter12pt7b);
    display.setTextSize(1);

    display.setCursor(200 - 7 * strlen(city), 290);
    display.println(city);
}

// Function for drawing temperatures
void drawTemps()
{
    // Drawing 4 rectangles in which temperatures will be written
    int rectWidth = 70;
    int rectSpacing = (E_INK_WIDTH - rectWidth * 4) / 5;
    int yRectangleOffset = 140;
    int rectHeight = 120;

    for (int i = 0; i < 4; i++)
    {
        display.drawRect((i + 1) * rectSpacing + i * rectWidth, yRectangleOffset, rectWidth, rectHeight, BLACK);
    }

    // Drawing days into rectangles
    int textMargin = 10;

    display.setFont(&Inter8pt7b);
    display.setTextSize(1);
    display.setTextColor(INKPLATE4_BLACK);

    int dayOffset = 10;

    for (int i = 0; i < 4; i++)
    {
        display.setCursor((i + 1) * rectSpacing + i * rectWidth + textMargin,
                          yRectangleOffset + textMargin + dayOffset);
        if (i == 0)
        {
            display.println("Today");
        }
        else
        {
            display.println(wDays[hours + i > 6 ? hours + i - 6 : hours + i]);
        }
    }

    int tempOffset = 55;

    // Drawing temperature values into rectangles
    display.setFont(&Inter12pt7b);
    display.setTextSize(1);
    display.setTextColor(INKPLATE4_RED);

    for (int i = 0; i < 4; i++)
    {
        display.setCursor((i + 1) * rectSpacing + i * rectWidth + textMargin,
                          yRectangleOffset + textMargin + tempOffset);
        display.print(temps[i]);
        display.println(F("C"));
    }

    // Drawing icons into rectangles
    int iconOffset = 60;

    for (int i = 0; i < 9; ++i)
    {   
        // If found draw specified icon
        if (strcmp(abbr1, abbrs[i]) == 0)
            display.drawBitmap(1 * rectSpacing + 0 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE4_BLACK, INKPLATE4_WHITE);
    }

    for (int i = 0; i < 9; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr2, abbrs[i]) == 0)
            display.drawBitmap(2 * rectSpacing + 1 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE4_BLACK, INKPLATE4_WHITE);
    }

    for (int i = 0; i < 9; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr3, abbrs[i]) == 0)
            display.drawBitmap(3 * rectSpacing + 2 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE4_BLACK, INKPLATE4_WHITE);
    }

    for (int i = 0; i < 9; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr4, abbrs[i]) == 0)
            display.drawBitmap(4 * rectSpacing + 3 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE4_BLACK, INKPLATE4_WHITE);
    }
}

// Current weather drawing function
void drawCurrent()
{
    // Drawing current information

    // Temperature:
    display.setFont(&Inter30pt7b);
    display.setTextSize(1);

    display.drawTextWithShadow(140, 85, currentTemp, INKPLATE4_BLACK, INKPLATE4_RED);

    int x = display.getCursorX();
    int y = display.getCursorY();

    display.setFont(&Inter12pt7b);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("C"));

    // Wind:
    display.setFont(&Inter30pt7b);
    display.setTextSize(1);

    display.drawTextWithShadow(270, 85, currentWind, INKPLATE4_BLACK, INKPLATE4_RED);

    x = display.getCursorX();
    y = display.getCursorY();

    display.setFont(&Inter12pt7b);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("m/s"));

    // Labels underneath
    display.setFont(&Inter8pt7b);
    display.setTextSize(1);

    display.setCursor(130, 110);
    display.println(F("TEMPERATURE"));

    display.setCursor(270, 110);
    display.println(F("WIND SPEED"));
}
