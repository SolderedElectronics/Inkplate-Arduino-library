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
char ssid[] = "Soldered";
char pass[] = "dasduino";

char channel_id[] = "UC2ppjz9TLbBCuGPmmafptrg"; // Youtube channel ID, read in example
                                                // description how to get it
char api_key[] = "AIzaSyASxGJ2D3OcgtkEEgR3Iq0RaZ9_cA7x6PI"; //API key, you can get one on https://developers.google.com/

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/Inter16pt7b.h"
#include "Fonts/Inter8pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Include header files
#include "youtube_icon.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

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

struct channelInfo channel;

char structData[30];

// Out UI elements data
textElement elements[] = {
    {5, 20, &Roboto_Light8, "Channel:", 0 , BLACK}, {75, 20, &Roboto_Light8, channel.name, 0 , RED},
    {5, 40, &Roboto_Light8, "Videos count:", 0, BLACK}, {105, 40, &Roboto_Light8, (char*)NULL, 0, RED},
    {5, 70, &Roboto_Light8, "Subscribers:", 0, BLACK}, {5, 90, &Roboto_Light8, (char*)NULL, 0, RED},
    {110, 70, &Roboto_Light8, "Total views:", 0, BLACK}, {110, 90, &Roboto_Light8, (char*)NULL, 0, RED}

};

// Our functions declared below setup and loop
void drawAll();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true);
    display.setTextColor(BLACK, WHITE);

    // Welcome screen
    display.setCursor(20, 90); // Set cursor, custom font uses different method for setting cursor
    // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    display.setTextSize(1);
    display.drawImage(youtube_icon, 0, 0, 204, 84);
    display.println(F("Youtube subscribers tracker!"));
    display.display();
    display.clearDisplay();
    delay(1000);

    // Our begin function
    network.begin();

    while (!network.getData(&channel))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    elements[3].text = itoa(channel.video_count, structData, 10); //This data should be copied into elements of UI after you get data
    elements[5].text = itoa(channel.subscribers, &structData[8], 10); //This data should be copied into elements of UI after you get data
    elements[7].text = itoa(channel.total_views, &structData[16], 10); //This data should be copied into elements of UI after you get data
    //This functions takes integer as parameter and converts that int into
    // number but in string format.

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
    // Save current date string, more about it in Network.cpp
    //network.getTime(date);


    // Draw our UI elements
    for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
    {
        // Text settings
        display.setTextColor(elements[i].text_color, WHITE);
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
