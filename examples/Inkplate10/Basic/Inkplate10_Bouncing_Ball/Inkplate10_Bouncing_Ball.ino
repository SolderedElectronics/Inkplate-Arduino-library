/**
 **************************************************
 * @file        Inkplate10_Bouncing_Ball.ino
 * @brief       Bouncing ball partial update example for Soldered
 *              Inkplate 10.
 *
 * @details     Bounces a ball around the Inkplate 10 screen with
 *              partial updates and counts the updates on the screen.
 *              A full refresh is forced every few partial updates to
 *              clear the ghosting that partial updates leave behind.
 *
 *              Partial update is available only in 1-bit (black &
 *              white) mode. Expected output is a ball that moves
 *              around the screen together with an update counter.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) The ball bounces around the display and the update counter
 *    grows.
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

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// The ball: its radius and its centre, in pixels
const int radius = 40;
int ballX = 200;
int ballY = 150;

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

  // Black letters on white, in the font from T1, at its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);

  // Make every fiftieth update a full refresh, to clear the leftovers
  display.setFullUpdateThreshold(49);
}

// loop() runs over and over
void loop() {
  // Draw the frame: a border around the screen, the ball and the update counter
  display.clearDisplay();
  display.drawRect(0, 0, 1200, 825, BLACK);
  display.fillCircle(ballX, ballY, radius, BLACK);
  display.setCursor(20, 20);
  display.print("Updates: ");
  display.print(updates);

  // Send only what changed
  display.partialUpdate(false, true);
  updates++;

  // Move the ball
  ballX += speedX;
  ballY += speedY;

  // Turn the ball around when it touches a side. Setting the position back to the
  // edge keeps it from sticking in the wall.
  if (ballX < radius) {
    ballX = radius;
    speedX = -speedX;
  }
  if (ballX > 1199 - radius) {
    ballX = 1199 - radius;
    speedX = -speedX;
  }
  if (ballY < radius) {
    ballY = radius;
    speedY = -speedY;
  }
  if (ballY > 824 - radius) {
    ballY = 824 - radius;
    speedY = -speedY;
  }
}
