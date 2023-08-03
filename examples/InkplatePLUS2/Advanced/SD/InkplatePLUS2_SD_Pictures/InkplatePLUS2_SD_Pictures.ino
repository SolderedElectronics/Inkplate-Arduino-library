/*
   InkplatePLUS2_SD_Pictures example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, InkplatePLUS2 and a SD card 
   loaded with images that can be found inside folder of this example.
   Select "Soldered InkplatePLUS2" from Tools -> Board menu.
   Don't have "Soldered InkplatePLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read .bmp and .jpeg files (pictures) from an SD card and
   display that image on the e-paper display. You can open .bmp, .jpeg or .png files (but there are 
   some limitations by the library) that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and 
   24 bit AND have resoluton smaller than 600x600 or otherwise it won't fit on screen. Format your
   SD card in standard FAT fileformat.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   24 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(3);      // Set font size to 3
    display.setCursor(30, 30); // Set the text a bit further from the corner so it's visible

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card OK! Reading image...");
        display.display();

        // If card is properly init, try to load image and display it on e-paper at position X=0, Y=0
        // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter
        // to true will flip all colors on the image, making black white and white black. This may be necessary when
        // exporting bitmaps from certain softwares.
        if (!display.drawImage("image1.bmp", 0, 0, 1))
        {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the parameter completely
            display.setCursor(30, 100); // Set the text a bit further
            display.println("Image open error");
        }
        display.display();
        delay(5000);

        // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
        display.clearDisplay();
        if (file.open("image2.bmp", O_RDONLY))
        {
            display.drawBitmapFromSd(&file, 0, 0, 1);
        }
        else
        {
            display.println("Image open error");
        }
        display.display();
        delay(5000);

        // Now draw a JPEG
        display.clearDisplay();
        if (!display.drawImage("pyramid.jpg", 0, 0, true))
        {
            // If is something failed (wrong filename or wrong format), write error message on the screen.
            // You can turn off dithering for somewhat faster image load by changing the fourth parameter to false, or
            // removing the parameter completely
            display.setCursor(30, 100); // Set the text a bit further
            display.println("Image open error");
        }
        display.display();
    }
    else
    {
        // If SD card init not success, display error on screen
        display.println("SD Card error!");
        display.display();
    }

    // Turn off the MOSFET that powers the SD card
    display.sdCardSleep();
}

void loop()
{
    // Nothing...
}
