/*
   Inkplate6COLOR_Read_Battery_Voltage example for Soldered Inkplate 6COLOR
   For this example you will need USB cable, Inkplate 6COLOR and a Lithium battery (3.6V) with two pin JST connector.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read voltage of the battery.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   23 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
}

void loop()
{
    float voltage = display.readBattery(); // Read battery voltage
    display.clearDisplay();                // Clear everything in frame buffer of e-paper display
    display.drawBitmap(100, 100, battSymbol, battSymbol_w, battSymbol_h,
                       INKPLATE_BLUE); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.setTextColor(INKPLATE_BLUE);
    display.setTextSize(3);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
