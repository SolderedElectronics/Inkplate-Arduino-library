
#ifndef ARDUINO_INKPLATECOLOR
#error "Please select Inkplate color from board selection"
#endif

#include "Inkplate.h"
#include "SdFat.h"

Inkplate display;
SdFile file;

void setup()
{
    display.begin();
    Serial.begin(115200);
    Serial.print("Sd card init state: ");
    Serial.println(display.sdCardInit());
    display.clearDisplay();
    display.display();
    display.drawImage("image.jpg", 0,0,0,1);
    display.display();
}

void loop()
{
    
}