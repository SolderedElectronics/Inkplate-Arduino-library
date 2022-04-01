/*
   Inkplate_Faster_Display example for e-radionica.com Inkplate 2
   For this example you will need a micro USB cable and an Inkplate 2
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   We can display our screens faster by leaving the panel power on.
   Just be sure to turn it off when going to deep sleep to save power.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   29 March 2022 by Soldered
*/

#include "Inkplate.h"

Inkplate display;

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

    display.fillCircle(52, 52, 40, RED);
    display.fillCircle(132, 52, 40, BLACK);

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


    display.fillCircle(200, 100, 50, BLACK);
    display.display(1);
    t = millis();
    display.display(1);
    t = millis() - t;

    Serial.print("display.display(1) took ");
    Serial.print(t);
    Serial.println(" ms");

    display.clearDisplay();

    display.clearDisplay();

    Serial.println();
    Serial.println();

    delay(5000);
}
