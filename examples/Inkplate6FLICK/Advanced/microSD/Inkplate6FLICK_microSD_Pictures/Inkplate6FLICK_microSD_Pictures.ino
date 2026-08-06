/**
 **************************************************
 * @file        Inkplate6FLICK_microSD_Pictures.ino
 * @brief       microSD image loading demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to load and display image files from a microSD
 *              card on Inkplate 6FLICK. The example shows multiple methods:
 *              drawing a bitmap directly by filename, drawing a bitmap using
 *              an SdFat SdFile object, and drawing a JPEG image. Images are
 *              rendered in 3-bit (grayscale) mode for better visual quality.
 *
 *              Supported formats include BMP, JPEG and PNG (library-dependent
 *              limits apply). BMP files must be uncompressed; typical supported
 *              colour depths are 1/4/8/24-bit. Images must fit the display
 *              resolution - oversized images may not render properly. Some draw
 *              functions support optional invert/dither parameters, and invert
 *              may be required depending on how the bitmap was exported. SD card
 *              power is disabled at the end with sdCardSleep() to reduce
 *              consumption.
 *
 *              Expected output: an "SD Card OK!" message if initialization
 *              succeeds, then image1.bmp, image2.bmp and pyramid.jpg displayed in
 *              sequence, or an error message on screen if an image cannot be
 *              opened or decoded.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable, microSD card
 * - Extra:      Images on the SD card (provided in the example folder)
 * - Libraries:  Inkplate library (includes SD/SdFat support)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Format a microSD card as FAT/FAT32.
 * 3) Copy the provided example images (e.g. image1.bmp, image2.bmp,
 *    pyramid.jpg) to the SD card (typically the root folder).
 * 4) Insert the microSD card into Inkplate 6FLICK.
 * 5) Upload the sketch.
 * 6) The display shows the images sequentially, with short delays between them.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
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

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card OK! Reading image...");
        display.display();

        // If card is properly init, try to load image and display it on e-paper at position X=0, Y=0
        // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter
        // to true will flip all colors on the image, making black white and white black. This may be necessary when
        // exporting bitmaps from certain softwares.
        if (!display.image.draw("image1.bmp", 0, 0, 1))
        {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            display.println("Image open error");
        }
        display.display();
        delay(5000);

        // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
        display.clearDisplay();
        if (file.open("image2.bmp", O_RDONLY))
        {
            display.image.drawBitmapFromSd(&file, 0, 0);
        }
        else
        {
            display.println("Image open error");
        }   
        display.display();
        delay(5000);
    
        // Now draw a JPEG
        display.clearDisplay();
        if (!display.image.draw("pyramid.jpg", 100, 0, true, false))
        {
            // If is something failed (wrong filename or wrong format), write error message on the screen.
            // You can turn off dithering for somewhat faster image load by changing the fifth parameter to false, or
            // removing the parameter completely
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
