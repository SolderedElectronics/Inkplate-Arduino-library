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

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/Inter16pt7b.h"
#include "Fonts/Inter8pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Include header files
#include "youtube_icon.h"

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Write your SSID and password
char ssid[] = "Soldered";
char pass[] = "dasduino";

// e-radionica.com channel ID
char channel_id[] = "UCaAzk1FwSGrM9UXpz8wIAyg"; // To get the channel ID of the public channel, go to the wanted Youtube
                                                // channel in Google Chrome, right click and 'view page source', then
                                                // CTRL+F search for "externalId".

char api_key[] =
    "AIzaSyBWcYzRBMh_SH4z7Q3GjRaY7axDZmelcU0"; // API key, you can get one on https://console.developers.google.com/
        // First create a project (name it whatever you like), then click on "Enable APIs and Services" (it's at the top
        // of the screen, it has a plus sign). Next select "YouTube Data API v3" in Enabled APIs and Serivces. Go back
        // at the homepage, click on "Create Credentials" and click "API Key". After the key has been created, edit API
        // Key to restrict it only on use for Youtube API. YouTube Data API v3 in Credentials submenu.

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

// Struct for storing channel data sch as channel name, subscriber count etc.
// It's defined in Network.h
struct channelInfo channel;

char structData[30];

// Out UI elements data
textElement elements[] = {{5, 20, &Inter8pt7b, "Channel:", 0, INKPLATE2_BLACK},
                          {75, 20, &Inter8pt7b, channel.name, 0, INKPLATE2_RED},
                          {5, 40, &Inter8pt7b, "Videos count:", 0, INKPLATE2_BLACK},
                          {125, 40, &Inter8pt7b, (char *)NULL, 0, INKPLATE2_RED},
                          {5, 70, &Inter8pt7b, "Subscribers:", 0, INKPLATE2_BLACK},
                          {5, 90, &Inter8pt7b, (char *)NULL, 0, INKPLATE2_RED},
                          {110, 70, &Inter8pt7b, "Total views:", 0, INKPLATE2_BLACK},
                          {110, 90, &Inter8pt7b, (char *)NULL, 0, INKPLATE2_RED}

};

void setup()
{
    // Begin serial communitcation (send some useful information; WiFi status, HTTP status, etc)
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Welcome screen
    display.setCursor(20, 90); // Set cursor, custom font uses different method for setting cursor
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

    elements[3].text =
        itoa(channel.video_count, structData, 10); // This data should be copied into elements of UI after you get data
    elements[5].text = itoa(channel.subscribers, &structData[8],
                            10); // This data should be copied into elements of UI after you get data
    elements[7].text = itoa(channel.total_views, &structData[16],
                            10); // This data should be copied into elements of UI after you get data
    // This functions takes integer as parameter and converts that int into number but in string format.

    // Our main drawing function
    drawAll();

    // Refresh
    display.display();

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS); // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    // Draw our UI elements
    for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
    {
        // Text settings
        display.setTextColor(elements[i].text_color, INKPLATE2_WHITE);
        // Use custom font. You can find more about that here
        // https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
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
