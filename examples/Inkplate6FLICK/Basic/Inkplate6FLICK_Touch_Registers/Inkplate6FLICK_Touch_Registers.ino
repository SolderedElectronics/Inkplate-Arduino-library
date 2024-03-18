/*
   Inkplate6FLICK_Touch_Registers example for Soldered Inkplate 6FLICK
   For this example you will need only USB cable and Inkplate 6FLICK
   Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows you how to use Inkplate 6FLICK touchscreen.
   Once the code is uploaded, open the serial monitor in Arduino IDE and you'll see touchscreen events there.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 March 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);

uint8_t touchRegs[8];

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    display.begin();
    display.clearDisplay();
    display.display();
    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.tsInit(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        while (true)
            ;
    }

    // NOTE!!!
    // Touchscreen cooridinates are automatically swapped and adjusted when screen is rotated
    display.fillTriangle(10, 10, 20, 40, 40, 20, BLACK);
    display.setTextSize(3);
    display.setCursor(60, 60);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Check if there is any touch detected
    if (display.tsAvailable())
    {
        display.tsGetRawData(touchRegs);
        for(int i = 0; i < 8; ++i)
        {
            Serial.print("Reg ");
            Serial.println(touchRegs[i], BIN);
        }

        Serial.println("---------------------------");
        Serial.println();
    }
    delay(1000);
}
