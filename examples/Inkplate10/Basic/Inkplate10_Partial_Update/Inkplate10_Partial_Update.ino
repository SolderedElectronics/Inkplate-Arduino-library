/*
   Inkplate10_Partial_Update example for Soldered Inkplate 10
   For this example you will need only USB cable and Inkplate 10
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/
   
   In this example we will show  how to use partial update functionality of Inkplate 10 e-paper display.
   It will scroll text that is saved in char array
   NOTE: Partial update is only available on 1 Bit mode (BW) and it is not recommended to use it on first refresh after
   power up. It is recommended to do a full refresh every 5-10 partial refresh to maintain good picture quality.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 February 2021 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Char array where you can store your text that will be scrolled.
const char text[] = "This is partial update on Inkplate 10 e-paper display! :)";

// This variable is used for moving the text (scrolling)
int offset = 1200;

// Variable that keeps count on how much screen has been partially updated
int n = 0;

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextColor(BLACK, WHITE); // Set text color to be black and background color to be white
    display.setTextSize(4);             // Set text to be 4 times bigger than classic 5x7 px text
    display.setTextWrap(false);         // Disable text wraping
}

void loop()
{
    // BASIC USAGE

    display.clearDisplay();         // Clear content in frame buffer
    display.setCursor(offset, 400); // Set new position for text
    display.print(text);            // Write text at new position
    if (n > 9)
    {                      // Check if you need to do full refresh or you can do partial update
        display.display(); // Do a full refresh
        n = 0;
    }
    else
    {
        display.partialUpdate(); // Do partial update
        n++;                     // Keep track on how many times screen has been partially updated
    }
    offset -= 20; // Move text into new position
    if (offset < 0)
        offset = 1200; // Text is scrolled till the end of the screen? Get it back on the start!
    delay(500);        // Delay between refreshes.

    // ADVANCED USAGE

    display.clearDisplay();         // Clear content in frame buffer
    display.setCursor(offset, 400); // Set new position for text
    display.print(text);            // Write text at new position

    display.einkOn(); // Turn on e-ink display
    if (n > 9)        // Check if you need to do full refresh or you can do partial update
    {
        display.einkOff(); // Turn off e-ink display after partial updates
        display.display(); // Do a full refresh
        n = 0;
    }
    else
    {
        display.partialUpdate(false, true); // Do partial update
        n++;                                // Keep track on how many times screen has been partially updated
    }
    offset -= 20; // Move text into new position
    if (offset < 0)
        offset = 1200; // Text is scrolled till the end of the screen? Get it back on the start!
    delay(500);        // Delay between refreshes.
}
