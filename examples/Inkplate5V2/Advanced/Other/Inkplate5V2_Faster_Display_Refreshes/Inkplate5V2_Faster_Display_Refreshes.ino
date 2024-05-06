/*
   Inkplate5V2_Faster_Display_Refreshes example for Soldered Inkplate 5 V2
   For this example you will need a USB-C cable and an Inkplate 5
   Select "Soldered Inkplate5 V2" from Tools -> Board menu.
   Don't have "Soldered Inkplate5 V2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   We can display and partial update our screens faster by leaving the panel power on.
   Just be sure to turn it off when going to deep sleep to save power.
   Open the Serial Monitor at 115200 baud rate to see the results.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 April 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

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

    // TESTING Displaying using display.partialUpdate(0, 1), where 1 is leaveOn flag
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
