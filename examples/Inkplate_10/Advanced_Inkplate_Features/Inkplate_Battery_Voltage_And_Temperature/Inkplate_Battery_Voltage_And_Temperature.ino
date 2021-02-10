/*
   Inkplate_Battery_Voltage_And_Temperature example for e-radionica Inkplate 6
   For this example you will need USB cable, Inkplate 6 and a Lithium battery (3.6V) with two pin JST connector.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to read voltage of the battery and read temperature from on-board
   temperature sensor which is part of TPS65186 e-paper PMIC.
   NOTE: In order to read temperature, e-paper has to be refreshed at least one time,
   or you have to power up epaper PMIC with einkOn() function from Inkplate library.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "symbols.h"  //Include .h file that contains byte array for battery symbol and temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on symbols.h tab in Arduino IDE
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
    int temperature;
    float voltage;

    temperature = display.readTemperature(); // Read temperature from on-board temperature sensor
    voltage =
        display.readBattery(); // Read battery voltage (NOTE: Doe to ESP32 ADC accuracy, you should calibrate the ADC!)
    display.clearDisplay();    // Clear everything in frame buffer of e-paper display
    display.drawImage(battSymbol, 100, 100, 106, 45, BLACK); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');

    display.drawImage(tempSymbol, 100, 200, 38, 79, BLACK); // Draw temperature symbol at position X=100, Y=200
    display.setCursor(150, 225);
    display.print(temperature, DEC); // Print temperature
    display.print('C');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
