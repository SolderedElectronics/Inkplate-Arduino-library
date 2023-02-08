/*
    Daily Weather station example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate 2" from Tools -> Board menu.
    Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can use Inkplate 2 to display API data from OpenWeatherMap.
    It shows the minimum and maximum temperature for 3 days and draws an icon from 15h on each of
    those 3 days. If 3 hours have passed, the forecast for the next 3 days is displayed, and if not,
    the first forecast is for today.

    IMPORTANT:
    Make sure to change your desired city, timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    2 January 2023 by Soldered
*/

#include "Inkplate.h"
#include "Network.h"
#include "icons.h"

// Enter your WiFi credentials
char *ssid = "";
char *password = "";

// Enter GPS coordinates for your city - these below are for Osijek in Croatia
String lat = "45.5510548";
String lon = "18.5947808";

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

int day1; // Index of the first day that will be shown in the first rectangle
char abbr[5][5] = {"123", "123", "123", "123", "123"}; // Abbreviations for drawing icons
int cnt = 0;                                           // The variable to store the amount of data

// Variables to find min and max temperature for each of 5 days. Later will be decided which 3 display on the screen
int minTemp[5], maxTemp[5];


void setup()
{
    // Start serial communication
    Serial.begin(115200);

    // Connect Inkplate to the WiFi
    network.begin(ssid, password);

    // Get data from API
    cnt = network.getData(fw, lat, lon, apiKey, &timezone);

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

    // Index of the first icon and max and min temperature which will be displayed
    int startIndex = 0;

    // If the first abbreviation in the abbr array equals 123, that means the first day which contains 15h is the next
    // and draw data for that day on index 1
    if (strcmp(abbr[startIndex], "123") == 0)
    {
        startIndex = 1;
    }

    for (int i = 0; i < 3; ++i)
    {
        // Draw reactangle
        display.drawRect(reactangleCoordinates[i][0], reactangleCoordinates[i][1], 60, 94, INKPLATE2_BLACK);

        // Print the day of the week in the rectangle
        display.setTextSize(2);
        display.drawTextWithShadow(daysCoordinates[i][0], daysCoordinates[i][1], weekDays[(day1 + i) % 7],
                                   INKPLATE2_RED, INKPLATE2_BLACK);
        // Print min and max temperature below the day of the week
        display.setTextSize(1);
        display.setCursor(tempCursorsCoordinates[i][0], tempCursorsCoordinates[i][1]);
        display.print(maxTemp[startIndex]);
        display.print(" C");
        display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
        display.setCursor(tempCursorsCoordinates[i][0], tempCursorsCoordinates[i][1] + 10);
        display.print(minTemp[startIndex]);
        display.print(" C");

        // Choose the proper icon and draw it
        for (int j = 0; j < 18; ++j)
        {
            // Searching for a certain icon through the entire array of icons
            if (strcmp(abbr[startIndex], abbrs[j]) == 0)
                display.drawBitmap(iconCoordinates[i][0], iconCoordinates[i][1], s_logos[j], 48, 48, INKPLATE2_BLACK,
                                   INKPLATE2_WHITE);
        }

        startIndex++;
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
    int startDayIndex[10]; // Index for the array of forecast weather structs on which starts each day
    startDayIndex[0] = 0;  // The first day starts at 0 index
    int day = 1;           // Index for startDayIndex[] array

    // Go through all data and store indexes for the start of each day
    for (int i = 0; i < cnt; i++)
    {
        time_t secs = fw[i].timestamp; // Get a timestamp from each forecast record
        struct tm tinfo;
        gmtime_r(&secs, &tinfo); // Convert timestamp to human-readable format

        // If the hour == 0, it's a new day
        if (tinfo.tm_hour == 0)
        {
            // Store this index
            startDayIndex[day] = i;

            // Go to the next index of the startDayIndex
            day++;
        }
    }

    // Set last index to last struct
    startDayIndex[day] = fw[cnt].timestamp;


    // Go through all 5 days separately
    for (int i = 0; i < 5; i++)
    {
        // Set min and max temp to first on that day
        minTemp[i] = fw[startDayIndex[i]].minTemp;
        maxTemp[i] = fw[startDayIndex[i]].maxTemp;

        // Go through each day and store temperatures and icons on that day
        for (int j = startDayIndex[i]; j < startDayIndex[i + 1]; j++)
        {
            // Find min temp
            if (fw[j].minTemp < minTemp[i])
            {
                minTemp[i] = fw[j].minTemp;
            }

            // Find max temp
            if (fw[j].maxTemp > maxTemp[i])
            {
                maxTemp[i] = fw[j].maxTemp;
            }

            // Take icon for each day in 15 h
            time_t secs = fw[j].timestamp; // Get a timestamp from each forecast record
            struct tm tinfo;
            gmtime_r(&secs, &tinfo); // Convert timestamp to human-readable format
            if (tinfo.tm_hour == 15)
            {
                // Save this icon to abbr[] for drawing on the screen
                strlcpy(abbr[i], fw[j].icon, sizeof(abbr[j]) - 1);
            }
        }
    }
}
