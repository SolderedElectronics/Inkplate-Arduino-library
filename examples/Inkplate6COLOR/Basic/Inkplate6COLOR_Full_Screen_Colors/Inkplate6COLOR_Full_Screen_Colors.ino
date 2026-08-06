/**
 **************************************************
 * @file        Inkplate6COLOR_Full_Screen_Colors.ino
 * @brief       Displays all supported Inkplate 6COLOR panel colors as vertical
 *              full-screen bars.
 *
 * @details     This example demonstrates the basic color capabilities of the
 *              Inkplate 6COLOR e-paper display. The sketch fills the screen
 *              with vertical color bars representing each color supported by
 *              the panel.
 *
 *              The display is divided into equal-width vertical regions and
 *              filled sequentially using the Inkplate color constants. This
 *              provides a quick visual reference for the available display
 *              palette and confirms that the panel is functioning correctly.
 *
 *              The example is useful for testing a newly assembled board,
 *              verifying panel color reproduction, or understanding the
 *              available color set when designing UI layouts. It performs a
 *              single full refresh and does not change afterwards; colour
 *              e-paper refreshes are slower than monochrome displays and should
 *              not be refreshed frequently in typical applications.
 *
 *              Expected output: full-screen vertical bars showing the Inkplate
 *              colour palette - black, white, green, blue, red, yellow and
 *              orange.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     not used in this example
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to the board.
 * 3) After initialization, the display is filled with vertical colour bars.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-09-27
 * @license     GNU GPL V3
 **************************************************/

#include "Inkplate.h"

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    // Draw a full screen of all colors
    display.fillRect(0, 0, 600 / 7 + 2, 448, INKPLATE_BLACK);
    display.fillRect(1 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_WHITE);
    display.fillRect(2 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_GREEN);
    display.fillRect(3 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_BLUE);
    display.fillRect(4 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_RED);
    display.fillRect(5 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_YELLOW);
    display.fillRect(6 * 600 / 7, 0, 600 / 7 + 2, 448, INKPLATE_ORANGE);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}
