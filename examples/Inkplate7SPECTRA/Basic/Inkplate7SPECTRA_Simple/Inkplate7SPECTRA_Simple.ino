/*
   Inkplate7SPECTRA_Simple example for Soldered Inkplate 7SPECTRA
   Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
   https://docs.soldered.com/inkplate/7spectra/quick-start-guide/

   Simple Inkplate example showing drawing functionalities of the Inkplate 7SPECTRA library.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   Looking to get support? Write on our forums: https://forum.soldered.com/
   23 January 2026 by Soldered
*/

#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
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
    display.fillRect(0, 0, 50, 50, INKPLATE_BLACK);
    display.fillRect(0, 50, 50, 50, INKPLATE_WHITE);
    display.fillRect(0, 100, 50, 50, INKPLATE_GREEN-1);
    display.fillRect(0, 150, 50, 50, INKPLATE_BLUE-1);
    display.fillRect(0, 200, 50, 50, INKPLATE_RED);
    display.fillRect(0, 250, 50, 50, INKPLATE_YELLOW);

    display.drawRect(55, 0, 50, 50, INKPLATE_BLACK);
    display.drawRect(55, 50, 50, 50, INKPLATE_WHITE);
    display.drawRect(55, 100, 50, 50, INKPLATE_GREEN-1);
    display.drawRect(55, 150, 50, 50, INKPLATE_BLUE-1);
    display.drawRect(55, 200, 50, 50, INKPLATE_RED);
    display.drawRect(55, 250, 50, 50, INKPLATE_YELLOW);

    // Draw many circles
    display.fillCircle(135, 25, 24, INKPLATE_BLACK);
    display.fillCircle(135, 75, 24, INKPLATE_WHITE);
    display.fillCircle(135, 125, 24, INKPLATE_GREEN-1);
    display.fillCircle(135, 175, 24, INKPLATE_BLUE-1);
    display.fillCircle(135, 225, 24, INKPLATE_RED);
    display.fillCircle(135, 275, 24, INKPLATE_YELLOW);

    display.drawCircle(190, 25, 24, INKPLATE_BLACK);
    display.drawCircle(190, 75, 24, INKPLATE_WHITE);
    display.drawCircle(190, 125, 24, INKPLATE_GREEN-1);
    display.drawCircle(190, 175, 24, INKPLATE_BLUE-1);
    display.drawCircle(190, 225, 24, INKPLATE_RED);
    display.drawCircle(190, 275, 24, INKPLATE_YELLOW);;

    // Draw many triangles
    display.fillTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.fillTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.fillTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN-1);
    display.fillTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE-1);
    display.fillTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.fillTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);

    display.drawTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.drawTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.drawTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN-1);
    display.drawTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE-1);
    display.drawTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.drawTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);

    // Show some pretty text
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(265, 0);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    display.setTextColor(INKPLATE_WHITE);
    display.setCursor(265, 50);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    display.setTextColor(INKPLATE_GREEN-1);
    display.setCursor(265, 100);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    display.setTextColor(INKPLATE_BLUE-1);
    display.setCursor(265, 150);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    display.setTextColor(INKPLATE_RED);
    display.setCursor(265, 200);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    display.setTextColor(INKPLATE_YELLOW);
    display.setCursor(265, 250);
    display.setTextSize(1);
    display.print("Welcome to Inkplate 7SPECTRA!");

    // Draw logo
    display.drawBitmap(0, 350, logo, logo_w, logo_h, INKPLATE_BLACK);
    display.drawBitmap(100, 350, logo, logo_w, logo_h, INKPLATE_GREEN-1);
    display.drawBitmap(200, 350, logo, logo_w, logo_h, INKPLATE_BLUE-1);
    display.drawBitmap(300, 350, logo, logo_w, logo_h, INKPLATE_RED);
    display.drawBitmap(400, 350, logo, logo_w, logo_h, INKPLATE_YELLOW);

    display.display();
}

void loop()
{
}
