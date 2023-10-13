/*
    Inkplate4TEMPERA_Touch_Registers.ino example for Soldered Inkplate 4 TEMPERA
    For this example you will need only a USB-C cable and Inkplate 4 TEMPERA.
    Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
    Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you advanced usage of the Inkplate 4TEMPERA touchscreen.
    Once the code is uploaded, open Serial monitor at 115200 baud!
    It will show raw data from the touch screen register,

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    12 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// Include Inkplate library
#include "Inkplate.h"

// Create Inkplate object in monochrome mode
Inkplate display(INKPLATE_1BIT);

// The array where to read registers of the touchscreen, used to print them
uint8_t touchRegs[8];

void setup()
{
    // Init Serial so we can see contents of the touchscreen registers
    Serial.begin(115200);

    // Init display (you should call this only once)
    display.begin();

    // Clear the frame buffer
    display.clearDisplay();

    // Display the results
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

    // Show where the touchscreen 0, 0 position is to the user
    display.fillTriangle(13, 13, 23, 43, 43, 23, BLACK);
    display.setTextSize(3);
    display.setCursor(65, 65);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Periodically check if we can communicate to the touch screen
    if (display.tsAvailable())
    {
        // Read the raw data of the touch screen registers
        display.tsGetRawData(touchRegs);
        for(int i = 0; i < 8; ++i)
        {
            Serial.print("Reg [");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(touchRegs[i], BIN);
        }

        Serial.println("---------------------------");
        Serial.println();
    }

    delay(1000); // Wait a bit before reading again
}
