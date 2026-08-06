/**
 **************************************************
 * @file        Inkplate6_Partial_Update.ino
 * @brief       Partial update text scrolling example for Soldered Inkplate 6.
 *
 * @details     Demonstrates how to use partial update functionality on the
 *              Inkplate 6 e-paper display in 1-bit (black & white) mode.
 *              The example scrolls a text string across the screen by updating
 *              only the changed areas using partialUpdate(). A full refresh is
 *              periodically forced (every N partial updates) to maintain good
 *              image quality.
 *
 *              Partial update is available only in 1-bit (black & white) mode
 *              and is not recommended for the first refresh after power-up.
 *              Perform a full refresh every 5-10 partial updates to maintain
 *              good picture quality. partialUpdate(_forced, leaveOn) takes two
 *              optional arguments: _forced is for advanced use (e.g. deep sleep
 *              workflows) to force a partial update, and leaveOn keeps the
 *              e-paper power rails enabled between updates for faster
 *              refreshes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6.
 * 3) The display performs an initial full refresh, then text scrolls across the
 *    screen using partial updates.
 * 4) A full refresh is automatically forced after a defined number of partial
 *    updates, reducing ghosting and maintaining display quality.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Char array where you can store your text that will be scrolled.
const char text[] = "This is partial update on Inkplate 6 e-paper display! :)";

// This variable is used for moving the text (scrolling)
int offset = 800;

//This variable is used to define the number of partial updates before doing a full update
int partialUpdates=9;

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextColor(BLACK, WHITE); // Set text color to be black and background color to be white
    display.setTextSize(4);             // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextWrap(false);         // Disable text wraping
    /*
    Set the number of partial updates before doing a full update
    This function forces a full update as the next update to ensure that the cycle of partial 
    updates starts from a fully updated screen.
    The Inkplate class keeps a internal counter that increments every time partialUpdate() gets called.
    */
    display.setFullUpdateThreshold(partialUpdates); 
}

void loop()
{
    // BASIC USAGE

    display.clearDisplay();         // Clear content in frame buffer
    display.setCursor(offset, 300); // Set new position for text
    display.print(text);            // Write text at new position
    /*
    //Updates changes parts of the screen without the need to refresh the whole display
    //partialUpdate(bool _forced, bool leaveOn)
	    _forced		Can force partial update in deep sleep (for advanced use)
	    leaveOn 	If set to 1, it will disable turning power supply for eink after display update in order to increase refresh time
    */
    display.partialUpdate(false, true); 
    offset -= 20; // Move text into new position
    if (offset < 0)
        offset = 800; // Text is scrolled till the end of the screen? Get it back on the start!
    delay(500);       // Delay between refreshes.

}
