/*
   Inkplate_basic_backlight example for e-radionica.com Inkplate 6 plus
   For this example you will need a micro USB cable, Inkplate 6 plus and a device with WiFi and Internet brower (PC,
   Laptop, Smartphone, ...). Select "Inkplate 6 plus(ESP32)" from Tools -> Board menu. Don't have "Inkplate 6
   plus(ESP32)" option? Follow our tutorial and add it: https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to use Inkplate 6 plus'es backlight.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the backlight

void setup()
{
    Serial.begin(115200);    // Set up a serial communication of 115200 baud
    display.begin();         // Init Inkplate library
    display.backlight(true); // Enable backlight circuit
    display.setBacklight(b); // Set backlight intensity
}

void loop()
{
    if (Serial.available()) // Change backlight value by sending "+" sign into serial monitor to increase backlight or
                            // "-" sign to decrese backlight
    {
        bool change = false; // Variable that indicates that backlight value has changed and intessity has to be updated
        char c = Serial.read(); // Read incomming serial data

        if (c == '+' && b < 63) // If is received +, increase backlight
        {
            b++;
            change = true;
        }
        if (c == '-' && b > 0) // If is received -, decrease backlight
        {
            b--;
            change = true;
        }

        if (change) // If backlight valuse has changed, update the intensity and show current value of backlight
        {
            display.setBacklight(b);
            Serial.print("Backlight:");
            Serial.print(b, DEC);
            Serial.println("/63");
        }
    }
}
