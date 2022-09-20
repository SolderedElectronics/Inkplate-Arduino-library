/*
    Inkplate_Wake_Up_On_Touch example for Soldered Inkplate 2
    For this example you will need USB cable, Inkplate 2 and some wire.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can use wake up on touch feature.
    You need to specify which pin (only some pins support touch functionallity on ESP32)
    will be used and which callback will be called on wake up (optional). In this
    example we will be using TS3 pin which is attached to GPIO15 of the ESP32. For all other touch pins
    you can refer to ESP32 documentation, Touch Sensor on link:
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html
    When woken up, Inkplate will make API call to fetch time and display it on display.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    10 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"
#include "Inter16pt7b.h"
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

#define Threshold 40 // Treshold sets sensitivity
touch_pad_t touchPin;

// Variable stored in RTC memory (doesn't get erased by the deep sleep)
RTC_DATA_ATTR int counter = 0;

// Initialize Inkplate object
Inkplate display;

void wake_up_callback()
{
    // Here is possible to write some commands for ESP32 to execute
    // on wake up
}

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize Inkplate
    display.begin();

    // Set new font.
    display.setFont(&Inter16pt7b);

    display.setTextSize(1);    // Set text size
    display.setCursor(10, 70); // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Print a message on the display
    display.print("Touch ");
    display.print(counter++, DEC);

    // Display to screen
    display.display();

    // Setup interrupt on Touch Pad 3 (GPIO15)
    touchAttachInterrupt(T3, wake_up_callback, Threshold);

    // Configure Touchpad as wakeup source
    esp_sleep_enable_touchpad_wakeup();

    Serial.println("Going to sleep..");
    esp_deep_sleep_start();
}

void loop()
{
    // Empty...
}
