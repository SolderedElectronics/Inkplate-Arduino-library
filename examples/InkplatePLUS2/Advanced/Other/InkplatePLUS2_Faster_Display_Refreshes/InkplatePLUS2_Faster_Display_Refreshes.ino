/*
   InkplatePLUS2_Faster_Display_Refreshes example for Soldered Inkplate PLUS2
   For this example you will need an USB-C cable and an Inkplate PLUS2
   Select "e-radionica Inkplate PLUS2" or "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "e-radionica Inkplate PLUS2" or "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   We can display and partial update our screens faster by leaving the panel power on.
   Just be sure to turn it off when going to deep sleep to save power.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   17 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Soldered InkplatePLUS2 in the boards menu."
#endif

// Include Inkplate library
#include "Inkplate.h"

// You can test it out in 3 bit mode too, by changing to INKPLATE_3BIT
// beware it doesn't support partialUpdate yet
Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200); // Init Serial so we can see the output prints
    display.begin(); // Init library (you should call this function ONLY ONCE)
}

void loop()
{
    // TESTING Standard display using display.display()
    uint32_t t;

    // Draw a circle 
    display.fillCircle(100, 100, 50, BLACK);

    // Measure the time it takes for a full display
    t = millis();
    display.display();
    t = millis() - t;

    // Print the result to Serial
    Serial.print("display.display() took ");
    Serial.print(t);
    Serial.println(" ms");

    display.clearDisplay();

    // TESTING Displaying using display.display(1), where 1 is leaveOn flag
    // by setting leaveOn to 1 screens power supply won't be turned off at the end,
    // and is assumed to be on already.

    display.einkOn(); // Turn on the ePaper (be careful when using this function manually)
    display.fillCircle(200, 100, 50, BLACK); // Draw another circle

    // Again, measure time and print result to Serial
    t = millis();
    display.display(1);
    t = millis() - t;

    Serial.print("display.display(1) took ");
    Serial.print(t);
    Serial.println(" ms");
    display.einkOff(); // Turn off the ePaper (be careful when using this function manually)

    display.clearDisplay();

    // TESTING Displaying using display.partialUpdate() as usual
    
    // Again, draw a circle and measure the time
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

    // Print newlines over Serial
    Serial.println();
    Serial.println();

    // Wait a bit before doing this again
    delay(5000);
}
