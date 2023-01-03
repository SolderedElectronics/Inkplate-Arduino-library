/*
    World clock example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example uses API call to get time for wanted city and it's timezone.
    Fetched data is in JSON format, and library is used to extract data. To choose
    city just type any part of city's name and it will be automatically found, but if you type
    to few letters, any city containig that letters will be found.

    IMPORTANT:
    Make sure to change your wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    12 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "SourceSansPro_Regular6pt7b.h"
#include "SourceSansPro_Regular8pt7b.h"

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300     // How long ESP32 will be in deep sleep (in seconds)

// Create object with all networking functions
Network network;

// Create display object
Inkplate display;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

// Structure for time and date data
struct tm t;

// You can type part of city's and it will be found automatically.
// The more letters you type, the more chance is that your city will be found.
// Refer to Network.h file for full list of cities. Instead of space use underline
// and every word starts with CAPITAL letter and the rest of letters are not capitals.
// List of all available cities and zones can be found here https://www.timeapi.io/api/TimeZone/AvailableTimeZones
const char city1[] = "Zag";
const char city2[] = "Lim";

// Pointers to store city names

void setup()
{
    // Begin serial communitcation, set for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();

    // Our begin function
    network.begin(ssid, pass);

    network.getAllCities();

    network.getData((char *)city1, &t);
    drawTime(17, 1, t.tm_hour > 12 ? 1 : 0,
             (char *)city1); // x coordinate, y coordinate, PM/AM indicator, pointer to city
                             // name Use ternary operator to specify PM or AM is currently.

    network.getData((char *)city2, &t);
    drawTime(115, 1, t.tm_hour > 12 ? 1 : 0, (char *)city2);

    display.display();

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}
// Function to draw time
void drawTime(uint16_t x_pos, uint16_t y_pos, bool am, const char *city)
{
    char *city_name = network.getFullCityName((char *)city);
    uint16_t w = 80;                                  // Clock width
    float xStart[12], yStart[12], xEnd[12], yEnd[12]; // Coordinates for lines that represents minutes

    // This part of code draws analog clock
    display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2, INKPLATE2_BLACK); // Draw outer circles
    display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2 + 1, INKPLATE2_BLACK);

    // Draws lines that represents 3, 6 ,9 and 12 hours on the clocks face
    display.drawThickLine(x_pos + w / 2, y_pos, x_pos + w / 2, y_pos + 5, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos + w, y_pos + w / 2, x_pos + w - 5, y_pos + w / 2, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos + w / 2, y_pos + w, x_pos + w / 2, y_pos + w - 5, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos, y_pos + w / 2, x_pos + 5, y_pos + w / 2, INKPLATE2_BLACK, 2);

    // Draw lines that represents 5, 10, 20, 25, 35, 40, 50, 55 minutes on the clocks face
    for (int i = 0; i < 12; i++)
    {
        float angle_rad = 30 * i * M_PI / 180;

        xStart[i] = x_pos + (cos(angle_rad) * w / 2) + w / 2;
        yStart[i] = y_pos + (sin(angle_rad) * w / 2) + w / 2;

        xEnd[i] = x_pos + (cos(angle_rad) * (w * 0.85) / 2) + w / 2;
        yEnd[i] = y_pos + (sin(angle_rad) * (w * 0.85) / 2) + w / 2;

        display.drawThickLine(xStart[i], yStart[i], xEnd[i], yEnd[i], INKPLATE2_BLACK, 1);
    }

    // This part of code draws needles and calculates their angles
    int x_minute, y_minute, x_hour, y_hour;
    x_minute = x_pos + w / 2 + 30 * (float)sin((t.tm_min / (float)60) * 2 * (float)3.14); //
    y_minute = y_pos + w / 2 - 30 * (float)cos((t.tm_min / (float)60) * 2 * (float)3.14);
    x_hour = x_pos + w / 2 + 22 * sin((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
    y_hour = y_pos + w / 2 - 22 * cos((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);

    display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_minute, y_minute, INKPLATE2_RED, 2); // Needle for minutes
    display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_hour, y_hour, INKPLATE2_BLACK, 3);   // Needle for hours

    // Draw filled circle in the middle
    display.fillCircle(x_pos + w / 2, y_pos + w / 2, 5, INKPLATE2_BLACK);

    display.setTextSize(1);                       // Set text size in comparison to original text 5x7
    display.setFont(&SourceSansPro_Regular8pt7b); // Set customn font
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    char *temp_city_name = strstr(city_name, "/") + 1;
    display.setCursor(x_pos + 40 - strlen(temp_city_name) * 5, 100); // Center city name

    uint8_t cnt = 0;
    while (*(temp_city_name + cnt) != '\0') // Print city name letter by letter
    {
        if (*(temp_city_name + cnt) != '_') // If current letter is underline symbol, replace it with space
            display.print(*(temp_city_name + cnt));
        else
            display.print(" "); // Print space
        cnt++;
    }
    display.setCursor(x_pos + 32, y_pos + 62);
    display.setFont(&SourceSansPro_Regular6pt7b); // Set customn font
    am ? display.print("PM") : display.print("AM");
}
