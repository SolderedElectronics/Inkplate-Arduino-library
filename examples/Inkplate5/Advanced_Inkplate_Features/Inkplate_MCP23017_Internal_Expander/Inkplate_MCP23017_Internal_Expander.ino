/*
   Inkplate_MCP23017 example for Soldered Inkplate 5
   For this example you will need only a micro USB cable, Inkplate5, 330 Ohm resistor and LED diode.
   Select "Inkplate 5(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 5(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   Connect resistor to GPB7 pin on MCP23017 header at bottom right corner on the backside (component side) of Inkplate.
   You will have to connect one side of 330 Ohm resistor to GPB7, than other side to anode of LED and finally, cathode
   pin of LED to GND.

   This example will show you how you can manipulate with I/Os of internal MCP23017 Expander.
   You can only manipulate with Port B of MCP23017 (GPB1-GPB7). Port A is used for epaper panel and TPS65186 PMIC.
   GPB0 is used for ESP32 GPIO0 so you can't use it either.
   GPB1 is used for enabling battery reading (if Batt solder bridge is bridged between second and third pad)
   GPB2, GPB3 and GPB4 are used for reading touchpad (if Touchpad solder bridges are bridged between second pad and
   third pad). If every thing is connected ok, after you upload code, LED should blink.

   DANGER: DO NOT USE GPA0-GPA7 and GPB0. In code those are pins from 0-8!!! Using those, you might permanently damage
   the screen. You should only use pins from 9-15.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   19 May 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Inkplate 5 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch

#define LED_PIN IO_PIN_B7 // We are going to use pin GPB7 (remember! GPA0 = 0, GPA1 = 1, ..., GPA7 = 7, GPB0 = 8, GBP1 = 9, ..., GPB7 =
       // 15)

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    display.pinModeIO(
        LED_PIN,
        OUTPUT, IO_INT_ADDR); // Set pin 15 (or GPB7) to output. On that pin, we sholud connect LED with current limiting resistor
                                    // and specify that we want use internal MCP or MCP with header named MCP23017-1
}

void loop()
{
    display.digitalWriteIO(LED_PIN, LOW, IO_INT_ADDR);  // Set output to low (LED does not light up)
    delay(1000);                            // Wait for one second
    display.digitalWriteIO(LED_PIN, HIGH, IO_INT_ADDR); // Set output to high (LED lights up)
    delay(1000);                            // Wait for one second
}
