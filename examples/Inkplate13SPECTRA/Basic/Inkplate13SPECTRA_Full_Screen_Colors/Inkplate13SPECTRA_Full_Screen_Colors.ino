/**
 **************************************************
 * @file        Inkplate13SPECTRA_Full_Screen_Colors.ino
 * @brief       Full screen colour example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Simple Inkplate example showing all colours of the Inkplate.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) Each colour of the Spectra 6 palette is shown full screen in turn.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-01-23
 **************************************************/

#include "Inkplate.h"

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate Inkplate 13SPECTRA in the boards menu."
#endif

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.clearDisplay();
    // Draw a full screen of all colors
    display.fillRect(0, 0, 1600 / 6 + 2, 1200, INKPLATE_BLACK);
    display.fillRect(1 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_WHITE);
    display.fillRect(2 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_YELLOW);
    display.fillRect(3 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_RED);
    display.fillRect(4 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_BLUE-1);
    display.fillRect(5 * 1600 / 6, 0, 1600 / 6 + 2, 1200, INKPLATE_GREEN-1);

    // Show the Image on the screen
    display.display();
}

void loop()
{
    // Loop forever
}
