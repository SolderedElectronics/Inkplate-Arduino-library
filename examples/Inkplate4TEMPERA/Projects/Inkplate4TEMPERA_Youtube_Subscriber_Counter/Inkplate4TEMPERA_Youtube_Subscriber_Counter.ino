/*
    Inkplate4TEMPERA_Youtube_Subscriber_Counter example for Soldered Inkplate 4 TEMPERA
    For this example you will need only USB cable and Inkplate 4TEMPERA.
    Select "Soldered Inkplate4TEMPERA" from Tools -> Board menu.
    Don't have "Soldered Inkplate4TEMPERA" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example show how to use Google API to show info about some youtube chhannel.
    You need to register on https://developers.google.com/ and get API key of any kind so, you
    can use your API key in API call. That key you should copy in variable api_key.
    Second thing you need to get ID of any youtube channel and copy it in channel_id variable.
    You can get ID by going on any youtube channel profile and copy part of URL link after
    https://www.youtube.com/channel/ (so just some random text after last backslash).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    13 July 2023 by Soldered

    In order to convert your images into a format compatible with Inkplate
    use the Soldered Image Converter available at:
    https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include font used
#include "Fonts/Inter30pt7b.h"
#include "Fonts/Inter20pt7b.h"
#include "Fonts/Roboto_Light_120.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// Include header files
#include "youtube_icon.h"

// Delay between API calls in miliseconds (10 minutes)
#define DELAY_MS 10 * 60 * 1000
#define DELAY_WIFI_RETRY_SECONDS 5
// Create object with all networking functions
Network network;

// Create display object in 3-bit mode
Inkplate display(INKPLATE_3BIT);

// Write your SSID and password
char ssid[] = "";
char pass[] = "";

char channel_id[] = ""; // To get the channel ID of the public channel, go to the wanted Youtube
                        // channel in Google Chrome, right click and 'view page source', then
                        // CTRL+F search for "externalId".

char api_key[] = ""; // API key, you can get one on https://console.developers.google.com/
                     // First create a project (name it whatever you like), then click on
                     // "Enable APIs and Services" (it's at the top of the screen, it has a
                     // plus sign). Next select "YouTube Data API v3" in Enabled APIs and
                     // Serivces. Go back at the homepage, click on "Create Credentials" and
                     // click "API Key". After the key has been created, edit API Key to
                     // restrict it only on use for Youtube API. YouTube Data API v3 in
                     // Credentials submenu.

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

// Buffers for printing channel data
char buf_subs[15];
char buf_views[15];
char buf_videos[15];

// Struct for storing channel data, defined in Network.h
channelInfo channel;

// Varaible for remembering how many times we have woken up (to only show the splash screen once)
RTC_DATA_ATTR int bootCount = 0;

// Our UI elements
textElement elements[] = {
    {20, 62, &Inter30pt7b, channel.name, 0, 3},
    {19, 60, &Inter30pt7b, channel.name, 0, BLACK},
    {58, 237, &Inter30pt7b, (char *)NULL, 0, BLACK},
    {165, 170, &Inter20pt7b, "Subs:", 1, BLACK},
    {58, 397, &Inter30pt7b, (char *)NULL, 0, BLACK},
    {180, 330, &Inter20pt7b, "Views:", 1, BLACK},
    {58, 537, &Inter30pt7b, (char *)NULL, 0, BLACK},
    {194, 470, &Inter20pt7b, "Videos:", 1, BLACK},
};

void setup()
{
    // Begin serial communitcation (send some useful information; WiFi status, HTTP status, etc)
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false);
    display.setTextColor(BLACK);

    // Show splash screen if it's the first boot
    if (bootCount == 0)
    {
        // Welcome screen
        display.drawImage(youtube_icon, 25, 222, 549, 123); // Draw the YouTube logo
        display.setCursor(50, 370); // Set cursor, custom font uses different method for setting cursor
        display.setTextSize(3);
        display.print("Youtube subscribers tracker!");
        display.display();
        display.clearDisplay();
        bootCount++;
        delay(1000);
    }

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

    while (!network.getData(&channel, channel_id, api_key, &display))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }

    // These functions convert a long (ultoa) or integer(itoa) into a number in string format.
    elements[2].text = ultoa(channel.subscribers, buf_subs, 10);
    elements[4].text = ultoa(channel.total_views, buf_views, 10);
    elements[6].text = itoa(channel.video_count, buf_videos, 10);

    // Our main drawing function
    drawAll();
    // Refresh
    display.display();

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS); // Activate wake-up timer
      
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
    // Draw our UI elements
    for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
    {
        // Text settings
        display.setTextColor(BLACK);
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
