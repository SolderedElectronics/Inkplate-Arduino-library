/*
   Inkplate_Battery_Voltage_And_Temperature example for e-radionica Inkplate COLOR
   For this example you will need USB cable, Inkplate COLOR and a Lithium battery (3.6V) with two pin JST connector.
   Select "Inkplate COLOR(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate COLOR(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to read voltage of the battery and read temperature from on-board
   temperature sensor which is part of TPS65186 e-paper PMIC.
   NOTE: In order to read temperature, e-paper has to be refreshed at least one time,
   or you have to power up epaper PMIC with einkOn() function from Inkplate library.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Inkplate color in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "symbols.h"  //Include .h file that contains byte array for battery symbol and temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on symbols.h tab in Arduino IDE
Inkplate display; // Create an object on Inkplate library

void setup()
{
    Serial.begin(115200);
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
}

void loop()
{
    float voltage;

    voltage =
        display.readBattery(); // Read battery voltage (NOTE: Doe to ESP32 ADC accuracy, you should calibrate the ADC!)

    display.clearDisplay();                                  // Clear everything in frame buffer of e-paper display
    display.drawBitmap(100, 100, battSymbol, battSymbol_w, battSymbol_h, INKPLATE_BLUE); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.setTextColor(INKPLATE_BLUE);
    display.setTextSize(3);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');

    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
