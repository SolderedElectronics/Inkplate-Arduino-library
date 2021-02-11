#include "Inkplate.h"

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.joinAP("", "");
}

void loop()
{
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test4.jpg",
                      0, 0, false);
    display.display();
    delay(5000);
}