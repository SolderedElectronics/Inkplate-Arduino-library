/**
 **************************************************
 * @file        Inkplate6COLOR_microSD_Pictures.ino
 * @brief       Loads and displays image files from an SD card on Inkplate
 *              6COLOR.
 *
 * @details     This example demonstrates how to initialize the SD card,
 *              open image files stored on it, and render them on the
 *              Inkplate 6COLOR display. It shows two workflows:
 *
 *              1) Loading an image directly by filename using the Inkplate
 *                 image helper API.
 *              2) Opening a file manually with SdFat and passing the file
 *                 handle to the image drawing function.
 *
 *              The sketch is intended for displaying bitmap images stored on
 *              removable media and is useful for slideshows, dashboards,
 *              signage, and offline image viewers. After use, the SD card power
 *              rail is disabled with sdCardSleep() to reduce power consumption.
 *
 *              Supported file types may include BMP, JPEG, and PNG, but format
 *              support depends on library capabilities and available RAM. BMP
 *              support is the most predictable option for embedded use: use
 *              uncompressed Windows BMP files with supported bit depths (the
 *              original example notes 1-bit, 4-bit, 8-bit and 24-bit BMP
 *              support). Large images and compressed formats can require
 *              substantial RAM and may fail on memory-constrained workflows.
 *              Image dimensions must fit within the 600x448 display resolution
 *              of Inkplate 6COLOR or be otherwise handled by the drawing
 *              routine. The optional invert parameter may be useful when bitmap
 *              colours appear reversed due to export settings. Display mode is
 *              Inkplate 6COLOR colour e-paper mode with full refreshes.
 *
 *              Expected output: the first image from image1.bmp, then the second
 *              image from image2.bmp, with status messages on Serial such as SD
 *              card init success, image load success, or image/file open errors.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      microSD card formatted as FAT/FAT32 with compatible image files
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Format a microSD card using a standard FAT/FAT32 filesystem.
 * 3) Copy compatible image files to the card root directory, using the
 *    filenames used in this example (image1.bmp, image2.bmp) or adjusting the
 *    sketch to match your files.
 * 4) Insert the card into Inkplate 6COLOR.
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) The sketch initializes the SD card, loads the first image by filename,
 *    displays it, then opens a second image through SdFat and displays it.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-17
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile file;          // Create SdFile object used for accessing files on SD card

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(3);      // Set font size to 3

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        Serial.println("SD Card OK! Reading image...");

        // If card is properly init, try to load image and display it on e-paper at position X=0, Y=0
        // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter
        // to true will flip all colors on the image, making black white and white black. This may be necessary when
        // exporting bitmaps from certain softwares.
        if (display.image.draw("image1.bmp", 0, 0, 1))
        {
            display.display();
            delay(5000);
        }
        else
        {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the Serial
            // Monitor. REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            Serial.println("Image open error");
        }


        // Now try to load image using SdFat library class (for more advanced users) and display image on epaper.
        display.clearDisplay();
        if (file.open("image2.bmp", O_RDONLY))
        {
            display.image.drawBitmapFromSd(&file, 0, 0);
            display.display();
            delay(5000);
        }
        else
        {
            Serial.println("Image open error");
        }
    }
    else
    {
        // If SD card init not success, display error on screen
        Serial.println("SD Card error!");
    }

    // Turn off the MOSFET that powers the SD card
    display.sdCardSleep();
}

void loop()
{
    // Nothing...
}
