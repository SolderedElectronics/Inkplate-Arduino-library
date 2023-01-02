/*
    Daily Weather station example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/
    
    This example will show you how you can use Inkplate 2 to display API data,
    e.g. OpenWeatherMap API, and weatherstack for real time data.
    
    IMPORTANT:
    Make sure to change your desired city, timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries.
    
    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    2 January 2023 by Soldered
*/

#include "Inkplate.h"
#include "icons.h"
#include "Network.h"

// Enter your WiFi credentials
char *ssid = "";
char *password = "";

// Enter GPS coordinates for your city
String lat = "";
String lon = "";

// Enter your OpenWeather API key
String apiKey = "";

#define WAKEUP 30 * 60 * 1000 // The time that ESP will be in deep sleep - refresh weather every 30 minutes

Inkplate display;

Network network;

struct tm timeinfo; // Current time info -> human-readable
int timezone;       // The time zone will be automatically set by OWM

// The array of structures for forecast weather for each hour - we will get 40 hours from API
struct forecastWeather fw[40]; 

// Declared abbreviations for week days
char weekDays[8][8] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

// Constants used for drawing icons
char abbrs[32][32] = {"01d", "02d", "03d", "04d", "09d", "10d", "11d", "13d", "50d"};

// Name of the icons in icons.h file
const uint8_t *s_logos[18] = {icon_01d, icon_02d, icon_03d, icon_04d, icon_09d, icon_10d, icon_11d, icon_13d, icon_50d};

int day1;                                // Index of the first day that will be shown in the first rectangle
char abbr[3][5] = {"123", "123", "123"}; // Abbreviations for drawing icons

// Variables to find min and max temperature for each of 3 days shown on the screen
int minTemp[3] = {40, 40, 40}, maxTemp[3] = {0, 0, 0};


void setup()
{
    // Start serial communication
    Serial.begin(115200);

    // Connect Inkplate to the WiFi
    network.begin(ssid, password);

    // Get data from API
    network.getData(fw, lat, lon, apiKey, &timezone);

    // Get the current time and store it to the variables declared on the top
    network.setTime(timezone, &timeinfo); 

    // Get the first day to display
    day1 = getFirstDay();

    // Find data for the next 3 days and save the icon for every day in 15 h
    getIconsAndTemp();

    // Draw this on screen
    displayData();   

    // Go to deep sleep
    Serial.println("Going to deep sleep, bye");
    esp_sleep_enable_timer_wakeup(1000LL * WAKEUP);
    (void)esp_deep_sleep_start();
}


void loop()
{
    // Never here
}


void displayData()
{
    Serial.println("Drawing on the screen");
    display.begin();
    display.clearDisplay();

    // Coordinates for drawing 3 big rectangles
    int reactangleCoordinates[3][2] = {{5, 5}, {75, 5}, {145, 5}};

    // Coordinates for printing day of the week
    int daysCoordinates[3][2] = {{19, 8}, {87, 8}, {158, 8}};

    // Coordinates for printing temperatures for each day
    int tempCursorsCoordinates[3][2] = {{25, 30}, {95, 30}, {165, 30}};

    // Coordinates for drawing icons
    int iconCoordinates[3][2] = {{11, 48}, {81, 48}, {151, 48}};

    for (int i = 0; i < 3; ++i)
    {
        // Draw reactangle
        display.drawRect(reactangleCoordinates[i][0], reactangleCoordinates[i][1], 60, 94, INKPLATE2_BLACK);

        // Print the day of the week in the rectangle
        display.setTextSize(2);
        display.drawTextWithShadow(daysCoordinates[i][0], daysCoordinates[i][1], weekDays[day1 + i], INKPLATE2_RED,
                                   INKPLATE2_BLACK);
        // Print min and max temperature below the day of the week
        display.setTextSize(1);
        display.setCursor(tempCursorsCoordinates[i][0], tempCursorsCoordinates[i][1]);
        display.print(minTemp[i]);
        display.print(" C");
        display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
        display.setCursor(tempCursorsCoordinates[i][0], tempCursorsCoordinates[i][1] + 10);
        display.print(maxTemp[0]);
        display.print(" C");

        // Choose the proper icon and draw it
        for (int j = 0; j < 18; ++j)
        {
            // Searching for a certain icon through the entire array of icons
            if (strcmp(abbr[i], abbrs[j]) == 0)
                display.drawBitmap(iconCoordinates[i][0], iconCoordinates[i][1], s_logos[j], 48, 48, INKPLATE2_BLACK,
                                   INKPLATE2_WHITE);
        }
    }

    display.display(); // Print it all to the inkplate
}

int getFirstDay()
{
    // If it is past 15 h (3 p.m.), then the first square shows the time for tomorrow
    if (timeinfo.tm_hour >= 15)
    {
        if (timeinfo.tm_wday + 1 > 6)
        {
            return 0;
        }
        else
        {
            return timeinfo.tm_wday + 1;
        }
    }

    return timeinfo.tm_wday;
}

void getIconsAndTemp()
{
    for (int j = 0; j < 3; j++) // j is the index for the day
    {
        for (int i = 0; i < 40; i++) // Go through all the data 3 times and for each day determine the data for that day
        {
            time_t secs = fw[i].timestamp; // Get a timestamp from each forecast record
            struct tm tinfo;
            gmtime_r(&secs, &tinfo); // Convert timestamp to human-readable format
            if (tinfo.tm_wday == ((day1 + j) % 7))
            {
                if (fw[i].temp < minTemp[j]) // Find min temp
                {
                    minTemp[j] = fw[i].temp;
                }
                if (fw[i].temp > maxTemp[j]) // Find max temp
                {
                    maxTemp[j] = fw[i].temp;
                }

                if (tinfo.tm_hour == 15) // Take icon for each day in 15 h
                {
                    // Save this icon to abbr[] for drawing on the screen
                    strlcpy(abbr[j], fw[i].icon, sizeof(abbr[j]) - 1);
                }
            }
        }
    }
}
