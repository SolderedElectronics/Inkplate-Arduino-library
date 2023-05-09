/*
    Inkplate2_Quotables example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 2. After displaying quote
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
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "Network.h" // Our networking functions, declared in Network.cpp

#include "Fonts/LoveLetter_Regular10.h" // Include fonts used

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 300

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

// Buffers to store quote, author name and quote length
char quote[256];
char author[64];
int len;

void setup()
{
    // Begin serial communitcation, used for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true); // Set text wrapping to true
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Our begin function
    network.begin(ssid, pass);

    // Try to get the new random quote from the Internet.
    while (!network.getData(quote, author, &len, &display))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // Our main drawing function
    drawAll();
    // Full refresh
    display.display();

    // Go to sleep before checking again
    // This is set in microseconds, so it needs to be
    // multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S); // Activate wake-up timer
    display.setPanelDeepSleep(true); // Put the panel into deep sleep
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Our main drawing function
void drawAll()
{
    display.setFont(&LoveLetter_Regular10); // Set custom font
    display.setTextSize(1);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    printQuote();
    display.setCursor(0, 100);
    display.print("-");
    display.println(author); // Print quote
}

void printQuote()
{
    int currentChar = 0;
    char currentWordBuf[128] = {0};
    display.setCursor(0, 15);
    bool lastWord = false;
    int currentRow = 0;

    while (1)
    {
        // Start from the current char
        int i = currentChar;
        while (quote[i] != ' ') // Find the next space
        {
            i++; // Finds the index where the current word ends
            if (i > len)
                lastWord = true; // If we went further than index, we're in the last word
        }

        // Clear current word buffer and copy the current word substring in it
        memset(currentWordBuf, 0, 128);

        if (!lastWord)
        {
            // copy currently observed part of the string as the current word
            memcpy(currentWordBuf, quote + currentChar, i - currentChar);
        }
        else
        {
            // If it's the last word, copy it until the ending
            memcpy(currentWordBuf, quote + currentChar, len - currentChar);
        }

        int16_t x1, y1;
        uint16_t w, h;

        // Check if the current word will go out of bounds
        display.getTextBounds(currentWordBuf, display.getCursorX(), display.getCursorY(), &x1, &y1, &w, &h);
        if ((x1 + w) > 202)
        {
            // Print in new row if it will
            currentRow++;
            // Offset for each row is 15 pixels
            // +15 for the first row (because currentRow is 0-indexed)
            display.setCursor(0, (15 * currentRow) + 15); 
        }

        // Print word and space
        display.print(currentWordBuf);
        display.print(" ");

        // If we've reached the last word, end the print
        if (lastWord)
            return;

        currentChar = i + 1;
    }
}
