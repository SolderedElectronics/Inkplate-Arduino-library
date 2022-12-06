/*
   Inkplate_basic_frontlight example for Soldered Inkplate 6Plus
   For this example you will need a micro USB cable, Inkplate 6PLUS and a device with WiFi and Internet brower (PC,
   Laptop, Smartphone, ...). Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu. 
   Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? 
   Follow our tutorial and add it: https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to use Inkplate 6Plus frontlight.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the frontlight

void setup()
{
    Serial.begin(115200);    // Set up a serial communication of 115200 baud
    display.begin();         // Init Inkplate library
    display.frontlight(true); // Enable frontlight circuit
    display.setFrontlight(b); // Set frontlight intensity
}

void loop()
{
    if (Serial.available()) // Change frontlight value by sending "+" sign into serial monitor to increase frontlight or
                            // "-" sign to decrese frontlight
                            // try to find hidden lightshow ;)
    {
        bool change = false;    // Variable that indicates that frontlight value has changed and intessity has to be updated
        char c = Serial.read(); // Read incomming serial data

        if (c == '+' && b < 63) // If is received +, increase frontlight
        {
            b++;
            change = true;
        }
        if (c == '-' && b > 0) // If is received -, decrease frontlight
        {
            b--;
            change = true;
        }

        if (c == 's')
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int i = 0; i < 64; ++i)
                {
                    display.setFrontlight(i);
                    delay(30);
                }

                for (int i = 63; i >= 0; --i)
                {
                    display.setFrontlight(i);
                    delay(30);
                }
            }

            change = true;
        }

        if (change) // If frontlight valuse has changed, update the intensity and show current value of frontlight
        {
            display.setFrontlight(b);
            Serial.print("Frontlight:");
            Serial.print(b, DEC);
            Serial.println("/63");
        }
    }
}
