/**
 **************************************************
 * @file        Inkplate13SPECTRA_SD_Pictures.ino
 * @brief       Display images from an SD card on Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to read .bmp and .jpeg files (pictures) from an SD card
 *              and display them on the e-paper display.
 *
 *              You can open .bmp, .jpeg or .png files (but there are some
 *              limitations in the library) that have a color depth of 1 bit (BW
 *              bitmap), 4 bit, 8 bit or 24 bit AND have a resolution smaller than
 *              600x448, otherwise the image won't fit on the screen. Format your
 *              SD card in the standard FAT file format.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable, SD card loaded with the
 *               images found inside the folder of this example
 * - SD card:    standard FAT format
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    (or "e-radionica Inkplate 13SPECTRA") from Tools -> Board.
 * 2) Copy the example images to a FAT-formatted SD card and insert it into the
 *    Inkplate.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The images are read from the SD card and shown on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-01-22
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
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
        if (display.image.draw("picture1.jpg", 0, 0, 1))
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
        if (file.open("picture2.jpg", O_RDONLY))
        {
            display.image.drawJpegFromSd(&file, 0, 0);
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
