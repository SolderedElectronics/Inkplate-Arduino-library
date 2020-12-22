#include "Inkplate.h"
#include "SdFat.h"
Inkplate display(INKPLATE_3BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.joinAP("e-radionica.com", "croduino");
    display.sdCardInit();
}

void loop()
{
    if (!display.drawImage("http://slosarek.eu/e/1.jpg", 0, 0, 1))
    {
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(1000);

    if (!display.drawImage("http://slosarek.eu/e/2.jpg", 0, 0, 1))
    {
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(1000);

    if (!display.drawImage("http://slosarek.eu/e/4-bit-1.bmp", 0, 0, 1))
    {
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(1000);

    if (!display.drawImage("http://slosarek.eu/e/4-bit-2.bmp", 0, 0, 1))
    {
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(1000);

    if (!display.drawImage("http://slosarek.eu/e/4-bit-3.bmp", 0, 0, 1))
    {
        display.println("Image open error");
        display.display();
    }
    display.display();
    delay(1000);
}
