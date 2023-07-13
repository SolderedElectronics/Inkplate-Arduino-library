/*
    InkplatePLUS2_Touchscreen_Serial.ino example for Soldered Inkplate PLUS2
    For this example you will need only a USB-C cable and Inkplate PLUS2.
    Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
    Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows you how to use Inkplate PLUS2 touchscreen.
    Once the code is uploaded, open the serial monitor at 115200 baud in Arduino IDE.
    You'll see touchscreen events there!

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    12 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

// Include Inkplate library
#include "Inkplate.h"

// Create Inkplate object in monochrome mode
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Init Serial so we can see the outputs
    Serial.begin(115200);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear the image buffer
    display.display();      // Show the cleared screen

    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.tsInit(true))
    {
        Serial.println("Touchscreen init OK!");
    }
    else
    {
        Serial.println("Touchscreen init fail!");
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
    // Periodically check if there is any touch detected
    if (display.tsAvailable())
    {
        // Variables for storing the touchscreen data
        uint8_t n;
        uint16_t x[2], y[2];

        // See how many fingers are detected (max 2) and copy x and y position of each finger on touchscreen
        n = display.tsGetData(x, y);
        if (n != 0)
        {
            // Print number of fingers to serial monitor, along with their coordinates
            Serial.printf("%d finger%c ", n, n > 1 ? 's' : NULL);
            for (int i = 0; i < n; i++)
                Serial.printf("X=%d Y=%d ", x[i], y[i]);
            Serial.println(); // Newline
        }
        else
        {
            // If touchscreen driver returns us a zero, it means that there are no more touch events pressent on the
            // screen
            x[0] = 0;
            x[1] = 0;
            y[0] = 0;
            y[1] = 0;
            Serial.println("Release");
        }
    }
}
