#include "Inkplate.h"

Inkplate display;

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.joinAP("e-radionica.com", "croduino");
}

void loop()
{
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test2.jpg",
                      0, 0, false, false);
    display.display();
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test2.jpg",
                      0, 0, false, true);
    display.display();
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test2.jpg",
                      0, 0, true, false);
    display.display();
    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/test/test2.jpg",
                      0, 0, true, true);
    display.display();
    delay(5000);
}