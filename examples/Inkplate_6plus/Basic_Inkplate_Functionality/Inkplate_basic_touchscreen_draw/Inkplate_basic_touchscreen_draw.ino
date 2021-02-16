/*
   Basic_touchscreen_draw example for e-radionica Inkplate 6 plus
   For this example you will need only USB cable and Inkplate 6 plus
   Select "Inkplate 6 plus(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6 plus(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows you how to use Inkplate 6 plus'es touchscreen.
   Once the code is uploaded, try drawing on the screen :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
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
#ifdef DRAW_LINE
            display.drawLine(xOld, yOld, x[0], y[0], BLACK);
            xOld = x[0];
            yOld = y[0];
#endif

#ifdef DRAW_CIRCLE
            display.fillCircle(x[0], y[0], 20, BLACK);
#endif
            display.partialUpdate();
        }
    }
}