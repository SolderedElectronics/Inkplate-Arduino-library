/*
   Inkplate5_Internal_IO_Expander example for Soldered Inkplate 5
   For this example you will need only a USB cable, Inkplate5, 330 Ohm resistor and LED diode.
   Select "Soldered Inkplate5" from Tools -> Board menu.
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Connect resistor to P1-7 on IO expander header at the top on the backside
   (component side) of Inkplate. You will have to connect one side of 330 Ohm resistor to P1-7, then other side
   to anode of LED and finally, cathode pin of LED to GND.

   This example will show you how you can manipulate with I/Os of internal IO Expander.
   You can only manipulate with Port B of IO Expander (P1-1->P1-7). Port 0 (or Port A) is used for epaper
   panel and TPS65186 PMIC.
   P1-0 is used for ESP32 GPIO0 so you can't use it either.
   P1-1 is used for enabling battery reading (if Batt solder bridge is bridged between second and third pad).
   P1-2 is used for tunring on and off the MOSFET for SD card (if solder bridge is bridged between second and third
   pad). If everything is connected ok, after you upload code, LED should blink.

   DANGER: DO NOT USE P0-0 -> P0-7 and P1-0. In code those are pins from 0-8!!! Using those, you
   might permanently damage the screen. You should only use pins from 9-15.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   21 March 2023 by Soldered.
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

// We are going to use pin P1-7.
// Remember! P0-0 = 0, P0-1 = 1, ..., P0-7 = 7, P1-0 = 8, P1-1 = 9, ..., P1-7 = 15.
#define LED_PIN IO_PIN_B7

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    display.pinModeIO(LED_PIN, OUTPUT,
                      IO_INT_ADDR); // Set P1-7 to output. On that pin, we sholud connect LED with current
                                    // limiting resistor and specify that we want use internal IO expander
}

void loop()
{
    display.digitalWriteIO(LED_PIN, LOW, IO_INT_ADDR);  // Set output to low (LED does not light up)
    delay(1000);                                        // Wait for one second
    display.digitalWriteIO(LED_PIN, HIGH, IO_INT_ADDR); // Set output to high (LED lights up)
    delay(1000);                                        // Wait for one second
}
