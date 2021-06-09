
#ifndef ARDUINO_INKPLATECOLOR
#error "Please select Inkplate color from board selection"
#endif

#include "Inkplate.h"
#include "SdFat.h"

Inkplate display;

void setup()
{
    display.begin();
    Serial.begin(115200);

    for(int j = 0; j < 50; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_BLACK);
        }
    }

    for(int j = 50; j < 100; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_WHITE);
        }
    }

    for(int j = 100; j < 150; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_GREEN);
        }
    }

    for(int j = 150; j < 200; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_BLUE);
        }
    }

    for(int j = 200; j < 250; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_RED);
        }
    }

    for(int j = 250; j < 300; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_YELLOW);
        }
    }

    for(int j = 300; j < 350; ++j)
    {
        for(int i = 0; i < 50; ++i)
        {
            display.drawPixel(j, i, INKPLATE_ORANGE);
        }
    }

    display.display();
}

void loop()
{
    
}