/*
    Inkplate6PLUS_Quotables example for Soldered Inkplate 6Plus
    For this example you will need only USB cable and Inkplate 6PLUS.
    Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
    Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 6PLUS. After displaying quote
    ESP32 goes to sleep and wakes up every 300 seconds to show new quote(you can change time interval).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/exmouth_40pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_1BIT);

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 300
#define DELAY_WIFI_RETRY_SECONDS 5

// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();
    display.setTextWrap(false); // Set text wrapping to true
    display.setTextColor(BLACK);
    display.setTextSize(3);

    // Our begin function
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
        display.println(F("Please check SSID and PASS!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    while (!network.getData(quote, author))
    {
        delay(1000);
    }

    display.clearDisplay();
    drawAll(); // Call funtion to draw screen
    display.display();

    // Go to sleep before checking again
    // This is set in microseconds, so it needs to be
    // multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    display.setFont(&exmouth_40pt7b); // Set custom font
    display.setTextSize(1);
    display.setTextColor(BLACK); //Set text color to random color
    printInBoundaries(quote,62,256,900,350,60);
    uint16_t w,h;
    int16_t x,y;
    display.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    display.setCursor(display.width() - w - 50, display.height() - 30); // Set cursor to fit author name in lower right corner
    display.print("-");
    display.println(author); // Print author
}

// Function to print text within a text box
void printInBoundaries(char *text, int x0, int y0, int print_width, int print_height, int rowHeight)
{
    int currentCharIndex = 0;
    char currentWordBuf[128] = {0};
    char currentWordBufWithThreeDots[128] = {0};
    bool lastWord = false;
    bool lastRow = false;
    int currentRow = 0;
    int textLen = strlen(text);

    display.setCursor(x0, y0);

    while (true)
    {
        int i = currentCharIndex;
        while (text[i] != ' ')
        {
            i++;
            if (i > textLen)
            {
                lastWord = true;
                break;
            }
        }

        memset(currentWordBuf, 0, 128);
        memcpy(currentWordBuf, text + currentCharIndex, i - currentCharIndex);

        int16_t printing_x0, printing_y0, printing_x1, printing_y1;
        uint16_t printing_w, printing_h;

        printing_x0 = display.getCursorX();
        printing_y0 = display.getCursorY();
		
        lastRow = (printing_y0 + printing_h + rowHeight > y0 + print_height);
        if (!lastRow)
        {
            display.getTextBounds(currentWordBuf, printing_x0, printing_y0, &printing_x1, &printing_y1, &printing_w,
                                  &printing_h);
        }
        else
        {
            memset(currentWordBufWithThreeDots, 0, 128);
            memcpy(currentWordBufWithThreeDots, currentWordBuf, i - currentCharIndex);
            strcat(currentWordBufWithThreeDots, " ...");
            display.getTextBounds(currentWordBufWithThreeDots, printing_x0, printing_y0, &printing_x1, &printing_y1,
                                  &printing_w, &printing_h);
        }
        bool wordCrossesWidthBoundary = printing_x1 + printing_w > x0 + print_width;

        if (wordCrossesWidthBoundary)
        {
            currentRow++;
            display.setCursor(x0, y0 + rowHeight * currentRow);
            if (lastRow)
            {
                display.setCursor(printing_x0, printing_y0);
                display.print(" ...");
                return;
            }
        }
        display.print(currentWordBuf);
        display.print(" ");

        if (lastWord)
        {
            return;
        }
        currentCharIndex = i + 1;
    }
}
