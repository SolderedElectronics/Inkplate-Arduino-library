/*
    Inkplate5_Quotables example for Soldered Inkplate 5
    For this example you will need only USB-C cable and Inkplate 5.
    Select "Soldered Inkplate5" from Tools -> Board menu.
    Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns data in JSON format
    containing one quote and some information about it, then using library ArduinoJson
    we extract only quote from JSON data and show it on Inkplate 5. After displaying quote
    ESP32 goes to sleep and wakes up every 300 seconds to show new quote (you can change time interval).

    IMPORTANT:
    Make sure to change wifi credentials below.
    Also have ArduinoJson installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    27 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid (WiFi name) and password
char ssid[] = "";
char pass[] = "";

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 300

//-------------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/exmouth_32pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Create object with all networking functions
Network network;

// Create display object
Inkplate display(INKPLATE_1BIT);

// Define the size of the buffer for storing quotes
#define BUFFER_SIZE 256

// Our functions declared below setup and loop
void drawAll();

char quote[BUFFER_SIZE]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communication at 115200 baud rate for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false); // Set text wrapping to true

    // Our begin function
    network.begin(ssid, pass);

    Serial.print("Retrying retriving data");
    int n = 0; // For counting tries
    while (!network.getData(quote, author, BUFFER_SIZE))
    {
        Serial.print('.');
        n++;
        delay(1000);

        // Restart ESP if cannot fetch data more than 10 times
        if(n > 10)
        {
            Serial.println("Something went wrong with fetching data. Restarting ESP");
            ESP.restart();
        }
    }

    display.clearDisplay();
    drawAll(); // Call funtion to draw screen
    display.display();

    // Go to sleep before checking again
    Serial.println("Going to sleep");
    // Activate wake-up timer - this is set in microseconds, so it needs to be multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S); 
    (void)esp_deep_sleep_start(); // Put ESP32 into deep sleep (this function does not return). Program stops here.
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}

// Our main drawing function
void drawAll()
{
    uint8_t rows = strlen(quote) / 43, row = 0;
    display.setFont(&exmouth_32pt7b); // Set custom font
    display.setTextSize(1);
    display.setTextColor(BLACK); //Set text color to black
    display.setCursor(48, display.height() / 2 - 24 * rows); // Place text in the middle
    uint16_t cnt = 0;
    while (quote[cnt] != '\0')
    {
        if (display.getCursorX() > display.width() - 150 && quote[cnt] == ' ')
        {
            row++;
            display.setCursor(48, display.height() / 2 - 24 * rows + row * 48);
        }
        display.print(quote[cnt]);
        cnt++;
    }
    uint16_t w, h;
    int16_t x, y;
    display.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    display.setCursor(display.width() - w - 50, display.height() - 30); // Set cursor to fit author name in lower right corner
    display.print("-");
    display.println(author); // Print author
}
