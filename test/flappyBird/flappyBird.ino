#include "Inkplate.h"
#include "bck_day.h"

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
    int32_t t = millis();
    display.drawBitmap(offSet, 0, (uint8_t *)bck_day, bck_day_w, bck_day_h, BLACK);
    display.partialUpdate();
    display.clearDisplay();
    Serial.println(millis() - t);
    offSet += 70;
    offSet %= 300;
}