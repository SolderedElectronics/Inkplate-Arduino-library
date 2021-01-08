#include "Inkplate.h"

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.joinAP("e-radionica.com", "croduino");
}

void loop()
{
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test.bmp",
                      0, 0);
    display.display();
    delay(5000);
}