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