/*
   Inkplate_SD_BMP example for e-radionica Inkplate6
   For this example you will need a micro USB cable, Inkplate6 and a SD card loaded with
   image1.bmp and image2.bmp file that can be found inside folder of this example.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library

   You can open .bmp, .jpeg or .png files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
   24 bit AND have resoluton smaller than 800x600 or otherwise it won't fit on screen.
   Format your SD card in standard FAT fileformat.

   This example will show you how you can read .bmp and .jpeg files (pictures) from SD card and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "SdFat.h"               //Include library for SD card
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    Serial.begin(115200);

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        // If card is properly init, try to load image and display it on e-paper at position X=0, Y=0
        // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter
        // to true will flip all colors on the image, making black white and white black. This may be necessary when
        // exporting bitmaps from certain softwares.
        if (!display.drawImage("image1.bmp", 0, 0, 1))
        {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            display.println("Image open error");
            display.display();
        }
        display.display();
    }
    else
    {
        // If SD card init not success, display error on screen and stop the program (using infinite loop)
        display.println("SD Card error!");
        display.partialUpdate();
        while (true)
            ;
    }
    delay(5000);

    // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
    if (file.open("image2.bmp", O_RDONLY))
    {
        display.drawBitmapFromSd(&file, 0, 0);
        display.display();
    }

    display.clearDisplay();
    delay(3000);

    // Now draw a JPEG
    if (!display.drawImage("pyramid.jpg", 100, 0, true, false))
    {
        // If is something failed (wrong filename or wrong format), write error message on the screen.
        // You can turn off dithering for somewhat faster image load by changing the fifth parameter to false, or
        // removing the parameter completely
        display.println("Image open error");
        display.display();
    }
    display.display();
}

void loop()
{
    // Nothing...
}
