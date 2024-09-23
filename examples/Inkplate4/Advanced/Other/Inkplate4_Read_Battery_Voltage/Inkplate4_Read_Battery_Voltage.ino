/*
   Inkplate4_Read_Battery_Voltage example for Soldered Inkplate 4
   For this example you will need a USB-C cable, Inkplate 4 and a Lithium battery (3.7V) with two pin JST connector.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read voltage of the battery

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   4 April 2023 by Soldered

   In order to convert your images into a format compatible with Inkplate
   use the Soldered Image Converter available at:
   https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    float voltage = display.readBattery();                    // Read battery voltage
    display.clearDisplay();                                   // Clear everything in frame buffer of e-paper display
    display.drawImage(battSymbol, 110, 100, 104, 104, BLACK); // Draw battery symbol at position X=110 Y=100
    display.setCursor(210, 148);                              // Set cursor at position X=210 Y=148
    display.print(voltage, 2);                                // Print battery voltage with 2 decimals
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
