/*
   Inkplate5V2_Crypto_Currency_Tracker example for Soldered Inkplate 5 V2
   For this example you will need only a USB C cable and Inkplate 5 V2.
   Select "Soldered Inkplate5 V2" from Tools -> Board menu.
   Don't have "Soldered Inkplate5 V2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 5 V2 to display API data.
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
   15 April 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
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
#include "Fonts/Roboto_Light_160.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_40.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_3BIT);

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
textElement elements[] = {
    {20, 173, &Roboto_Light_160, currencyAbbr, 0}, {931, 56, &Roboto_Light_40, date, 0},
    {931, 133, &Roboto_Light_36, fromToDate, 0},   {931, 233, &Roboto_Light_40, "Current price:", 0},
    {1191, 290, &Roboto_Light_40, current, 1},     {991, 413, &Roboto_Light_40, "Minimum:", 0},
    {1191, 463, &Roboto_Light_40, minimum, 1},     {991, 606, &Roboto_Light_40, "Maximum:", 0},
    {1191, 658, &Roboto_Light_40, maximum, 1},

    {24, 700, &Roboto_Light_36, dates, 0},         {163, 700, &Roboto_Light_36, dates + 8, 0},
    {302, 700, &Roboto_Light_36, dates + 16, 0},   {455, 700, &Roboto_Light_36, dates + 24, 0},
    {620, 700, &Roboto_Light_36, dates + 32, 0},

    {745, 300, &Roboto_Light_36, prices, 0},       {745, 413, &Roboto_Light_36, prices + 16, 0},
    {745, 534, &Roboto_Light_36, prices + 32, 0},  {745, 648, &Roboto_Light_36, prices + 48, 0},
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
    display.setTextColor(0, 7);

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
    int x1 = 6;
    int y1 = 651;

    int x2 = 798;
    int y2 = 186;

    int textMargin = 90;

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

    // Draw gradients
    for (int i = 0; i < 31; ++i)
    {
        // Calculate heights and current x value for a data point
        int tx = x1 + i * (x2 - x1 - textMargin) / 31;
        double v = data[i];
        int h = (int)((double)(v - minData) * (double)abs(y1 - y2) / span);
        int ty = y1 - h;

        // If i is not 0, hence prev x and y exist so draw gradients under them
        if (i)
        {
            // Rise over run for one pixel
            double dy = (double)(ty - prev_y) / (double)((x2 - x1 - textMargin) / 31);

            // Draw a gradient line from every pixel to bottom line in graph line
            for (int j = 0; j < (x2 - x1 - textMargin) / 31 + 1; ++j)
                display.drawGradientLine(prev_x + j, (int)round((double)prev_y + dy * (double)j), prev_x + j, y1, 3, 7);
        }

        // Set previous x and y
        prev_x = tx;
        prev_y = ty;
    }

    // After drawing gradients, draw lines
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
            display.drawThickLine(prev_x, prev_y, tx, ty, 0, 5.0);
        }

        // Set previous x and y
        prev_x = tx;
        prev_y = ty;
    }

    // Draw grid
    for (int i = 0; i < 4; ++i)
        display.drawFastHLine(x1, y2 + i * (y1 - y2) / 4, x2 - x1, 4);
    for (int i = 0; i < 5; ++i)
        display.drawFastVLine(x1 + i * (x2 - x1) / 5, y2, y1 - y2, 4);

    display.drawFastVLine(x2 - textMargin + 2, y2, y1 - y2, 4);
    display.drawThickLine(x1, y1, x2, y1, 0, 3);
}

// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(date, timeZone);

    // Text settings
    if (display.getDisplayMode() == INKPLATE_3BIT)
        display.setTextColor(0, 7);
    else
        display.setTextColor(BLACK, WHITE);
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
        display.setTextColor(0, 7);
        display.setFont(elements[i].font);
        display.setTextSize(1);

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
