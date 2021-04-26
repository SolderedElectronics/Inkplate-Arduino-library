/*
   Cryptocurrency tracker example for e-radionica.com Inkplate 6 plus
   For this example you will need only USB cable and Inkplate 6 plus.
   Select "Inkplate 6 plus(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6 plus(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 plus to display API data.
   Here we use Coingecko API to get latest cryptocurrency prices and display
   them on the Inkplate screen. If you wish to change the currecny, you can
   edit it below.

   IMPORTANT:
   Make sure to change your timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char *ssid = "";
char *pass = "";

// OPTIONAL:
// change to a different currency
char *currency = "bitcoin";
char *currencyAbbr = "BTC";

// You can find your currency id here:
// https://api.coingecko.com/api/v3/coins

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

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

// Refresh counter that's persisant between deepsleeps
RTC_DATA_ATTR unsigned refreshes = 0;

// Constant to determine when to full update and fetch
const int fullRefresh = 20;

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
char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

// Out UI elements data
textElement elements[] = {
    {63, 163, &Roboto_Light_160, currencyAbbr, 0}, {491, 100, &Roboto_Light_40, date, 0},
    {240, 233, &Roboto_Light_40, fromToDate, 0},   {718, 176, &Roboto_Light_40, "Current price:", 0},
    {995, 239, &Roboto_Light_40, current, 1},      {793, 346, &Roboto_Light_40, "Minimum:", 0},
    {995, 403, &Roboto_Light_40, minimum, 1},      {787, 529, &Roboto_Light_40, "Maximum:", 0},
    {995, 587, &Roboto_Light_40, maximum, 1},

    {23, 718, &Roboto_Light_36, dates, 0},         {153, 718, &Roboto_Light_36, dates + 8, 0},
    {286, 718, &Roboto_Light_36, dates + 16, 0},   {430, 718, &Roboto_Light_36, dates + 24, 0},
    {587, 718, &Roboto_Light_36, dates + 32, 0},

    {567, 302, &Roboto_Light_36, prices, 0},       {567, 405, &Roboto_Light_36, prices + 16, 0},
    {567, 505, &Roboto_Light_36, prices + 32, 0},  {567, 608, &Roboto_Light_36, prices + 48, 0},
};

// Our functions declared below setup and loop
void drawGraph();
void drawAll();

void setup()
{
    // Begin serial communication, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.tsInit(true);

    // Turn off backlight
    display.backlight(true);
    display.setBacklight(0);

    display.setTextWrap(false);
    display.setTextColor(0, 7);

    // Welcome screen
    if (refreshes == 0)
    {
        display.setCursor(90, 240);
        display.setTextSize(2);
        display.println(F("Welcome to Inkplate 6 plus cryptocurrency tracker example!"));
        display.setCursor(90, 280);
        display.println(F("Connecting to WiFi..."));
        display.display();
        display.clearDisplay();
        delay(1000);
    }

    // Do a new network request every fullRefresh times, defined above
    if (refreshes % fullRefresh == 0)
    {
        // Our begin function
        network.begin();
        while (!network.getData(data))
        {
            Serial.println("Retrying retriving data!");
            delay(1000);
        }
        // Our main drawing function
        drawAll();
        // Time drawing function
        drawTime();
        // Full refresh
        display.display();
    }
    else
    {
        display.setDisplayMode(INKPLATE_1BIT);
        // Reset screen where date is drawn
        int16_t x1, y1;
        uint16_t w1, h1;
        display.setFont(elements[1].font);
        display.setTextSize(1);
        network.getTime(date);

        // Clear date
        display.getTextBounds(date, (int)(elements[1].x * 0.96), (int)(elements[1].y), &x1, &y1, &w1, &h1);
        display.fillRect(x1, y1, w1 + 5, h1 + 5, BLACK);
        display.partialUpdate();
        display.fillRect(x1, y1, w1 + 5, h1 + 5, WHITE);
        display.partialUpdate();

        // Clear reset text 600, 746
        display.getTextBounds("Press screen to wake up.", 600, 748, &x1, &y1, &w1, &h1);
        display.fillRect(x1 - 1, y1, w1 + 1, h1 + 1, BLACK);
        display.partialUpdate();
        display.fillRect(x1 - 1, y1, w1 + 1, h1 + 1, WHITE);
        display.partialUpdate();

        // Time drawing function
        drawTime();

        // Draw selection
        display.fillRoundRect(755, 620, 100, 100, 15, BLACK);
        display.fillRoundRect(870, 620, 100, 100, 15, BLACK);

        // Just update time
        display.partialUpdate();
    }

    uint32_t t = millis();
    while (millis() - t < 20000)
    {
        if (display.touchInArea(755, 620, 100, 100))
        {
            Serial.println("aa");
            t = millis();
        }
        if (display.touchInArea(870, 620, 100, 100))
        {
            Serial.println("bb");
            t = millis();
        }
        delay(15);
    }

    // Increment refresh count
    ++refreshes;
    goToSleep();
}

void loop()
{
    // Nothing here as we're using deep sleep
}

void goToSleep()
{
    display.setDisplayMode(INKPLATE_1BIT);

    display.fillRect(755, 620, 250, 100, BLACK);
    display.partialUpdate();
    display.fillRect(755, 620, 250, 100, WHITE);
    display.partialUpdate();

    display.setFont(&Roboto_Light_36);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(600, 746);
    display.print("Press screen to wake up.");
    display.partialUpdate();

    Serial.println("Going to sleep...");

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0);

    (void)esp_deep_sleep_start();
}

// Function to draw our graph
void drawGraph()
{
    // Edge Coordinates
    int x1 = 12;
    int y1 = 674;

    int x2 = 630;
    int y2 = 260;

    int textMargin = 68;

    // Set min to a very high value, and max to very low, so that any real world data changes it
    double minData = 1e9F;
    double maxData = -1e9F;

    // Find min and max in data
    for (int i = 0; i < 31; ++i)
    {
        minData = min(minData, data[i]);
        maxData = max(maxData, data[i]);
    }

    double span = max(0.3D, (double)abs(maxData - minData));

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

    // Used for drawing linesa
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

    display.fillRoundRect(755, 620, 100, 100, 15, 0);
    display.fillRoundRect(870, 620, 100, 100, 15, 0);

    display.fillTriangle(815, 680, 815, 660, 795, 670, 7);
    display.fillTriangle(910, 680, 910, 660, 930, 670, 7);
}

// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(date);

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
    network.getTime(date);

    // Find current day from string
    int day;
    sscanf(date + 3, "%d", &day);

    // Find what month is it numericly and display it
    for (int i = 0; i < 12; ++i)
    {
        if (strncmp(months[i], date, 3) == 0)
            sprintf(fromToDate, "%d.%d. to %d.%d.", day, ((i + 1) % 12 ? i + 1 : 12), day, ((i + 2) % 12 ? i + 2 : 12));
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
