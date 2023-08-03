/*
   Inkplate10_Internal_IO_Expander example for Soldered Inkplate 10
   For this example you will need only a micro USB cable, Inkplate10, 330 Ohm resistor and LED diode.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Connect resistor to P1-7 (GPB7 pin on older Inkplates with MCP23017) on IO expander header at the top on the backside
   (component side) of Inkplate. You will have to connect one side of 330 Ohm resistor to P1-7 (GPB7), then other side
   to anode of LED and finally, cathode pin of LED to GND.

   This example will show you how you can manipulate with I/Os of internal IO Expander.
   You can only manipulate with Port B of IO Expander (P1-1->P1-7 or GPB1->GPB7). Port 0 (or Port A) is used for epaper
   panel and TPS65186 PMIC.
   P1-0 (GPB0) is used for ESP32 GPIO0 so you can't use it either.
   P1-1 (GPB1) is used for enabling battery reading (if Batt solder bridge is bridged between second and third pad).
   On older Inkplates GPB2, GPB3 and GPB4 are used for reading touchpad (if Touchpad solder bridges are bridged
   between second pad and third pad). If everything is connected ok, after you upload code, LED should blink.

   DANGER: DO NOT USE GPA0-GPA7 and GPB0. In code those are pins from 0-8!!! Using those, you might permanently damage
   the screen. You should only use pins from 9-15.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   5 December 2022 by Soldered Electronics.
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch

// We are going to use pin P1-7 (or GPB7 on older Inkplates).
// Remember! GPA0 = 0, GPA1 = 1, ..., GPA7 = 7, GPB0 = 8, GBP1 = 9, ..., GPB7 = 15.
// And also P0-0 = GPA0, P0-1 = GPA1, ..., P0-7 = GPA7, P1-0 = GPB0, P1-1 = GPB1, ..., P1-7 = GPB7.
#define LED_PIN IO_PIN_B7

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    display.pinModeIO(LED_PIN, OUTPUT,
                      IO_INT_ADDR); // Set P1-7 (or GPB7) to output. On that pin, we sholud connect LED with current
                                    // limiting resistor and specify that we want use internal IO expander or IO
                                    // expander with header named IO Expander 1
}

void loop()
{
    display.digitalWriteIO(LED_PIN, LOW, IO_INT_ADDR);  // Set output to low (LED does not light up)
    delay(1000);                                        // Wait for one second
    display.digitalWriteIO(LED_PIN, HIGH, IO_INT_ADDR); // Set output to high (LED lights up)
    delay(1000);                                        // Wait for one second
}
