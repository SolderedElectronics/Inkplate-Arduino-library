/*
    Inkplate_RTC example for soldered.com Inkplate 2
    For this example you will need USB cable and the Inkplate 2.

    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how to use the internal RTC of the ESP32
    and also read the time from the internet.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    28 Nov 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// For ESP32 time
#include <sys/time.h>

// Create network object for WiFi and HTTP functions
Network network;

// Initialize Inkplate object
Inkplate display;

// Write your SSID and password
char ssid[] = "Soldered";
char pass[] = "dasduino";

// Structure that contains time info
struct tm t;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize network
    network.begin();

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    display.display(); // Clear everything that has previously been on a screen
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.drawTextWithShadow(0, 30, "Welcome to", INKPLATE2_RED, INKPLATE2_BLACK);  // Draw text with shadow
    display.drawTextWithShadow(0, 50, "Inkplate 2!", INKPLATE2_RED, INKPLATE2_BLACK); // Draw text with shadow
    display.display();                                                                // Write hello message
    delay(5000);                                                                      // Wait a little bit

    setTime()

}

void loop()
{
}
