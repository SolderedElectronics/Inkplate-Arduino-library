#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();
    Serial.println(display.joinAP("e-radionica.com", "croduino"));

    delay(500);
}

void loop()
{
    display.clearDisplay();

    Serial.println(display.drawBitmapFromWeb("http://img.cale.es/bmp/fasani/5e5ff140694ee", 0, 0, 1, 0));

    display.display();
    delay(5000);
}