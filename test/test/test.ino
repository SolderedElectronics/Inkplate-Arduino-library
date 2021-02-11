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
    Serial.println(display.sdCardInit());
    display.drawImage("/monalisa.jpg", 0, 0, true);
    display.display();
    delay(5000);
}