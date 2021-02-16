#include "Inkplate.h"

Inkplate display;

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.joinAP("", "");
}

void loop()
{
    display.drawImage("https://www.fnordware.com/superpng/pnggrad16rgb.png", 0, 0, true, false);
    display.display();
    delay(5000);
}