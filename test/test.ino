#include "Inkplate.h"
#include "SdFat.h"

const int n = 500;

Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("", "");

    delay(500);
}

void loop()
{
    display.clearDisplay();

    display.sdCardInit();
    display.drawPngFromSd("png.png", 0, 0, 1, 0);

    display.display();
    delay(5000);
}