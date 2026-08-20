/**
 **************************************************
 * @file        Inkplate13SPECTRA_Simple.ino
 * @brief       Simple shapes, text and bitmap example for Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Shows how to draw various shapes, coloured text and a bitmap logo
 *              on the Inkplate 13SPECTRA display.
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
 * 3) The shapes, coloured text and bitmap logo are drawn and the display
 *    refreshes.
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
#include "logoImg.h"

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.display();

    display.fillScreen(INKPLATE_WHITE);

    // Draw many rectangles
    display.fillRect(0, 40, 150, 150, INKPLATE_BLACK);
    display.fillRect(0, 190, 150, 150, INKPLATE_WHITE);
    display.fillRect(0, 340, 150, 150, INKPLATE_GREEN);
    display.fillRect(0, 490, 150, 150, INKPLATE_BLUE);
    display.fillRect(0, 640, 150, 150, INKPLATE_RED);
    display.fillRect(0, 790, 150, 150, INKPLATE_YELLOW);

    display.drawRect(165, 40, 150, 150, INKPLATE_BLACK);
    display.drawRect(165, 190, 150, 150, INKPLATE_WHITE);
    display.drawRect(165, 340, 150, 150, INKPLATE_GREEN);
    display.drawRect(165, 490, 150, 150, INKPLATE_BLUE);
    display.drawRect(165, 640, 150, 150, INKPLATE_RED);
    display.drawRect(165, 790, 150, 150, INKPLATE_YELLOW);

    // Draw many circles
    display.fillCircle(405, 115, 72, INKPLATE_BLACK);
    display.fillCircle(405, 265, 72, INKPLATE_WHITE);
    display.fillCircle(405, 415, 72, INKPLATE_GREEN);
    display.fillCircle(405, 565, 72, INKPLATE_BLUE);
    display.fillCircle(405, 715, 72, INKPLATE_RED);
    display.fillCircle(405, 865, 72, INKPLATE_YELLOW);

    display.drawCircle(570, 115, 72, INKPLATE_BLACK);
    display.drawCircle(570, 265, 72, INKPLATE_WHITE);
    display.drawCircle(570, 415, 72, INKPLATE_GREEN);
    display.drawCircle(570, 565, 72, INKPLATE_BLUE);
    display.drawCircle(570, 715, 72, INKPLATE_RED);
    display.drawCircle(570, 865, 72, INKPLATE_YELLOW);

    // Draw many triangles
    display.fillTriangle(630, 190, 780, 190, 705, 40, INKPLATE_BLACK);
    display.fillTriangle(630, 340, 780, 340, 705, 190, INKPLATE_WHITE);
    display.fillTriangle(630, 490, 780, 490, 705, 340, INKPLATE_GREEN);
    display.fillTriangle(630, 640, 780, 640, 705, 490, INKPLATE_BLUE);
    display.fillTriangle(630, 790, 780, 790, 705, 640, INKPLATE_RED);
    display.fillTriangle(630, 940, 780, 940, 705, 790, INKPLATE_YELLOW);

    display.drawTriangle(630, 190, 780, 190, 705, 40, INKPLATE_BLACK);
    display.drawTriangle(630, 340, 780, 340, 705, 190, INKPLATE_WHITE);
    display.drawTriangle(630, 490, 780, 490, 705, 340, INKPLATE_GREEN);
    display.drawTriangle(630, 640, 780, 640, 705, 490, INKPLATE_BLUE);
    display.drawTriangle(630, 790, 780, 790, 705, 640, INKPLATE_RED);
    display.drawTriangle(630, 940, 780, 940, 705, 790, INKPLATE_YELLOW);

    // Show some pretty text
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(795, 40);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_WHITE);
    display.setCursor(795, 190);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_GREEN);
    display.setCursor(795, 340);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_BLUE);
    display.setCursor(795, 490);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_RED);
    display.setCursor(795, 640);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_YELLOW);
    display.setCursor(795, 790);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    // Draw logo
    display.drawBitmap(0, 1090, logo, logo_w, logo_h, INKPLATE_BLACK);
    display.drawBitmap(380, 1090, logo, logo_w, logo_h, INKPLATE_GREEN);
    display.drawBitmap(760, 1090, logo, logo_w, logo_h, INKPLATE_BLUE);
    display.drawBitmap(1140, 1090, logo, logo_w, logo_h, INKPLATE_RED);
    display.drawBitmap(1520, 1090, logo, logo_w, logo_h, INKPLATE_YELLOW);

    display.display();
}

void loop()
{
}
