/*
    Inkplate2_Crypto_Currency_Tracker example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can use Inkplate 2 to display API data.
    Here we use Coingecko API to get latest cryptocurrency prices and display
    them on the Inkplate screen. If you wish to change the currecny, you can
    edit it below.

    IMPORTANT:
    Make sure to change your timezone and wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    30 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "Network.h" // Our networking functions, declared in Network.cpp

// Include fonts used
#include "Fonts/Inter16pt7b.h"
#include "Fonts/Inter8pt7b.h"
#include "bitcoin.h"
#include "ethereum.h"
#include "tether.h"

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000 // Every 3 minutes, minute has 60 seconds and second has 1000 miliseconds

Network network; // Create object with all networking functions

Inkplate display; // Create object for Inkplate library

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

// OPTIONAL:
// change to a different currency
char currency[] = "bitcoin";
char currencyAbbr[] = "BTC";

// You can find your currency id here:
// https://api.coingecko.com/api/v3/coins

// If it loads weirdly you can search the JSON using ctrl/command+f for
// your crypto by name and then find it's id next to it's name and copy those above

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
    uint8_t text_color;
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
    {32, 30, &Inter16pt7b, currencyAbbr, 0, INKPLATE2_BLACK}, {120, 18, &Inter8pt7b, date, 0, INKPLATE2_BLACK},
    {10, 180, &Inter8pt7b, fromToDate, 0, INKPLATE2_BLACK},   {25, 60, &Inter16pt7b, current, 0, INKPLATE2_RED},
    {10, 80, &Inter8pt7b, "Minimum:", 0, INKPLATE2_BLACK},    {10, 100, &Inter8pt7b, minimum, 0, INKPLATE2_RED},
    {115, 80, &Inter8pt7b, "Maximum:", 0, INKPLATE2_BLACK},   {115, 100, &Inter8pt7b, maximum, 0, INKPLATE2_RED}};

// Our functions declared below setup and loop
void drawAll();
void getCurrencyData();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false); // Disable text wrapping
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    display.setCursor(10, 10); // Set cursor, custom font uses different method for setting cursor
    // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    display.setTextSize(2); // Set size of font in comparison to original 5x7 font

    // Our begin function
    network.begin(ssid, pass);

    while (!network.getData(data, currency)) // Get data and check if data is successfully fetched
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    
    // Refresh
    display.display();

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS); // Activate wake-up timer
    display.setPanelDeepSleep(0); // Put the panel into deep sleep
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

void getCurrencyData()
{
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
    strcat(current, "$");
    dtostrf(data[30], 8, 2, current + 1);
    strcat(minimum, "$");
    dtostrf(minData, 8, 2, minimum + 1);
    strcat(maximum, "$");
    dtostrf(maxData, 8, 2, maximum + 1);

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
}

// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(date, timeZone);

    // Text settings
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
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

    getCurrencyData();

    // Find what month is it numericly and display it
    for (int i = 0; i < 12; ++i)
    {
        if (strncmp(months[i], date, 3) == 0)
            sprintf(fromToDate, "%d.%d. to %d.%d.", day, ((i + 1) % 12 ? i + 1 : 12), day, ((i + 2) % 12 ? i + 2 : 12));
    }

    // Draw our UI elements
    for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
    {
        // Text settings
        display.setTextColor(elements[i].text_color, INKPLATE2_WHITE);
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

    if (strstr(currencyAbbr, "BTC"))
    {
        display.drawBitmap(5, 5, bitcoin, 22, 30, INKPLATE2_BLACK);
    }
    else if (strstr(currencyAbbr, "ETH"))
    {
        display.drawBitmap(5, 5, ethereum, 18, 28, INKPLATE2_BLACK);
    }
    else if (strstr(currencyAbbr, "TET"))
    {
        display.drawBitmap(5, 5, tether, 26, 30, INKPLATE2_BLACK);
    }
}
