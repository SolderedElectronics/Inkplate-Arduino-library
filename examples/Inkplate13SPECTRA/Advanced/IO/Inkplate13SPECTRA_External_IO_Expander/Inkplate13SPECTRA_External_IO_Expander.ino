/**
 **************************************************
 * @file        Inkplate13SPECTRA_External_IO_Expander.ino
 * @brief       External IO expander LED example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to drive a pin on the external IO expander of Inkplate
 *              13SPECTRA. Connect a resistor to the P1-7 pin on the IO Expander 2
 *              header on the right of the backside (component side) of the
 *              Inkplate: one side of the 330 Ohm resistor goes to P1-7, the other
 *              side to the anode of the LED, and the cathode of the LED to GND.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable, 330 Ohm resistor, LED diode
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Wire the LED and 330 Ohm resistor to P1-7 on the IO Expander 2 header as
 *    described above.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The LED blinks, driven through the external IO expander.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch

// We are going to use pin P1-7 (or GPB7 on older Inkplates).
// Remember! GPA0 = 0, GPA1 = 1, ..., GPA7 = 7, GPB0 = 8, GBP1 = 9, ..., GPB7 = 15.
// And also P0-0 = GPA0, P0-1 = GPA1, ..., P0-7 = GPA7, P1-0 = GPB0, P1-1 = GPB1, ..., P1-7 = GPB7.
#define LED_PIN IO_PIN_B7

Inkplate display; // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    display.expander1.pinMode(LED_PIN,
                      OUTPUT); // Set P1-7 to output. On that pin, we sholud connect LED with current limiting resistor
                               // If we do not specify which IO expander we want to use, by the default external IO
                               // expander will be used of the one with header named IO Expander 2.
}

void loop()
{
    display.expander1.digitalWrite(LED_PIN, LOW);  // Set output to low (LED does not light up)
    delay(1000);                           // Wait for one second
    display.expander1.digitalWrite(LED_PIN, HIGH); // Set output to high (LED lights up)
    delay(1000);                           // Wait for one second
}