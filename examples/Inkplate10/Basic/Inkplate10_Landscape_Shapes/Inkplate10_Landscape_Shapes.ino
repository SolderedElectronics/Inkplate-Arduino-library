/**
 **************************************************
 * @file        Inkplate10_Landscape_Shapes.ino
 * @brief       Landscape drawing example with basic shapes for
 *              Soldered Inkplate 10.
 *
 * @details     Draws a landscape on the Inkplate 10 with every basic
 *              shape of the Adafruit GFX compatible drawing
 *              functions. It shows how simple shapes combine into a
 *              picture.
 *
 *              Expected output is a landscape drawn from ten shapes
 *              on the Inkplate screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) After initialization, the landscape appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the Inkplate library
#include "Inkplate.h"

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// A cloud is three overlapping circles. Draw the outlines first, then fill every
// circle with white, a little smaller. The fills rub out the outline lines inside
// the cloud, and only the outer edge is left.
void drawCloud(int x, int y) {
  display.drawCircle(x, y, 28, BLACK);
  display.drawCircle(x + 40, y - 18, 38, BLACK);
  display.drawCircle(x + 88, y, 30, BLACK);
  display.fillCircle(x, y, 25, WHITE);
  display.fillCircle(x + 40, y - 18, 35, WHITE);
  display.fillCircle(x + 88, y, 27, WHITE);
}

// A mountain is a triangle. Fill it with white first, so it covers whatever is
// behind it, then draw its outline on top.
void drawMountain(int left, int right, int peakX, int peakY) {
  display.fillTriangle(left, 700, right, 700, peakX, peakY, WHITE);
  display.drawTriangle(left, 700, right, 700, peakX, peakY, BLACK);
}

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();

  // The sun: fillCircle(x, y, radius, colour) is solid, drawCircle(...) is an outline.
  // x, y is the centre of the circle.
  display.fillCircle(1030, 130, 55, BLACK);
  display.drawCircle(1030, 130, 85, BLACK);

  // Two clouds
  drawCloud(150, 150);
  drawCloud(520, 90);

  // Mountains, from the back to the front
  drawMountain(0, 600, 300, 380);
  drawMountain(400, 1120, 760, 330);
  drawMountain(850, 1200, 1050, 470);

  // The ground: drawLine(x1, y1, x2, y2, colour) joins two points.
  display.drawLine(0, 700, 1199, 700, BLACK);

  // Speckles on the ground: drawPixel(x, y, colour) sets one pixel.
  for (int x = 10; x < 1200; x += 23) {
    display.drawPixel(x, 715 + (x * 7) % 29, BLACK);
  }

  // The house. drawRect(x, y, width, height, colour) is an outline and fillRect(...) is
  // solid. The chimney is drawn first, so the roof covers its lower part.
  display.fillRect(630, 420, 30, 70, BLACK);
  display.fillRect(380, 520, 320, 180, WHITE);
  display.drawRect(380, 520, 320, 180, BLACK);
  display.fillTriangle(350, 520, 730, 520, 540, 400, WHITE);
  display.drawTriangle(350, 520, 730, 520, 540, 400, BLACK);

  // Smoke from the chimney, three circles that grow
  display.drawCircle(646, 400, 8, BLACK);
  display.drawCircle(662, 370, 12, BLACK);
  display.drawCircle(690, 335, 16, BLACK);

  // The window has rounded corners, and two lines make the panes.
  // drawRoundRect(x, y, width, height, radius, colour), and fillRoundRect(...) is solid.
  display.drawRoundRect(420, 560, 100, 90, 10, BLACK);
  display.drawLine(470, 560, 470, 649, BLACK);
  display.drawLine(420, 605, 519, 605, BLACK);

  // The door
  display.fillRoundRect(580, 600, 70, 100, 10, BLACK);

  // The path from the door to the bottom of the screen
  display.drawLine(580, 700, 540, 824, BLACK);
  display.drawLine(650, 700, 720, 824, BLACK);

  // The fence: a loop draws the posts, two lines are the rails
  for (int x = 780; x <= 1000; x += 30) {
    display.drawLine(x, 650, x, 700, BLACK);
  }
  display.drawLine(780, 665, 1000, 665, BLACK);
  display.drawLine(780, 685, 1000, 685, BLACK);

  // The tree: a trunk and two triangles
  display.fillRect(1082, 640, 26, 60, BLACK);
  display.fillTriangle(1030, 650, 1160, 650, 1095, 540, BLACK);
  display.fillTriangle(1040, 580, 1150, 580, 1095, 490, BLACK);

  // Tufts of grass along the bottom, drawn with a loop
  for (int x = 30; x < 1200; x += 40) {
    if (x < 500 || x > 760) {
      display.drawLine(x, 810, x - 8, 785, BLACK);
      display.drawLine(x, 810, x + 8, 785, BLACK);
    }
  }

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
