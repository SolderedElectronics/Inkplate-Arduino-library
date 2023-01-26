/*
   Inkplate_Image_frame_From_SD example for Soldered Inkplate6
   For this example you will need a micro USB cable, Inkplate6 and a SD card loaded with images.
   Select "e-radionica Inkplate6" or "Soldered Inkplate6" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library

   You can open .bmp, .jpeg or .png files that have color depth of 1 bit (BW bitmap), 4 bit, 8 bit and
   24 bit AND have resoluton smaller than 800x600 or otherwise it won't fit on screen.
   Format your SD card in standard FAT fileformat.

   This example will show you how you can make slideshow images from an SD card. Put your images on
   the SD card in the root directory (just open the SD card on "my computer" and paste the images).

   NOTE: Make sure that on SD are only images, not other files!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   26 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

// Set the time between changing 2 images in seconds
#define SECS_BETWEEN_PICTURES 2

#include "Inkplate.h"            // Include Inkplate library to the sketch
#include "SdFat.h"               // Include library for SD card
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile root;                     // Create SdFile object used for opening root directory
SdFile file;                     // Create SdFile object used for accessing files on SD card

// Last image index stored in RTC RAM that stores variable even if deep sleep is used
RTC_DATA_ATTR uint16_t lastImageIndex = 0;

void setup()
{
    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setCursor(0, 0);     // Set the cursor on the beginning of the screen
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(5);      // Scale text to be five times bigger then original (5x7 px)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        // Open root directory
        if (!root.open("/"))
        {
            display.println("root.open failed!");
        }
        else
        {
            // Open last opened file
            if (lastImageIndex != 0) // If it's the first file, the file open at index 0 won't work so skip this for the
                                     // index zero (first file because the index is declared as 0)
            {
                if (!file.open(&root, lastImageIndex, O_READ))
                {
                    display.println("Error opening last opened file");
                }
                else
                {
                    // Close the file so the code below can open the next one
                    file.close();
                }
            }

            // Go to the next file
            if (!file.openNext(&root, O_RDONLY))
            {
                // If it can't open the next file, there is an end of the file so set the index of the last file to 0
                // because it's used later for restart
                lastImageIndex = 0;
            }
            else
            {
                // Save the index of the last opened file
                lastImageIndex = file.dirIndex();

                // Skip hidden files and subdirectories
                while (file.isHidden() || file.isSubDir())
                {
                    file.close();

                    // Opening the next file while the file is hidden or the file is the directory
                    if (!file.openNext(&root, O_RDONLY))
                    {
                        // It is end of file
                        lastImageIndex = 0;
                    }
                    else
                    {
                        // Save the index of the last opened file
                        lastImageIndex = file.dirIndex();
                    }
                }

                // Get the name of the picture
                int maxCharacters = 50;
                char pictureName[maxCharacters];
                file.getName(pictureName, maxCharacters);

                // Draw the imahe on the screen
                if (!display.drawImage(pictureName, 0, 0, 0, 0))
                {
                    display.print("Error printing image ");
                    display.println(pictureName);
                }

                // Display the picture on the screen
                display.display();

                // Close the file
                file.close();
            }
        }
    }
    else
    {
        // If the SD card init is not successful, display an error on the screen and try again when being the time for
        // the next image
        display.println("SD Card error!");
        display.display();
    }

    // If the index is equal to 0, it is the end of the file so reset the ESP to go to the beginning of the code for
    // printing the first image
    if (lastImageIndex == 0)
    {
        ESP.restart();
    }

    // Turn off the power supply for the SD card
    display.sdCardSleep();

    // Set EPS32 to be woken up in 10 seconds (in this case)
    esp_sleep_enable_timer_wakeup(SECS_BETWEEN_PICTURES * 1000000LL);

    // Put ESP32 into deep sleep (low power mode)
    esp_deep_sleep_start();
}

void loop()
{
    // Nothing...
}
