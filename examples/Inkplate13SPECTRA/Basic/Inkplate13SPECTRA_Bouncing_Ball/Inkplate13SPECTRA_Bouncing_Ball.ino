/**
 **************************************************
 * @file        Inkplate13SPECTRA_Bouncing_Ball.ino
 * @brief       Bounce a ball around a box on the Inkplate 13 with partial
 *              updates, and count the updates.
 *
 * @details     Connection: none, the screen is on the board. Power and upload
 *              over USB-C.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The sketch runs as described above.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// The display object
Inkplate display;

// The box the ball bounces in: top left corner, width and height
const int boxX = 0;
const int boxY = 0;
const int boxWidth = 1600;
const int boxHeight = 1200;

// The ball: its radius and its centre, in pixels
const int radius = 80;
int ballX = boxX + 200;
int ballY = boxY + 150;

// How far the ball moves on each update. A negative value means left or up.
int speedX = 30;
int speedY = 24;

// How many updates have been sent to the screen so far
int updates = 0;

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black letters in the font from T1, at its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(1);
}

// loop() runs over and over
void loop() {
  // Draw the frame: the box, the ball and the update counter
  display.fillRect(boxX, boxY, boxWidth, boxHeight, INKPLATE_WHITE);
  display.drawRect(boxX, boxY, boxWidth, boxHeight, INKPLATE_BLACK);
  display.fillCircle(ballX, ballY, radius, INKPLATE_RED);
  display.setCursor(boxX + 20, boxY + 20);
  display.print("Updates: ");
  display.print(updates);

  // Every 50th update is a full refresh. The others send only the box.
  updates++;
  if (updates % 50 == 0) {
    display.display();
  } else {
    display.displayPartial(boxX, boxY, boxWidth, boxHeight, true);
  }

  // Move the ball
  ballX += speedX;
  ballY += speedY;

  // Turn the ball around when it touches a side of the box. Setting the position back to
  // the edge keeps it from sticking in the wall.
  if (ballX < boxX + radius) {
    ballX = boxX + radius;
    speedX = -speedX;
  }
  if (ballX > boxX + boxWidth - 1 - radius) {
    ballX = boxX + boxWidth - 1 - radius;
    speedX = -speedX;
  }
  if (ballY < boxY + radius) {
    ballY = boxY + radius;
    speedY = -speedY;
  }
  if (ballY > boxY + boxHeight - 1 - radius) {
    ballY = boxY + boxHeight - 1 - radius;
    speedY = -speedY;
  }
}
