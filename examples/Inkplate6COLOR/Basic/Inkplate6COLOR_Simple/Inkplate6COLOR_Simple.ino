/**
 **************************************************
 * @file        Inkplate6COLOR_Simple.ino
 * @brief       Demonstrates basic drawing functions of the Inkplate 6COLOR
 *              graphics library.
 *
 * @details     This example showcases the core drawing capabilities available
 *              in the Inkplate 6COLOR Arduino library. It demonstrates how to
 *              render different graphical primitives and text using the
 *              display framebuffer before performing a full refresh of the
 *              e-paper panel.
 *
 *              The sketch draws multiple shapes using the supported display
 *              colors, including filled and outlined rectangles, circles,
 *              and triangles. It also prints colored text strings and renders
 *              bitmap graphics from an included image file (logoImg.h).
 *
 *              The example provides a quick overview of the graphics API and
 *              serves as a reference for developers building custom user
 *              interfaces, dashboards, or graphical applications on Inkplate.
 *              All drawing operations occur in the framebuffer and appear on the
 *              display only after display() is called. Colour e-paper displays
 *              require a full refresh for updates, which is slower than
 *              monochrome partial-update workflows on supported boards. Bitmap
 *              images must be converted to compatible header arrays before use
 *              in embedded applications.
 *
 *              Expected output: a graphical test screen showing coloured
 *              rectangles, circles, triangles, repeated text in different
 *              colours, and multiple instances of the Inkplate logo bitmap.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      bitmap header file (logoImg.h) in the sketch folder
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to the board.
 * 3) The example draws various shapes, coloured text and bitmap graphics in
 *    the framebuffer.
 * 4) The display is refreshed once to render the complete graphics demo.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-17
 * @license     GNU GPL V3
 **************************************************/

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
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
    display.fillRect(0, 100, 50, 50, INKPLATE_GREEN);
    display.fillRect(0, 150, 50, 50, INKPLATE_BLUE);
    display.fillRect(0, 200, 50, 50, INKPLATE_RED);
    display.fillRect(0, 250, 50, 50, INKPLATE_YELLOW);
    display.fillRect(0, 300, 50, 50, INKPLATE_ORANGE);

    display.drawRect(55, 0, 50, 50, INKPLATE_BLACK);
    display.drawRect(55, 50, 50, 50, INKPLATE_WHITE);
    display.drawRect(55, 100, 50, 50, INKPLATE_GREEN);
    display.drawRect(55, 150, 50, 50, INKPLATE_BLUE);
    display.drawRect(55, 200, 50, 50, INKPLATE_RED);
    display.drawRect(55, 250, 50, 50, INKPLATE_YELLOW);
    display.drawRect(55, 300, 50, 50, INKPLATE_ORANGE);

    // Draw many circles
    display.fillCircle(135, 25, 24, INKPLATE_BLACK);
    display.fillCircle(135, 75, 24, INKPLATE_WHITE);
    display.fillCircle(135, 125, 24, INKPLATE_GREEN);
    display.fillCircle(135, 175, 24, INKPLATE_BLUE);
    display.fillCircle(135, 225, 24, INKPLATE_RED);
    display.fillCircle(135, 275, 24, INKPLATE_YELLOW);
    display.fillCircle(135, 325, 24, INKPLATE_ORANGE);

    display.drawCircle(190, 25, 24, INKPLATE_BLACK);
    display.drawCircle(190, 75, 24, INKPLATE_WHITE);
    display.drawCircle(190, 125, 24, INKPLATE_GREEN);
    display.drawCircle(190, 175, 24, INKPLATE_BLUE);
    display.drawCircle(190, 225, 24, INKPLATE_RED);
    display.drawCircle(190, 275, 24, INKPLATE_YELLOW);
    display.drawCircle(190, 325, 24, INKPLATE_ORANGE);

    // Draw many triangles
    display.fillTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.fillTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.fillTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN);
    display.fillTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE);
    display.fillTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.fillTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);
    display.fillTriangle(210, 350, 260, 350, 235, 300, INKPLATE_ORANGE);

    display.drawTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.drawTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.drawTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN);
    display.drawTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE);
    display.drawTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.drawTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);
    display.drawTriangle(210, 350, 260, 350, 235, 300, INKPLATE_ORANGE);

    // Show some pretty text
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(265, 0);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_WHITE);
    display.setCursor(265, 50);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_GREEN);
    display.setCursor(265, 100);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_BLUE);
    display.setCursor(265, 150);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_RED);
    display.setCursor(265, 200);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_YELLOW);
    display.setCursor(265, 250);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    display.setTextColor(INKPLATE_ORANGE);
    display.setCursor(265, 300);
    display.setTextSize(2);
    display.print("Welcome to Inkplate 6COLOR!");

    // Draw logo
    display.drawBitmap(0, 350, logo, logo_w, logo_h, INKPLATE_BLACK);
    display.drawBitmap(100, 350, logo, logo_w, logo_h, INKPLATE_GREEN);
    display.drawBitmap(200, 350, logo, logo_w, logo_h, INKPLATE_BLUE);
    display.drawBitmap(300, 350, logo, logo_w, logo_h, INKPLATE_RED);
    display.drawBitmap(400, 350, logo, logo_w, logo_h, INKPLATE_YELLOW);
    display.drawBitmap(500, 350, logo, logo_w, logo_h, INKPLATE_ORANGE);

    display.display();
}

void loop()
{
}
