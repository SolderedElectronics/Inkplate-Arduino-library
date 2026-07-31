/**
 **************************************************
 * @file        Inkplate2_Text_With_Shadow.ino
 * @brief       Adafruit GFX drawing demo: render tri-color text and a shadowed
 *              string, then enter deep sleep.
 *
 * @details     This example demonstrates basic graphics and text rendering on
 *              Inkplate 2 using Adafruit GFX-compatible drawing functions. It
 *              prints standard text in black and red, then uses
 *              drawTextWithShadow() to render a red string with a black shadow.
 *              An additional small signature line is drawn with inverted
 *              shadow/text colors, and a simple red line is drawn to show other
 *              primitives supported by the library.
 *
 *              Inkplate 2 uses a 1-bit display mode with a tri-color palette
 *              (BLACK/WHITE/RED). The sketch performs a single full refresh to
 *              show the drawing and then enters deep sleep. Because deep sleep
 *              resets the ESP32, the program restarts from setup() after any
 *              wake/reset; this sketch does not configure a wake source, so a
 *              reset or power cycle is required to run it again. The Inkplate
 *              Arduino library is compatible with Adafruit GFX primitives, making
 *              it easy to reuse many existing GFX-based drawing examples.
 *
 *              Expected output: "Inkplate 2" in black, then in red, then a red
 *              "Inkplate 2" with a black shadow; a small "By soldered.com" line
 *              with a shadow; and a red diagonal line.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      none
 * - Serial:     none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 2.
 * 3) After upload/reset, the display refreshes once and shows the rendered
 *    text, shadow effects, and a line.
 * 4) The board enters deep sleep after updating the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-02-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"

// Initialize Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate library
    display.begin();

    display.setTextSize(2);                // Set text size
    display.setTextColor(INKPLATE2_BLACK); // Set text color
    display.setCursor(0, 0);               // Set cursor position

    // Draw black text
    display.print("Inkplate 2"); // Print text

    // Draw red text
    display.setTextColor(INKPLATE2_RED);
    display.setCursor(0, 20);
    display.print("Inkplate 2");

    // Draw text with shadow, specify color for text and background
    // (x coordinate, y coordinate, string to write, text color, shadow color)
    display.drawTextWithShadow(0, 40, "Inkplate 2", INKPLATE2_RED, INKPLATE2_BLACK);

    display.setTextSize(1);
    display.drawTextWithShadow(110, 80, "By soldered.com", INKPLATE2_BLACK, INKPLATE2_RED);

    // Other basic drawing functions supported
    display.drawLine(10, 80, 100, 100, INKPLATE2_RED);

    // Display to screen
    display.display();

    // Go to deep sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Empty...
}
