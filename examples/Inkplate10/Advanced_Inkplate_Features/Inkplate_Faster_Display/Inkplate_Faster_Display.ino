/*
   Inkplate_Faster_Display example for e-radionica.com Inkplate 6
   For this example you will need a micro USB cable and an Inkplate 6
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   We can display and partial update our screens faster by leaving the panel power on.
   Just be sure to turn it off when going to deep sleep to save power.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   22 September 2021 by e-radionica.com
*/

#include "Inkplate.h"

// You can test it out in 3 bit mode too, by changing to INKPLATE_3BIT
// beware it doesn't support partialUpdate yet
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Initialize the display and serial
    Serial.begin(115200);
    display.begin();
}

void loop()
{
    // TESTING Displaying standardly using display.display()
    uint32_t t;

    display.fillCircle(100, 100, 50, BLACK);

    t = millis();
    display.display();
    t = millis() - t;

    Serial.print("display.display() took ");
    Serial.print(t);
    Serial.println(" ms");

    display.clearDisplay();

    // TESTING Displaying using display.display(1), where 1 is leaveOn flag
    // by setting leaveOn to 1 screens power supply won't be turned off at the end,
    // and is assumed to be on already.

    display.einkOn();

    display.fillCircle(200, 100, 50, BLACK);

    t = millis();
    display.display(1);
    t = millis() - t;

    Serial.print("display.display(1) took ");
    Serial.print(t);
    Serial.println(" ms");

    display.einkOff();

    display.clearDisplay();

    // TESTING Displaying using display.partialUpdate() as usual

    display.fillCircle(300, 100, 50, BLACK);

    t = millis();
    display.partialUpdate();
    t = millis() - t;

    Serial.print("display.partialUpdate() took ");
    Serial.print(t);
    Serial.println(" ms");

    display.clearDisplay();

    // TESTING Displaying using display.partialUpdate(1), where 1 is leaveOn flag
    // by setting leaveOn to 1 screens power supply won't be turned off at the end,
    // and is assumed to be on already, as with display.display's leaveOn flag.

    display.einkOn();

    display.fillCircle(400, 100, 50, BLACK);

    t = millis();
    display.partialUpdate(0, 1);
    t = millis() - t;

    Serial.print("display.partialUpdate(0, 1) took ");
    Serial.print(t);
    Serial.println(" ms");

    display.einkOff();

    display.clearDisplay();

    Serial.println();
    Serial.println();

    delay(5000);
}
