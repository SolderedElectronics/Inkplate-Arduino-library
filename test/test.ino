#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200);

    pinMode(12, INPUT);
    pinMode(13, INPUT);
    display.begin();
    delay(500);
}

void loop()
{
    Serial.println(analogRead(12));
    Serial.println(analogRead(13));
    Serial.println();
    delay(1000);
}