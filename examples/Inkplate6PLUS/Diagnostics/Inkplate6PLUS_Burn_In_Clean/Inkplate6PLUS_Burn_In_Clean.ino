/**
 **************************************************
 * @file        Inkplate6PLUS_Burn_In_Clean.ino
 * @brief       Burn-in cleaning routine for Soldered Inkplate 6PLUS.
 *
 * @details     This example will try to remove heavy burn-in visible on the
 *              panel. Set the number of refresh / clear cycles and upload the
 *              program.
 *
 * Requirements:
 * - Board:      e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "e-radionica Inkplate 6Plus"
 *    or "Soldered Inkplate 6Plus" from Tools -> Board.
 * 2) Set the number of refresh / clear cycles in the sketch.
 * 3) Upload the sketch to Inkplate 6PLUS.
 * 4) Keep the device powered until the cleaning cycles finish.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Adding the board definition to the Arduino IDE:
 *              https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/
 *
 * @author      Soldered
 * @date        2026-02-19
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"             // Include Inkplate library to the sketch
Inkplate inkplate(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochorme mode

// Nubmer of clear cycles.
#define CLEAR_CYCLES 20

// Delay between clear cycles (in milliseconds)
// NOTE: cycles delay should not be smaller than 5 seconds
#define CYCLES_DELAY 5000

void setup()
{
  // Uncomment this line if you have a USB Power Only Inkplate6PLUS
  // Must be called before inkplate.begin()!
  //inkplate.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
  inkplate.begin();        // Init library (you should call this function ONLY ONCE)
  inkplate.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

  int cycles = CLEAR_CYCLES;

  // Clean the screen by running the burn in function which starts the cleaning sequence
  inkplate.burnInClean(cycles, CYCLES_DELAY);

  // Print text when clearing is done.
  inkplate.setTextSize(4);
  inkplate.setCursor(100, 100);
  inkplate.print("Clearing done.");
  inkplate.display();
}

void loop()
{
  // Empty...
}