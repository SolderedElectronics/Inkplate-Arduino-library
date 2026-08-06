/**
 **************************************************
 * @file        Inkplate4TEMPERA_Black_White.ino
 * @brief       Demonstrates Adafruit GFX drawing primitives and text/bitmap
 *              rendering in black & white mode on Inkplate 4 TEMPERA.
 *
 * @details     This example runs the Inkplate 4 TEMPERA in 1-bit mode using
 *              INKPLATE_1BIT, enabling BLACK and WHITE rendering on the
 *              e-paper panel. It demonstrates a selection of Adafruit
 *              GFX-compatible drawing operations (pixels, lines, thick lines,
 *              rectangles, circles, rounded rectangles, triangles, ellipses,
 *              polygons), plus rendering a bitmap/logo and showing text.
 *
 *              Each demo step draws into the ESP32 frame buffer first and then
 *              performs a full refresh using display.display(). A small status
 *              label is printed at the bottom-left to indicate the current demo
 *              step. The sketch finishes by continuously rotating a text
 *              message using setRotation().
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) The display cycles through the drawing demos, then keeps rotating the text
 *    message.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-12
 * @license     GNU GPL V3
 **************************************************/

#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);   // <-- 1-bit mode (BLACK/WHITE)

#define DELAY_MS 5000

#include "logo.h"

void setup()
{
    display.begin();
    display.clearDisplay();
    display.display();

    display.setCursor(60, 300);
    display.setTextSize(3);
    display.setTextColor(BLACK, WHITE);
    display.print("Welcome to Inkplate 4TEMPERA!");
    display.display();
    delay(5000);
}

void loop()
{
    // Single pixel
    display.clearDisplay();
    displayCurrentAction("Drawing a pixel");
    display.drawPixel(100, 50, BLACK);
    display.display();
    delay(DELAY_MS);

    // Random pixels
    display.clearDisplay();
    for (int i = 0; i < 600; i++)
        display.drawPixel(random(0, 599), random(0, 599), BLACK);
    displayCurrentAction("Drawing random pixels");
    display.display();
    delay(DELAY_MS);

    // Diagonal lines
    display.clearDisplay();
    display.drawLine(0, 0, 599, 599, BLACK);
    display.drawLine(599, 0, 0, 599, BLACK);
    displayCurrentAction("Diagonal lines");
    display.display();
    delay(DELAY_MS);

    // Random lines
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
        display.drawLine(random(0, 599), random(0, 599),
                         random(0, 599), random(0, 599), BLACK);
    displayCurrentAction("50 random lines");
    display.display();
    delay(DELAY_MS);

    // Thick lines
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
        display.drawThickLine(random(0, 599), random(0, 599),
                              random(0, 599), random(0, 599),
                              BLACK, (float)random(1, 20));
    displayCurrentAction("50 random thick lines");
    display.display();
    delay(DELAY_MS);

    // Horizontal line
    display.clearDisplay();
    display.drawFastHLine(100, 300, 400, BLACK);
    displayCurrentAction("Horizontal line");
    display.display();
    delay(DELAY_MS);

    // Vertical line
    display.clearDisplay();
    display.drawFastVLine(300, 100, 400, BLACK);
    displayCurrentAction("Vertical line");
    display.display();
    delay(DELAY_MS);

    // Grid
    display.clearDisplay();
    for (int i = 0; i < 600; i += 10)
        display.drawFastVLine(i, 0, 600, BLACK);
    for (int i = 0; i < 600; i += 10)
        display.drawFastHLine(0, i, 600, BLACK);
    displayCurrentAction("Grid");
    display.display();
    delay(DELAY_MS);

    // Rectangle
    display.clearDisplay();
    display.drawRect(150, 200, 300, 200, BLACK);
    displayCurrentAction("Rectangle outline");
    display.display();
    delay(DELAY_MS);

    // Filled rectangle
    display.clearDisplay();
    display.fillRect(150, 200, 300, 200, BLACK);
    displayCurrentAction("Filled rectangle");
    display.display();
    delay(DELAY_MS);

    // Circles
    display.clearDisplay();
    display.drawCircle(300, 300, 90, BLACK);
    display.fillCircle(300, 300, 60, BLACK);
    displayCurrentAction("Circle + filled circle");
    display.display();
    delay(DELAY_MS);

    // Rounded rectangle
    display.clearDisplay();
    display.drawRoundRect(150, 200, 300, 200, 12, BLACK);
    display.fillRoundRect(170, 220, 260, 160, 18, BLACK);
    displayCurrentAction("Rounded rectangle");
    display.display();
    delay(DELAY_MS);

    // Triangle
    display.clearDisplay();
    display.drawTriangle(150, 400, 450, 400, 300, 100, BLACK);
    display.fillTriangle(200, 350, 400, 350, 300, 150, BLACK);
    displayCurrentAction("Triangle");
    display.display();
    delay(DELAY_MS);

    // Bitmap logo
    display.clearDisplay();
    display.image.draw(logo, 43, 247, 513, 105, BLACK);
    displayCurrentAction("Logo");
    display.display();
    delay(DELAY_MS);

    // Text demo
    display.clearDisplay();
    for (int i = 0; i < 5; i++)
    {
        display.setTextSize(i + 1);
        display.setCursor(90, (i * i * 20));
        display.print("Inkplate 4TEMPERA");
    }
    displayCurrentAction("Text sizes");
    display.display();
    delay(DELAY_MS);

    // Ellipse
    display.clearDisplay();
    display.drawElipse(400, 200, 300, 300, BLACK);
    display.fillElipse(400, 200, 150, 150, BLACK);
    displayCurrentAction("Ellipse");
    display.display();
    delay(DELAY_MS);

    // Rotate text forever
    int r = 0;
    display.setTextSize(5);
    display.setTextColor(BLACK, WHITE);

    while (true)
    {
        display.clearDisplay();
        display.setCursor(120, 250);
        display.setRotation(r);
        display.print("Inkplate 4TEMPERA");
        display.display();
        r++;
        delay(DELAY_MS);
    }
}

void displayCurrentAction(String text)
{
    display.setTextSize(2);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(20, 560);
    display.print(text);
}