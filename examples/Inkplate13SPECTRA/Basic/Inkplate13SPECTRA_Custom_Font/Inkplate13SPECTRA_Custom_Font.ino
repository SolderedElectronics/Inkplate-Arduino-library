/**
 **************************************************
 * @file        Inkplate13SPECTRA_Custom_Font.ino
 * @brief       Custom font printing example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to print text using a custom font instead of the
 *              default one.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The text is drawn with the custom font and the display refreshes.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-23
 **************************************************/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"
#include "FreeMono9pt7b.h"

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.display();
    display.setFont(&FreeMono9pt7b);
    display.setCursor(100, 100);
    display.setTextColor(INKPLATE_BLACK);
    display.setTextSize(3);
    display.print("Hello World!");
    display.display();
}

void loop()
{
}
