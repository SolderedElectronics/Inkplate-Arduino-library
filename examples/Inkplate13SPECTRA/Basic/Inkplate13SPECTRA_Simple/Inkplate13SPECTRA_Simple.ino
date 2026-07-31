/*
   Inkplate13SPECTRA_Simple  example for Soldered Inkplate 13SPECTRA
   For this example you will need a micro USB cable and Inkplate 13SPECTRA.
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

   https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

   This example will show you how to draw various shapes, colored text, and a bitmap logo
   on the Inkplate 13SPECTRA display.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   23 January 2026 by Soldered Electronics
*/

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
    display.fillRect(0, 340, 150, 150, INKPLATE_GREEN-1);
    display.fillRect(0, 490, 150, 150, INKPLATE_BLUE-1);
    display.fillRect(0, 640, 150, 150, INKPLATE_RED);
    display.fillRect(0, 790, 150, 150, INKPLATE_YELLOW);

    display.drawRect(165, 40, 150, 150, INKPLATE_BLACK);
    display.drawRect(165, 190, 150, 150, INKPLATE_WHITE);
    display.drawRect(165, 340, 150, 150, INKPLATE_GREEN-1);
    display.drawRect(165, 490, 150, 150, INKPLATE_BLUE-1);
    display.drawRect(165, 640, 150, 150, INKPLATE_RED);
    display.drawRect(165, 790, 150, 150, INKPLATE_YELLOW);

    // Draw many circles
    display.fillCircle(405, 115, 72, INKPLATE_BLACK);
    display.fillCircle(405, 265, 72, INKPLATE_WHITE);
    display.fillCircle(405, 415, 72, INKPLATE_GREEN-1);
    display.fillCircle(405, 565, 72, INKPLATE_BLUE-1);
    display.fillCircle(405, 715, 72, INKPLATE_RED);
    display.fillCircle(405, 865, 72, INKPLATE_YELLOW);

    display.drawCircle(570, 115, 72, INKPLATE_BLACK);
    display.drawCircle(570, 265, 72, INKPLATE_WHITE);
    display.drawCircle(570, 415, 72, INKPLATE_GREEN-1);
    display.drawCircle(570, 565, 72, INKPLATE_BLUE-1);
    display.drawCircle(570, 715, 72, INKPLATE_RED);
    display.drawCircle(570, 865, 72, INKPLATE_YELLOW);

    // Draw many triangles
    display.fillTriangle(630, 190, 780, 190, 705, 40, INKPLATE_BLACK);
    display.fillTriangle(630, 340, 780, 340, 705, 190, INKPLATE_WHITE);
    display.fillTriangle(630, 490, 780, 490, 705, 340, INKPLATE_GREEN-1);
    display.fillTriangle(630, 640, 780, 640, 705, 490, INKPLATE_BLUE-1);
    display.fillTriangle(630, 790, 780, 790, 705, 640, INKPLATE_RED);
    display.fillTriangle(630, 940, 780, 940, 705, 790, INKPLATE_YELLOW);

    display.drawTriangle(630, 190, 780, 190, 705, 40, INKPLATE_BLACK);
    display.drawTriangle(630, 340, 780, 340, 705, 190, INKPLATE_WHITE);
    display.drawTriangle(630, 490, 780, 490, 705, 340, INKPLATE_GREEN-1);
    display.drawTriangle(630, 640, 780, 640, 705, 490, INKPLATE_BLUE-1);
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

    display.setTextColor(INKPLATE_GREEN-1);
    display.setCursor(795, 340);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 13SPECTRA!");

    display.setTextColor(INKPLATE_BLUE-1);
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
    display.drawBitmap(380, 1090, logo, logo_w, logo_h, INKPLATE_GREEN-1);
    display.drawBitmap(760, 1090, logo, logo_w, logo_h, INKPLATE_BLUE-1);
    display.drawBitmap(1140, 1090, logo, logo_w, logo_h, INKPLATE_RED);
    display.drawBitmap(1520, 1090, logo, logo_w, logo_h, INKPLATE_YELLOW);

    display.display();
}

void loop()
{
}
