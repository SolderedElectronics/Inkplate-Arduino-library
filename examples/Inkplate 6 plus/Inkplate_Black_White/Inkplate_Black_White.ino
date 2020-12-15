#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);
void setup()
{
    // put your setup code here, to run once:
    display.begin();
    display.setTextSize(3);
    display.setCursor(50, 0);
    display.print("Hellou!");
    display.drawLine(100, 100, 250, 300, BLACK);
    display.display();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
