/*
   InkplatePLUS2_Touchscreen_Draw example for Soldered Inkplate PLUS2
   For this example you will need only USB cable and Inkplate PLUS2
   Select "e-radionica Inkplate PLUS2" or "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "e-radionica Inkplate PLUS2" or "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows you how to use Inkplate PLUS2 touchscreen.
   Once the code is uploaded, try drawing on the screen :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 February 2021 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h"

// Select to draw a line on screen or filled circle
#define DRAW_LINE
// #define DRAW_CIRCLE

Inkplate display(INKPLATE_1BIT);

#ifdef DRAW_LINE
uint16_t xOld, yOld;
#endif

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    display.begin();

    display.setCursor(50,50);
    display.setTextSize(2);
    display.print("Touchscreen test!");

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
}

void loop()
{
    // Check if there is any touch detected
    if (display.tsAvailable())
    {
        uint8_t n;
        uint16_t x[2], y[2];
        // See how many fingers are detected (max 2) and copy x and y position of each finger on touchscreen
        n = display.tsGetData(x, y);
        if (n != 0)
        {
#ifdef DRAW_LINE // Draw line from old point to new
            display.drawLine(xOld, yOld, x[0], y[0], BLACK);

            // Save coordinates to use as old next time
            xOld = x[0];
            yOld = y[0];
#endif

#ifdef DRAW_CIRCLE // Draw circle on touch event coordinates
            display.fillCircle(x[0], y[0], 20, BLACK);
#endif
            display.partialUpdate();
        }
    }
}
