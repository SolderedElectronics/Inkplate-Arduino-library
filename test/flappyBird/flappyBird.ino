#include "Inkplate.h"
#include "background_day.h"

Inkplate display(INKPLATE_1BIT);

int offSet = 0;

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.display();
}

void loop()
{
    display.drawBitmap(offSet, 400, (uint8_t *)background_day, background_day_w, background_day_h, BLACK);
    display.partialUpdate();
    display.drawBitmap(offSet, 400, (uint8_t *)background_day, background_day_w, background_day_h, WHITE);
    ++offSet;
    offSet %= 10;
}