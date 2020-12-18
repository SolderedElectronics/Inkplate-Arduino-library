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
    if (!display.sdCardInit())
    {
        display.println("Sd card error!");
        display.display();
        delay(1000);
        return;
    }

    display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                      "inkplate10-integration/test/drawImage/png.png",
                      display.PNG, display.BottomLeft);
    display.display();

    delay(5000);
}