/**
 **************************************************
 * @file        Inkplate6_Burn_In_Clean.ino
 * @brief       Run a burn-in cleaning cycle to reduce ghosting/burn-in on the
 *              Inkplate 6 e-paper panel.
 *
 * @details     This example calls Inkplate's burnInClean() routine to perform a
 *              repeated full-refresh cleaning sequence intended to reduce heavy
 *              ghosting (burn-in) artifacts on the e-paper panel. The cleaning
 *              runs for a configurable number of cycles (CLEAR_CYCLES) with a
 *              fixed delay between cycles (CYCLES_DELAY) to respect e-paper
 *              refresh limitations.
 *
 *              When the cleaning sequence finishes, a confirmation message is
 *              rendered on the display. Display mode is 1-bit (BW). This routine
 *              performs many full refreshes, so do not interrupt power during
 *              the cleaning sequence, and use CYCLES_DELAY >= 5 seconds to avoid
 *              overstressing the panel and to allow refresh waveforms to
 *              complete properly. Burn-in/ghosting reduction effectiveness
 *              depends on the panel condition and the content that caused the
 *              artifact; multiple runs may be required.
 *
 *              Expected output is the panel repeatedly full-refreshing during the
 *              cleaning routine, followed by the message "Clearing done."
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      none
 * - Serial:     none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Set CLEAR_CYCLES to the number of cleaning refresh cycles you want.
 * 3) Set CYCLES_DELAY (ms) between cycles (keep it >= 5000 ms).
 * 4) Upload the sketch and keep the device powered for the entire process.
 * 5) Wait until the screen shows "Clearing done."
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-02-19
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
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
