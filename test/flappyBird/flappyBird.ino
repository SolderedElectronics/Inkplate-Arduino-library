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

    display.drawBitmap(offSet, 0, (uint8_t *)bck_day, bck_day_w, bck_day_h, BLACK);
    int32_t t = millis();
    display.partialUpdate();
    Serial.println(millis() - t);
    display.clearDisplay();
    offSet += 70;
    offSet %= 300;
}