#include "Inkplate.h"
#include "SdFat.h"

Inkplate display(INKPLATE_1BIT);

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
        Serial.println(display.drawJpegFromSd("Lenna.jpg", 0, 0, 1, 0));
    }
    display.display();

    delay(5000);
}