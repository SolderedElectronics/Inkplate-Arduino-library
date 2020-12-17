#include "Inkplate.h"

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);
    display.begin();
}

void loop()
{
    if (!display.sdCardInit())
    {
        display.println("Sd card error!");
        display.display();
        delay(1000);
        return;
    }

    display.drawImage("test.jpg", display.JPG, display.BottomRight);
    display.display();

    delay(5000);
}