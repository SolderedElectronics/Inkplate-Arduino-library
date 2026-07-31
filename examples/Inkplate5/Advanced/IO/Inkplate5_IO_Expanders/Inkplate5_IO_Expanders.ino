/**
 **************************************************
 * @file        Inkplate5_IO_Expanders.ino
 * @brief       Internal and external IO expander control example for Soldered Inkplate 5.
 *
 * @details     Demonstrates how to control GPIO pins on both the internal and
 *              external IO expanders available on Inkplate 5. The example
 *              alternates blinking an LED connected to the external IO expander
 *              (IO Expander 2) and an LED connected to the internal IO expander
 *              (IO Expander 1), showing correct usage and addressing for each.
 *
 *              Expected output: the external IO expander LED blinks for
 *              5 seconds, then the internal IO expander LED blinks for
 *              5 seconds, repeating continuously. External IO expander pins are
 *              all free to use by default. The internal IO expander has
 *              restrictions: DO NOT use GPA0-GPA7 or GPB0, use only pins 9-15
 *              (P1-1 to P1-7). Using restricted pins may permanently damage the
 *              display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable, 2x LED, 2x 330 Ohm resistors
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5"
 *    from Tools -> Board.
 * 2) Connect an LED + 330 Ohm resistor to P1-7 (GPB7) on IO Expander 2
 *    (external).
 * 3) Connect another LED + 330 Ohm resistor to P1-7 (GPB7) on IO Expander 1
 *    (internal).
 * 4) Upload the sketch to Inkplate 5.
 * 5) Observe alternating blinking between external and internal LEDs.
 *
 * @note        Quick start guide: Inkplate 5 has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-05
 * @license     GNU GPL V3
 **************************************************/

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
    display.expander1.pinMode(LED_PIN, OUTPUT); // Set P1-7 to output. On that pin, we sholud connect LED with current
                                    // limiting resistor and specify that we want use internal IO expander
}

void loop()
{
    display.expander1.digitalWrite(LED_PIN, LOW, IO_INT_ADDR);  // Set output to low (LED does not light up)
    delay(1000);                                        // Wait for one second
    display.expander1.digitalWrite(LED_PIN, HIGH, IO_INT_ADDR); // Set output to high (LED lights up)
    delay(1000);                                        // Wait for one second
}
