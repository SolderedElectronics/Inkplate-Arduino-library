/*
   Inkplate6COLOR_SD_Pictures example for Soldered Inkplate 6COLOR
   For this example you will need a micro USB cable, an Inkplate 6COLOR and a SD card loaded with
   image1.bmp and image2.bmp file that can be found inside folder of this example.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library

   You can open .bmp, .jpeg or .png files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
   24 bit AND have resoluton smaller than 800x600 or otherwise it won't fit on screen.
   Format your SD card in standard FAT fileformat.

   This example will show you how you can read .bmp and .jpeg files (pictures) from SD card and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "SdFat.h"    //Include library for SD card
Inkplate display;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;          // Create SdFile object used for accessing files on SD card

void setup()
{
    Serial.begin(115200);

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        Serial.println("SD Card OK! Reading image...");

        // If card is properly init, try to load image and display it on e-paper at position X=0, Y=0
        // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter
        // to true will flip all colors on the image, making black white and white black. This may be necessary when
        // exporting bitmaps from certain softwares.
        if (!display.drawImage("image1.bmp", 0, 0, 0))
        {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            Serial.println("Image open error");
        }
        display.display();
    }
    else
    {
        // If SD card init not success, display error on screen, put sd card in sleep mode and stop the program (using infinite loop)
        Serial.println("SD Card error!");
        display.sdCardSleep();
        while (true)
            ;
    }
    delay(5000);

    // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
    if (display.drawImage("image2.bmp", 0, 0, 0))
    {
        display.display();
    }
    else
    {
        Serial.println("Image error.");
    }
    
    display.sdCardSleep(); // Put sd card in sleep mode
}

void loop()
{
    // Nothing...
}
