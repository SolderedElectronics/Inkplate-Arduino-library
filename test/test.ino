#include "Inkplate.h"
#include "SdFat.h"
Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("e-radionica.com", "croduino");

    Serial.println();
    delay(500);
}

void loop()
{
    display.clearDisplay();
    display.display();


    if (display.sdCardInit())
    {
        Serial.println(display.drawBitmapFromSD("8bit.bmp", 0, 0));
    }
    display.display();

    delay(5000);
}