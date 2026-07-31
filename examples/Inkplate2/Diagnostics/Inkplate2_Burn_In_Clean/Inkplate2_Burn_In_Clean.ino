/**
 **************************************************
 * @file        Inkplate2_Burn_In_Clean.ino
 * @brief       Burn-in cleaning routine for Soldered Inkplate 2.
 *
 * @details     This example will try to remove heavy burn-in visible on the
 *              panel. Set the number of refresh / clear cycles and upload the
 *              program.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Set the number of refresh / clear cycles in the sketch.
 * 3) Upload the sketch to Inkplate 2.
 * 4) Keep the device powered until the cleaning cycles finish.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Adding the board definition to the Arduino IDE:
 *              https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/
 *
 * @author      e-radionica.com
 * @date        2022-01-12
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
Inkplate display;     // Create object on Inkplate library

// Nubmer of clear cycles.
#define CLEAR_CYCLES 5

// Delay between clear cycles (in milliseconds)
#define CYCLES_DELAY 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.

    int cycles = CLEAR_CYCLES;  // Set the number of clear cycles

    // Clean it by writing clear sequence to the panel.
    while (cycles)
    {
        cycles--;
        display.display();
        delay(CYCLES_DELAY);
    }

    // Print text when clearing is done.
    display.setTextSize(4);
    display.setCursor(10, 15);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.print("Clearing done.");
    display.display();
}

void loop()
{
    // Empty...
}
