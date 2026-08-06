/**
 **************************************************
 * @file        Inkplate6PLUS_Partial_Update.ino
 * @brief       Partial update scrolling text demo for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to use the partial update functionality of the
 *              Inkplate 6PLUS e-paper display. Partial updates refresh only
 *              the changed portions of the screen, making updates significantly
 *              faster and reducing power consumption compared to full refreshes.
 *              This example scrolls a line of text across the display using
 *              repeated partial updates.
 *
 *              Partial updates are available only in 1-bit display mode. It is
 *              recommended not to use partial update immediately after power-up -
 *              perform a full refresh first. To prevent image ghosting, a full
 *              refresh should occur after several partial updates (e.g. every
 *              5-10 updates); this example uses setFullUpdateThreshold() to
 *              manage that automatically. The leaveOn parameter keeps the
 *              e-paper power supply active to speed up consecutive updates.
 *
 *              Expected output: a scrolling text message moving from right to
 *              left across the screen, with periodic full refreshes
 *              automatically triggered after a set number of partial updates to
 *              maintain image quality.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6PLUS"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) The display continuously scrolls a line of text horizontally.
 * 4) Only the changed parts of the display are refreshed using partial updates.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Char array where you can store your text that will be scrolled.
const char text[] = "This is partial update on Inkplate 6PLUS e-paper display! :)";

// This variable is used for moving the text (scrolling)
int offset = 800;

//This variable is used to define the number of partial updates before doing a full update
int partialUpdates=9;

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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


