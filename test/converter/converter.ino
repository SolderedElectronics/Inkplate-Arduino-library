#include "Inkplate.h"
#include "img_new_dither.h"

Inkplate display;

void setup()
{
    Serial.begin(115200);

    display.begin();
}

void loop()
{
    display.drawImage(rick_and_morty_1604562846, 0, 0, rick_and_morty_1604562846_w, rick_and_morty_1604562846_h);
    display.display();
    delay(5000);
}