/*
   Basic_custom_font example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how to use custom fonts on Inkplate 6 thanks to Adafruit GFX
   More on custom fonts in Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
   In this example, we will use already prebuilt .h font file.

   If you want use your own fonts, you first need to convert it from .ttf to .h using online converter:
   https://rop.nl/truetype2gfx/ When you convert it, download .h file and put it inside sketch folder. Include that file
   using #include macro and set font using setFont() function. NOTE: When using custom fonts, you can't use background
   color. Also, start position of text is not in top left corner!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h" //Include Inkplate library to the sketch

#include "DSEG14Classic_Regular20pt7b.h" //Include second font
#include "Not_Just_Groovy20pt7b.h"       //Include first .h font file to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.setFont(&Not_Just_Groovy20pt7b); // Select new font
    display.setTextSize(2);                  // Set font scaling to two (font will be 2 times bigger)
    display.setCursor(0, 60);                // Set print cursor on X = 0, Y = 60
    display.println("Inkplate 6");           // Print some text
    display.setTextSize(1);                  // Set font scaling to one (font is now original size)
    display.print("by e-radionica.com");     // Print text

    display.setFont(&DSEG14Classic_Regular20pt7b); // Select second font
    display.setCursor(0, 250);                     // Set print position on X = 0, Y = 250
    display.println("Some old-school 14 segment"); // Print text
    display.println("display font on e-paper");
    display.print("display");

    display.setFont();                     // Use original 5x7 pixel fonts
    display.setCursor(0, 550);             // Set new print position at X = 0, Y = 550
    display.setTextSize(3);                // Set font scaling to three (font will be 3 times bigger)
    display.print("Classic 5x7 px fonts"); // Print text
    display.display();                     // Display everything on display
}

void loop()
{
    // Nothing...
}