/*
   Inkplate6_Image_Frame_From_SD example for Soldered Inkplate6
   For this example you will need a micro USB cable, Inkplate6 and a SD card loaded with images.
   Select "e-radionica Inkplate6" or "Soldered Inkplate6" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   You can open .bmp, .jpeg, or .png files that have a color depth of 1-bit (BW bitmap), 4-bit, 8-bit and
   24 bit, but there are some limitations of the library. Make sure that the image has a resolution smaller
   than 800x600 or otherwise it won't fit on the screen. Format your SD card in standard FAT file format.

   This example will show you how you can make slideshow images from an SD card. Put your images on
   the SD card in a file and specify the file path in the sketch.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   16 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif


/******************CHANGE HERE***********************/

// Set the time between changing 2 images in seconds
#define SECS_BETWEEN_PICTURES 60

// Path to the folder with pictures (e.g: there is a folder called test on the SD card)
char folderPath[] = "/test/"; // NOTE: Must end with /

/****************************************************/

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile folder, file;             // Create SdFile objects used for accessing files on SD card

// Last image index stored in RTC RAM that stores variable even if deep sleep is used
RTC_DATA_ATTR uint16_t lastImageIndex = 0;

void setup()
{
    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setCursor(0, 0);     // Set the cursor on the beginning of the screen
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(5);      // Scale text to be five times bigger then original (5x7 px)

    // If the folder is empty print a message and go to the sleep
    if (!getFileCount())
    {
        display.println("The folder is empty");
        display.display();

        // Go to deep sleep and do nothing
        deepSleep();
    }
}

void loop()
{
    // Open directory with pictures
    if (folder.open(folderPath))
    {
        // Open the last opened file if it's not the beginning of the file
        openLastFile();

        // If it's the beginning of the file, just open the next file
        if (!file.openNext(&folder, O_RDONLY))
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
            skipHiden();

            // Get name of the pucture, create path and draw image on the screen
            if (!displayImage())
            {
                // Reset the loop if there is an error displaying the image
                return;
            }

            // Close the file
            file.close();
        }
        // Close the folder
        folder.close();
    }
    else
    {
        display.printf("Error opening folder! Make sure \nthat you have entered the proper \nname and add / to the end "
                       "of the \npath");
        display.display();
        deepSleep();
    }

    // If the index is equal to 0, it is the end of the file so repeat the code in loop again
    // If not, go into a deep sleep and wait for displaying next image
    if (lastImageIndex != 0)
    {
        // Set EPS32 to be woken up in 10 seconds (in this case)
        esp_sleep_enable_timer_wakeup(SECS_BETWEEN_PICTURES * 1000000LL);

        // Go to the deep sleep
        deepSleep();
    }
}

// Activate the SD card and count the files in the folder. Close the folder and file and return the number of files
int getFileCount()
{
    // Init SD card
    if (!display.sdCardInit())
    {
        // If the SD card init is not successful, display an error on the screen
        display.println("SD Card error!");
        display.display();

        // Go to deep sleep and do nothing
        deepSleep();
    }
    else
    {
        // If the SD card init successfully, count the files
        int fileCount = 0;

        // Open the folder
        if (folder.open(folderPath))
        {
            // Opening the next file until it reaches the end
            while (file.openNext(&folder, O_READ))
            {
                // If the file is not hidden, increase the counter
                if (!file.isHidden())
                {
                    fileCount++;
                }

                // Close the file
                file.close();
            }
            // Close the folder
            folder.close();
        }
        else
        {
            display.println("The folder doesn't exist");
            display.display();
            deepSleep();
        }

        // Return the number of files
        return fileCount;
    }
}

// Go to deep sleep
void deepSleep()
{
    // Turn off the power supply for the SD card
    display.sdCardSleep();

    // Put ESP32 into deep sleep (low power mode)
    esp_deep_sleep_start();
}

// If it's the first file, the file open at index 0 won't work so skip this for the index zero (first file because the
// index is declared as 0)
void openLastFile()
{
    if (lastImageIndex != 0)
    {
        if (file.open(&folder, lastImageIndex, O_READ))
        {
            // Close the file so the code below can open the next one
            file.close();
        }
    }
}

// Get name of the pucture, create path and draw image on the screen
bool displayImage()
{
    // Get the name of the picture and create picture path
    int maxCharacters = 100;
    char pictureName[maxCharacters];
    file.getName(pictureName, maxCharacters);
    char *picturePath = strcat(folderPath, pictureName);

    // Draw the image on the screen
    if (!display.drawImage(picturePath, 0, 0, 1, 0))
    {
        // Return 0 to signalize an error
        return 0;
    }

    // Display the picture on the screen
    display.display();
    return 1;
}

// Skip hidden files and subdirectories
void skipHiden()
{
    while (file.isHidden() || file.isSubDir())
    {
        file.close();

        // Opening the next file while the file is hidden or the file is the directory
        if (!file.openNext(&folder, O_RDONLY))
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
}
