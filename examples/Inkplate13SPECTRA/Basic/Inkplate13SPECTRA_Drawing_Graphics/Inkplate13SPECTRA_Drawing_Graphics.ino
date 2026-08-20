/**
 **************************************************
 * @file        Inkplate13SPECTRA_Drawing_Graphics.ino
 * @brief       Graphics drawing example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to draw shapes, lines and text using the Adafruit GFX
 *              functions built into the Inkplate library.
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
 * 3) The shapes, lines and text are drawn and the display refreshes.
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

// Declare Inkplate object
Inkplate display;

void setup()
{
    // Initialize Inkplate
    display.begin();

    display.clearDisplay();
    display.fillScreen(INKPLATE_WHITE);

    // Rectangles
    display.drawRect(40, 40, 520, 240, INKPLATE_BLACK);
    display.fillRect(60, 60, 200, 80, INKPLATE_YELLOW);
    display.drawRect(60, 60, 200, 80, INKPLATE_BLACK);
    display.fillRoundRect(290, 60, 240, 80, 16, INKPLATE_RED);
    display.drawRoundRect(290, 60, 240, 80, 16, INKPLATE_BLACK);

    // Circles
    display.drawCircle(160, 220, 60, INKPLATE_BLUE);
    display.fillCircle(380, 220, 60, INKPLATE_GREEN);
    display.drawCircle(380, 220, 60, INKPLATE_BLACK);

    // Triangle
    display.fillTriangle(520, 320, 640, 160, 760, 320, INKPLATE_YELLOW);
    display.drawTriangle(520, 320, 640, 160, 760, 320, INKPLATE_BLACK);

    // Lines
    display.drawLine(40, 360, 760, 520, INKPLATE_BLACK);
    display.drawLine(40, 390, 760, 550, INKPLATE_RED);
    display.drawLine(40, 420, 760, 580, INKPLATE_YELLOW);
    display.drawLine(40, 450, 760, 610, INKPLATE_BLUE);
    display.drawLine(40, 480, 760, 640, INKPLATE_GREEN);

    // Simple text
    display.setTextSize(3);
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(40, 760);
    display.print("Inkplate 13SPECTRA");

    display.setTextSize(2);
    display.setCursor(40, 800);
    display.print("Adafruit_GFX shapes & colors");

    // Update e-paper
    display.display();
}

void loop()
{
    // Loop forever
}
