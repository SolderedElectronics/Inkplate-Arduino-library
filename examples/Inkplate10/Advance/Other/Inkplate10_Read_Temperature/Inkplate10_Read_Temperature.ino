/*
   Inkplate10_Read_Temperature example for Soldered Inkplate 10
   For this example you will need USB cable and Inkplate 10.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read temperature from on-board
   temperature sensor which is part of TPS65186 e-paper PMIC.
   NOTE: In order to read temperature, e-paper has to be refreshed at least one time,
   or you have to power up epaper PMIC with einkOn() function from Inkplate library.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   23 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "tempSymbol.h" //Include .h file that contains byte array for temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on tempSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    int temperature = display.readTemperature();            // Read temperature from on-board temperature sensor
    display.clearDisplay();                                 // Clear frame buffer of display
    display.drawImage(tempSymbol, 100, 100, 38, 79, BLACK); // Draw temperature symbol at position X=100, Y=100
    display.setCursor(150, 125);
    display.print(temperature, DEC); // Print temperature
    display.print('C');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
