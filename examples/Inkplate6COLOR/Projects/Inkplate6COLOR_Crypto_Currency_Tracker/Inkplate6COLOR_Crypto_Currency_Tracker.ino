/*
   Inkplate6COLOR_Crypto_Currency_Tracker example for Soldered Inkplate 6COLOR
   For this example you will need only USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 6COLOR to display API data.
   Here we use Coingecko API to get latest cryptocurrency prices and display
   them on the Inkplate screen. If you wish to change the currecny, you can
   edit it below.

   IMPORTANT:
   Make sure to change your timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   22 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000
#define DELAY_WIFI_RETRY_SECONDS 5
// OPTIONAL:
// change to a different currency
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
#include "Fonts/Roboto_Light_100.h"
#include "Fonts/Roboto_Light_20.h"

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
    {36, 93, &Roboto_Light_100, currencyAbbr, 0}, {278, 57, &Roboto_Light_20, date, 0},
    {135, 132, &Roboto_Light_20, fromToDate, 0},  {407, 100, &Roboto_Light_20, "Current price:", 0},
    {564, 135, &Roboto_Light_20, current, 1},     {450, 196, &Roboto_Light_20, "Minimum:", 0},
    {564, 228, &Roboto_Light_20, minimum, 1},     {446, 300, &Roboto_Light_20, "Maximum:", 0},
    {564, 332, &Roboto_Light_20, maximum, 1},

    {13, 407, &Roboto_Light_20, dates, 0},        {87, 407, &Roboto_Light_20, dates + 8, 0},
    {162, 407, &Roboto_Light_20, dates + 16, 0},  {244, 407, &Roboto_Light_20, dates + 24, 0},
    {332, 407, &Roboto_Light_20, dates + 32, 0},

    {321, 171, &Roboto_Light_20, prices, 0},      {321, 230, &Roboto_Light_20, prices + 16, 0},
    {321, 286, &Roboto_Light_20, prices + 32, 0}, {321, 345, &Roboto_Light_20, prices + 48, 0},
};

// Our functions declared below setup and loop
void drawGraph();
void drawAll();
void setTime();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.clearDisplay();
    display.setTextWrap(false);
    display.setTextColor(0, 7);

    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        // Can't connect to netowrk
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
    setTime();
}

void loop()
{
    // Do a new network request every fullRefresh times, defined above
    Serial.print("Retrying retriving data ");
    while (!network.getData(data))
    {
        Serial.print('.');
        delay(1000);
    }

    // Our main drawing function
    drawAll();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

// Function for getting time from NTP server
void setTime()
{
    // Structure used to hold time information
    struct tm timeInfo;
    time_t nowSec;
    // Fetch current time in epoch format and store it
    display.getNTPEpoch(&nowSec);
    // This loop ensures that the NTP time fetched is valid and beyond a certain threshold
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        display.getNTPEpoch(&nowSec);
    }
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));
}

// Function to draw our graph
void drawGraph()
{
    // Edge Coordinates
    int x1 = 10;
    int y1 = 382;

    int x2 = 357;
    int y2 = 146;

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
        itoa(month - ((day - i * 7) <= 0 ? (month == 1 ? -11 : 1) : 0), temp + 32, 10);

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

// Our main drawing function
void drawAll()
{ // Do a full refresh every fullRefresh times, defined above

    // Initial screen clear
    display.clearDisplay();

    // Save current date string, more about it in Network.cpp
    network.getTime(date);

    date[6] = 0; // Remove hours and seconds

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

    // Display all
    display.display();
}
