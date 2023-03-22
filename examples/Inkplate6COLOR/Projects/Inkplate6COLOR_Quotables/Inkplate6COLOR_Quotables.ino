/*
    Inkplate6COLOR_Quotables example for Soldered Inkplate 6COLOR
    For this example you will need only USB cable and Inkplate 6COLOR.
    Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
    Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML 
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 6COLOR. After displaying quote
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
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/exmouth_32pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 300

// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true); // Set text wrapping to true
    display.setTextColor(BLACK);
    
    // Our begin function
    network.begin();

    while (!network.getData(quote, author))
    {
        Serial.print('.');
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    // Full refresh
    display.display();

    // Activate wakeup timer
    // This is set in microseconds, so it needs to be
    // multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S);

    // Put the panel in the deep sleep
    display.setPanelDeepSleep(0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

    

// Our main drawing function
void drawAll()
{
    display.setFont(&exmouth_32pt7b); // Set custom font
    display.setTextSize(1);
    display.setTextColor(random(0,7)); //Set text color to random color
    printInBoundaries(quote,40,100,520,300,50);
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
