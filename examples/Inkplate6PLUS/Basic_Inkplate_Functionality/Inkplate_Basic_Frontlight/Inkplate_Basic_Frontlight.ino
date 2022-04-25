/*
   Inkplate_basic_frontlight example for e-radionica.com Inkplate 6PLUS
   For this example you will need a micro USB cable, Inkplate 6PLUS and a device with WiFi and Internet brower (PC,
   Laptop, Smartphone, ...). Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu. Don't have "Inkplate
   6PLUS(ESP32)" option? Follow our tutorial and add it: https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to use Inkplate 6PLUS frontlight.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6PLUS in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the frontlight


// If your Inkplate doesn't have external (or second) MCP I/O expander, you should uncomment next line,
// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander
//#define ONE_MCP_MODE

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
