/**
 **************************************************
 * @file        Inkplate4TEMPERA_IO_Expanders.ino
 * @brief       Internal and external IO expander control example for Soldered Inkplate 4TEMPERA.
 *
 * @details     Demonstrates how to control GPIO pins on both the internal and
 *              external IO expanders available on Inkplate 4TEMPERA. The example
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
 * - Board:      Soldered Inkplate 4TEMPERA
 * - Hardware:   Inkplate 4TEMPERA, USB cable, 2x LED, 2x 330 Ohm resistors
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate4TEMPERA"
 *    from Tools -> Board.
 * 2) Connect an LED + 330 Ohm resistor to P1-7 (GPB7) on IO Expander 2
 *    (external).
 * 3) Connect another LED + 330 Ohm resistor to P1-7 (GPB7) on IO Expander 1
 *    (internal).
 * 4) Upload the sketch to Inkplate 4TEMPERA.
 * 5) Observe alternating blinking between external and internal LEDs.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch

// We are going to use pin P1-7 (or GPB7 on older Inkplates).
// Remember! GPA0 = 0, GPA1 = 1, ..., GPA7 = 7, GPB0 = 8, GBP1 = 9, ..., GPB7 = 15.
// And also P0-0 = GPA0, P0-1 = GPA1, ..., P0-7 = GPA7, P1-0 = GPB0, P1-1 = GPB1, ..., P1-7 = GPB7.
#define LED_PIN IO_PIN_B7

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();

    // Configure LED pin on both IO expanders
    display.expander2.pinMode(LED_PIN, OUTPUT); // IO Expander 2
    display.expander1.pinMode(LED_PIN, OUTPUT); // IO Expander 1
}

void loop()
{
    // External IO Expander (IO Expander 2) 
    for (int i = 0; i < 5; i++)
    {
        display.expander2.digitalWrite(LED_PIN, HIGH);
        delay(500);
        display.expander2.digitalWrite(LED_PIN, LOW);
        delay(500);
    }

    delay(1000);

    // Internal IO Expander (IO Expander 1)
    for (int i = 0; i < 5; i++)
    {
        display.expander1.digitalWrite(LED_PIN, HIGH, IO_INT_ADDR);
        delay(500);
        display.expander1.digitalWrite(LED_PIN, LOW, IO_INT_ADDR);
        delay(500);
    }

    delay(2000);
}
