/*
    Inkplate2_World_Clock example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and WiFi.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example uses API call to get time for wanted city and it's timezone.
    Fetched data is in JSON format, and library is used to extract data. To choose
    the cities just type a part of city's name and it will be automatically found.
    The more letters you type, the more accurate it is.

    IMPORTANT:
    Make sure to change your WiFi credentials below.
    Also, have ArduinoJSON installed in your Arduino libraries (https://arduinojson.org)

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    12 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
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

// Variables to store the hours and minutes about to be drawn
int hours;
int minutes;

// You can type part of city's name and it will be automatically found.
// The more letters you type, the better.
// Refer to Network.h for a full list of cities.
// Important: Instead of space use underline.
// Every word should start with a CAPITAL letter (the rest of letters are lowercase), e.g. "New_York".
// List of all available cities and zones can be found here https://www.timeapi.io/api/TimeZone/AvailableTimeZones
const char city1[] = "Zag";
const char city2[] = "Lim";

void setup()
{
    // Begin serial communitcation, set for debugging
    Serial.begin(115200);

    // Init display
    display.begin();

    // Network begin function
    network.begin(ssid, pass);

    // Get all the available timezones
    network.getAllCities();

    if (network.getData((char *)city1, &hours, &minutes))
    {
        // x coordinate, y coordinate, PM/AM indicator, pointer to city name. Use ternary operator to specify AM or PM
        drawTime(17, 1, hours >= 12 ? 1 : 0, (char *)city1);
    }

    if (network.getData((char *)city2, &hours, &minutes))
    {
        drawTime(115, 1, hours >= 12 ? 1 : 0, (char *)city2);
    }

    display.display();

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    Serial.println("Going to sleep, bye!");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Set wakeup timer
    display.setPanelDeepSleep(0); // Put the panel into deep sleep
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
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
    x_minute = x_pos + w / 2 + 30 * (float)sin((minutes / (float)60) * 2 * (float)3.14); //
    y_minute = y_pos + w / 2 - 30 * (float)cos((minutes / (float)60) * 2 * (float)3.14);
    x_hour = x_pos + w / 2 + 22 * sin((hours / (float)12 + minutes / (float)720) * 2 * (float)3.14);
    y_hour = y_pos + w / 2 - 22 * cos((hours / (float)12 + minutes / (float)720) * 2 * (float)3.14);

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

