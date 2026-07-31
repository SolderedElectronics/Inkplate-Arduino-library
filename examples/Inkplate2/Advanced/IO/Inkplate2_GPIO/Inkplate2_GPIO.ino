/**
 **************************************************
 * @file        Inkplate2_GPIO.ino
 * @brief       Basic GPIO control example: blink an external LED using
 *              ESP32 GPIO on Inkplate 2.
 *
 * @details     This example demonstrates how to use general-purpose I/O (GPIO)
 *              pins available on the Inkplate 2 header. An external LED is
 *              connected to ESP32 GPIO14 through a current-limiting resistor.
 *              The sketch configures the selected pin as an OUTPUT and toggles
 *              it every second, creating a visible blink.
 *
 *              The e-paper display is used only to show instructions and runs
 *              in 1-bit (black/white) mode with a single full refresh during
 *              setup. Use only GPIO pins that are exposed on the Inkplate 2
 *              header and not reserved for internal hardware. Ensure correct
 *              polarity of the LED and always use a resistor to avoid damaging
 *              the GPIO pin.
 *
 *              Expected output: "Blink example" and wiring instructions on the
 *              display, and the LED connected to GPIO14 blinking continuously
 *              (1 s ON, 1 s OFF).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      LED, ~330 Ohm resistor, jumper wires, breadboard
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Connect the LED anode to GPIO14 through a 330 Ohm resistor and the LED
 *    cathode to GND.
 * 3) Upload the sketch to Inkplate 2.
 * 4) After reset, read the instructions on the display.
 * 5) Observe the LED blinking once per second.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-05-10
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"

// LED is connected to ESP32 GPIO 14
#define PIN_LED 14

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.setTextSize(1);    // Set text size
    display.setCursor(10, 20); // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Blink example");
    display.setCursor(10, 35); // Set cursor position
    display.println("Connect LED to ESP32 GPIO14 and LED will blink once every two seconds.");

    // Display to screen
    display.display();

    // Set LED GPIO to be output pin
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    digitalWrite(PIN_LED, HIGH); // Turn on LED
    delay(1000);                 // Wait a bit
    digitalWrite(PIN_LED, LOW);  // Turn off LED
    delay(1000);                 // Wait a bit
}
