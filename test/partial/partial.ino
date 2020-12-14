#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200);
    display.begin();
}

void loop()
{
    for (int j = 0; j < 50; ++j)
    {
        for (int i = 0; i < 100; ++i)
            display.drawFastVLine(i * 5 + j * 3, 0, 600, BLACK);
        for (int i = 0; i < 100; ++i)
            display.drawFastHLine(j * 3, i * 5, 800, BLACK);
        display.partialUpdate();
        display.clearDisplay();
    }

    display.display();
}
