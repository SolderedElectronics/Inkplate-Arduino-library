// This example shows how to use SD card functionality of this Inkplate library and Inkplate board itself.
// For this example to run properly, you will need:
// - SD card (about 1GB, can be bigger), at least class 10
// - Two 24 bit bitmap images (if you don't have any, open any image using MS Paint and save it as 24 bit bitmap) that
// has to be in root directory of your SD Card named image1.bmp and image2.bmp
// - Some .txt file with not more than 200 characters, also in root directory of your SD card, named text.txt

#include "SdFat.h"               //Include modified SdFat library
#include <Inkplate.h>            //Include Inkplate Library
Inkplate display(INKPLATE_1BIT); // Constructor on Inkplate object
SdFile file;                     // Constructor on SdFile object used for accessing files on SD card
void setup()
{
    // Init inkplate library
    display.begin();

    // Clear everything prev displayed on epaper
    display.clearDisplay();
    display.display();

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok! Reading image...");
        display.partialUpdate();

        // If card is properly init., try to load image and display it on epaper
        if (!display.drawBitmapFromSD("image1.bmp", 0, 0))
        {
            // If is something failed (wrong filename or wrong bitmap format [REMEMBER! You can only use Windows Bitmap
            // file with color depth of 1 or 24 bits with no compression!] write error on epaper.
            display.println("Image open error");
            display.display();
        }
        display.display();
    }
    else
    {
        // If not, display error on screen and stop the program (using infinitive loop)
        display.println("SD Card error!");
        display.partialUpdate();
        while (true)
            ;
    }
    delay(5000);

    // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
    if (file.open("image2.bmp", O_RDONLY))
    {
        display.drawBitmapFromSD(&file, 0, 0);
        display.display();
    }

    delay(5000);

    // And the last thing, try to load some text from SD card. Because, we want to write vrey tinny text, it's better to
    // use 1 bit mode, so we switch to it.
    display.selectDisplayMode(INKPLATE_1BIT);

    // Try to load text with max lenght of 200 chars.
    if (!file.open("/text.txt", O_RDONLY))
    {
        display.println("File open error");
        display.display();
    }
    else
    {
        char text[201];
        file.read(text, 200);
        text[file.fileSize()] = 0;
        display.print(text);
        display.display();
    }
}

void loop()
{
}
