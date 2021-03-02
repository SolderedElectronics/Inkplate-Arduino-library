#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

int k = 0;

void setup()
{
    Serial.begin(115200);
    display.begin();
}

void loop()
{
    for (int i = 0; i < 600 / 50; ++i)
        for (int j = 0; j < 800 / 50; ++j)
            display.fillRect(j * 50, i * 50, 50, 50, (i + j + k % 2) & 1);
    display.partialUpdate();
    ++k;
    if (k % 2 == 0)
        display.display();
}
