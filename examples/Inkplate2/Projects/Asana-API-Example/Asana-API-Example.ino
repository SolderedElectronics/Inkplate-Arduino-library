/*
    Youtube subscribers tracker example for Soldered Inkplate 2
    For this example you will need only USB cable and Inkplate 2.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example show how to use Google API to show info about some youtube chhannel.
    You need to register on https://developers.google.com/ and get API key of any kind so you,
    can use yxour API key in API call. That key you should copy in variable api_key.
    Second thing you need to get ID of any youtube channel and copy it in channel_id variable.
    You can get ID by going on any youtube channel profile and copy part of URL link after
    https://www.youtube.com/channel/ (so just some random text after last backslash).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "Besest";
char pass[] = "12345678";

char user_task_list_gid[] = "1201504935951544"; // Youtube channel ID, read in example
                                                // description how to get it
char user_PAT[] = "1/1201504935921103:75e34b1ff53598c60f7f64f61cba783e";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/Inter16pt7b.h"
#include "Fonts/Inter8pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"


// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

struct task *tasks = NULL;
struct task *curr_task = NULL;

// Our functions declared below setup and loop
void drawAll();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Welcome screen
    display.setCursor(20, 90); // Set cursor, custom font uses different method for setting cursor
    // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    display.setTextSize(1);
    display.println(F("Asana API Example!"));
    display.display();
    display.clearDisplay();
    delay(1000);

    // Our begin function
    network.begin();

    while (!network.getData(tasks, curr_task))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    curr_task = tasks; 

    // Our main drawing function
    drawAll();
    // Refresh
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    display.println(tasks->name);
    
}
