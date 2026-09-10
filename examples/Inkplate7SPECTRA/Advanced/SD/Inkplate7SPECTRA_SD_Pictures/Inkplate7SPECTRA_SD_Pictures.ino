/*
   Inkplate7SPECTRA_SD_Pictures example for Soldered Inkplate 7SPECTRA
   For this example you will need a micro USB cable, Inkplate 7SPECTRA and a SD card loaded with
   images that can be found inside folder of this example.
   Select "e-radionica Inkplate 7SPECTRA" or "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 7SPECTRA" or "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
   https://docs.soldered.com/inkplate/7spectra/quick-start-guide/

   You can open .bmp, .jpeg or .png files (but there are some limitations by the library) that have
   color depth of 1 bit (BW bitmap), 4 bit, 8 bit and 24 bit AND have resoluton smaller than 600x448
   or otherwise it won't fit on screen. Format your SD card in standard FAT fileformat.

   This example will show you how you can read .bmp and .jpeg files (pictures) from SD card and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   Looking to get support? Write on our forums: https://forum.soldered.com/
   22 January 2026 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
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
    display.setTextSize(2);      // Set font size to 2

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
