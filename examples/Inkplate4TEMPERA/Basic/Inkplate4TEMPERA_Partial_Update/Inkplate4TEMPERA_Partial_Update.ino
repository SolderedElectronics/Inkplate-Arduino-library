/**
 **************************************************
 * @file        Inkplate4TEMPERA_Partial_Update.ino
 * @brief       Demonstrates 1-bit partial updates by scrolling text across the
 *              Inkplate 4 TEMPERA e-paper display.
 *
 * @details     This example runs the Inkplate 4 TEMPERA in 1-bit (BW) mode and
 *              scrolls a text string horizontally by repeatedly redrawing it at
 *              a new X position and calling partialUpdate().
 *
 *              Partial updates refresh only the changed areas of the screen,
 *              reducing flashing and improving perceived speed compared to full
 *              refreshes. To keep image quality stable, the sketch configures a
 *              full refresh threshold so that after a defined number of partial
 *              updates, the next update is forced to be a full refresh.
 *
 *              Partial update is supported only in 1-bit (BW) mode. Do not rely
 *              on partial update as the first refresh after power-up; perform a
 *              full refresh first (this sketch does an initial clear +
 *              display()). For best quality, do a full refresh every 5-10
 *              partial updates - this example uses setFullUpdateThreshold() to
 *              force a full refresh cycle. partialUpdate(false, true) keeps the
 *              e-paper power enabled after the update (leaveOn = true) for faster
 *              subsequent partial updates but increases power usage; use
 *              leaveOn = false when optimizing for energy.
 *
 *              Expected output: a single line of text moving horizontally,
 *              updated with minimal flashing compared to full refresh updates.
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
 * 3) The text will continuously scroll across the display.
 * 4) Adjust the scroll speed by changing the delay or offset step.
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

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Char array where you can store your text that will be scrolled.
const char text[] = "This is partial update on Inkplate 4TEMPERA e-paper display! :)";

// This variable determines the horizontal position of the text,
// creating a scrolling effect as it decreases.
int offset = 600;

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
