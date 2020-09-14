#include "Inkplate.h"
#include "generatedUI.h"

Inkplate display(INKPLATE_3BIT);

void setup()
{
    display.begin();
    mainDraw();
    int x[] = {600, 700, 800, 700, 600}, y[] = {200, 100, 400, 500, 300};
    int x2[] = {600, 700, 800, 700, 600}, y2[] = {400, 300, 600, 600, 500};
    display.drawPolygon(x, y, 5, 0);
    display.fillPolygon(x2, y2, 5, 0);
    display.display();
}

void loop()
{
}