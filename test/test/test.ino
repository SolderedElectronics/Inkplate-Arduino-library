/*
   10_Inkplate_Download_And_Show example for e-radionica Inkplate6 original example
   Modified by Martin to make it work with CALE.es web-service for ESP32 displays
   Added deepsleep functionality so it wakes up each N minutes and downloads a new image
*/

#include "HTTPClient.h"          //Include library for HTTPClient
#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "WiFi.h"                //Include library for WiFi
#include "driver/rtc_io.h"       // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>             // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (B/W)

// CALE.es screen URL:
#define SCREEN_URL "https://measlytidylinux.nitko12.repl.co/image.bmp" // BMP 1 bits works

// Einkplate will sleep this minutes after rendering the image.
// Please don't make this shorter than once per hour since it's using our shared server!
#define SLIDES_CHANGE_SECONDS 1800

char *screenUrl = SCREEN_URL;
uint64_t USEC = 1000000;

void setup()
{
    Serial.begin(115200);
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    // Connect to the WiFi network.
    Serial.println("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin("e-radionica.com", "croduino");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(3000);
    }
    Serial.println("aaaa");
    delay(100);

    uint8_t attempts = 3;
    while (attempts--)
    {
        if (display.drawBitmapFromWeb(screenUrl, 0, 0, false))
        {
            WiFi.mode(WIFI_OFF); // Reduce power
            display.display();
            break;
        }
        else
        {
            Serial.println("Image open error");
        }
    }
    display.display();
    Serial.println("Going to sleep...");
    esp_sleep_enable_timer_wakeup(SLIDES_CHANGE_SECONDS *
                                  USEC); // Set EPS32 to be woken up in SLIDES_CHANGE_SECONDS seconds (in this case)
    rtc_gpio_isolate(GPIO_NUM_12);       // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_deep_sleep_start();
}

void loop()
{
    // Nothing should be here while using deep sleep!
}
