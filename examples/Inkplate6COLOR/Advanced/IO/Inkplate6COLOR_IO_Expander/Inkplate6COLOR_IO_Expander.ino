/**
 **************************************************
 * @file        Inkplate6COLOR_IO_Expander.ino
 * @brief       Controls an LED using the onboard I/O expander on Inkplate 6COLOR.
 *
 * @details     This example demonstrates how to use the external I/O expander
 *              available on Inkplate 6COLOR. The I/O expanders allow additional
 *              GPIO pins to be controlled through I2C, extending the number of
 *              usable digital pins beyond the ESP32's native GPIOs.
 *
 *              In this example, pin P1-7 on the IO Expander 2 header is
 *              configured as a digital output and used to blink an LED.
 *              The LED is connected through a current-limiting resistor and
 *              toggled every second using the Inkplate library's expander1
 *              interface.
 *
 *              Inkplate boards provide two MCP23017-based I/O expanders. The
 *              pins are addressed using GPA/GPB numbering internally. The
 *              mapping is:
 *
 *              GPA0-GPA7 -> pins 0-7
 *              GPB0-GPB7 -> pins 8-15
 *
 *              These correspond to headers labeled P0-0 ... P0-7 and
 *              P1-0 ... P1-7 on the board.
 *
 *              The display is not used in this example (no screen updates
 *              occur). If no expander is explicitly selected, the default target
 *              is the IO Expander 2 header on the board. I/O expanders
 *              communicate via I2C, so switching speeds are slower than direct
 *              ESP32 GPIO pins. Expected output is the LED connected to P1-7
 *              blinking with a 1-second interval.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable, LED, 330 Ohm resistor
 * - Extra:      none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Connect the LED anode through the 330 Ohm resistor to P1-7 (IO Expander 2
 *    header) and the LED cathode to GND.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) After startup, the I/O expander pin is configured as a digital output.
 * 5) The LED toggles ON and OFF every second.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
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
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
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
