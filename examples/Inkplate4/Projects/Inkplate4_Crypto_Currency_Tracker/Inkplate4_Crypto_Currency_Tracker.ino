/*
   Inkplate4_Crypto_Currency_Tracker example for Soldered Inkplate 4
   For this example you will need only a USB C cable and Inkplate 4.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have ""Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 4 to display API data.
   Here we use Coingecko API to get last 90 days prices and display
   them on the Inkplate screen:
   https://www.coingecko.com/en/coins/bitcoin/historical_data/usd#panel
   If you wish to change the currecny, you can
   edit it below.

   IMPORTANT:
   Make sure to change your timezone and WiFi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   22 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

// Delay between API calls in miliseconds (first 60 represents minutes so you can change to your need)
#define DELAY_MS 3 * 60 * 1000

// OPTIONAL:
// Change to a different currency
// Use lowercase letters
char currency[] = "bitcoin";
char currencyAbbr[] = "BTC";

// You can find your currency id here:
// https://www.coingecko.com/en/all-cryptocurrencies

// If it loads weirdly you can search the JSON using ctrl/command+f for
// your crypto by name and then find it's id next to it's name and copy those above

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/Picopixel.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Used for storing raw price values
double data[64];

// Used to simplify UI design
struct textElement
{
    int x;
    int y;
    const GFXfont *font;
    char *text;
    char align;
    int size;
};

// Variables for storing all displayed data as char arrays
char date[64];
char fromToDate[64];

char dates[8 * 8];
char prices[16 * 16];

char current[16];
char minimum[16];
char maximum[16];

// All months in a year, for finding current date
char months[][12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

// Out UI elements data
// NULL is default font
textElement elements[] = {
    {3, 17, &FreeSans9pt7b, currencyAbbr, 0, 1}, {167, 8, NULL, date, 0, 1},
    {340, 8, NULL, fromToDate, 0, 1},   {35, 29, NULL, "Current price:", 0, 1},
    {120, 59, &FreeSans9pt7b, current, 1, 1},      {195, 29, NULL, "Minimum:", 0, 1},
    {255, 59, &FreeSans9pt7b, minimum, 1, 1},     {320, 29, NULL, "Maximum:", 0, 1},
    {390, 59, &FreeSans9pt7b, maximum, 1, 1},

    {10, 285, NULL, dates, 0, 1},        {70, 285, NULL, dates + 8, 0, 1},
    {150, 285, NULL, dates + 16, 0, 1},  {235, 285, NULL, dates + 24, 0, 1},
    {310, 285, NULL, dates + 32, 0, 1},

    {380, 115, NULL, prices, 0, 1},      {380, 164, NULL, prices + 16, 0, 1},
    {380, 213, NULL, prices + 32, 0, 1}, {380, 261, NULL, prices + 48, 0, 1},
};

// Our functions declared below setup and loop
void drawGraph();
void drawAll();

void setup()
{
    // Begin serial communication, used for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false);
    display.setTextColor(INKPLATE_BLACK);

    // Our begin function
    network.begin(ssid, pass);

    Serial.println("Retrying retriving data");
    while (!network.getData(data, currency))
    {
        Serial.print('.');
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    // Time drawing function
    drawTime();
    // Full refresh
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Function to draw our graph
void drawGraph()
{
    // Edge Coordinates
    // Lower left
    int x1 = 3;
    int y1 = 275;

    // Upper right
    int x2 = 390;
    int y2 = 78;

    int textMargin = 60;

    // Set min to a very high value, and max to very low, so that any real world data changes it
    double minData = 1e9F;
    double maxData = -1e9F;

    // Find min and max in data
    for (int i = 0; i < 31; ++i)
    {
        minData = min(minData, data[i]);
        maxData = max(maxData, data[i]);
    }

    double span = max(0.01D, (double)abs(maxData - minData));

    // Copy current, min and max data to char arrays to be displayed
    dtostrf(data[30], 8, 2, current);
    strcat(current, "$");
    dtostrf(minData, 8, 2, minimum);
    strcat(minimum, "$");
    dtostrf(maxData, 8, 2, maximum);
    strcat(maximum, "$");

    // Temporary buffer
    char temp[64];

    for (int i = 0; i < 4; ++i)
    {
        dtostrf(minData + (double)i / 4 * span, 5, (maxData < 10.0D ? 3 : 0), temp);
        strncpy(prices + 16 * (3 - i), temp, 16);
    }

    // Find current day in a month
    int day;
    sscanf(date + 3, "%d", &day);

    // Find current month
    int month = 0;
    for (int i = 0; i < 12; ++i)
    {
        if (strncmp(months[i], date, 3) == 0)
            month = i + 1;
    }

    // Find days to display underneath the graph
    for (int i = 0; i < 5; ++i)
    {
        itoa(((day - i * 7) % 31 + 31) % 31, temp, 10);
        itoa(month - ((day - i * 7) <= 0), temp + 32, 10);

        strncpy(dates + 8 * (4 - i), temp, 8);
        strcat(dates + 8 * (4 - i), ".");
        strcat(dates + 8 * (4 - i), temp + 32);
        strcat(dates + 8 * (4 - i), ".");
    }

    // Used for drawing lines
    int prev_x = -1;
    int prev_y = -1;

    // Draw lines
    for (int i = 0; i < 31; ++i)
    {
        // Calculate heights and current x value for a data point
        int tx = x1 + i * (x2 - x1 - textMargin) / 31;
        double v = data[i];
        int h = (int)((double)(v - minData) * (double)abs(y1 - y2) / span);
        int ty = y1 - h;

        // If i is not 0, hence prev x and y exsist so draw lines
        if (i)
        {
            display.drawThickLine(prev_x, prev_y, tx, ty, INKPLATE_BLACK, 2);
        }

        // Set previous x and y
        prev_x = tx;
        prev_y = ty;
    }

    // Draw grid
    for (int i = 0; i < 4; ++i)
        display.drawFastHLine(x1, y2 + i * (y1 - y2) / 4, x2 - x1, INKPLATE_RED);
    for (int i = 0; i < 5; ++i)
        display.drawFastVLine(x1 + i * (x2 - x1) / 5, y2, y1 - y2, INKPLATE_RED);

    display.drawFastVLine(x2 - textMargin + 2, y2, y1 - y2, INKPLATE_RED);
    display.drawThickLine(x1, y1, x2, y1, INKPLATE_RED, 2);
}

// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(date, timeZone);

    // Text settings
    display.setTextColor(INKPLATE_BLACK);
    display.setFont(elements[i].font);
    display.setTextSize(1);

    // 0 is aligned by left bottom corner, 1 by right
    if (elements[i].align == 0)
        display.setCursor((int)(elements[i].x * 0.96), (int)(elements[i].y));

    // Print out text to above set cursor location
    display.print(date);
}

// Our main drawing function
void drawAll()
{
    // Save current date string, more about it in Network.cpp
    network.getTime(date, timeZone);

    // Find current day from string
    int day;
    sscanf(date + 3, "%d", &day);

    // Find what month is it numericly and display it
    for (int i = 0; i < 12; ++i)
    {
        if (strncmp(months[i], date, 3) == 0)
            sprintf(fromToDate, "%d.%d. to %d.%d.", day, (i % 12 ? i : 12), day, ((i + 1) % 12 ? i + 1 : 12));
    }

    // Draw graph
    drawGraph();

    // Draw our UI elements
    for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
    {
        // Text settings
        display.setTextColor(INKPLATE_BLACK);
        display.setFont(elements[i].font);
        display.setTextSize(elements[i].size);

        // 0 is aligned by left bottom corner, 1 by right
        if (elements[i].align == 0)
            display.setCursor((int)(elements[i].x * 0.96), (int)(elements[i].y));
        else if (elements[i].align == 1)
        {
            int16_t x, y;
            uint16_t w, h;

            // Get how much the textx offsets pointer and draw it that much more left
            display.getTextBounds(elements[i].text, 0, 0, &x, &y, &w, &h);

            display.setCursor((int)(elements[i].x * 0.96) - w, (int)(elements[i].y));
        }

        // Print out text to above set cursor location
        display.print(elements[i].text);
    }
}
