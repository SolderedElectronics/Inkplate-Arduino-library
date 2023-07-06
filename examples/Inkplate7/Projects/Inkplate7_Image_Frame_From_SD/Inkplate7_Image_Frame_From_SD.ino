/*
   Inkplate7_Image_Frame_From_SD example for Soldered Inkplate7
   For this example you will need a USB-C cable, Inkplate7 and a SD card loaded with images.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   You can open .bmp, .jpeg, or .png files that have a color depth of 1-bit (BW bitmap), 4-bit, 8-bit and
   24 bit, but there are some limitations of the library. It will skip images that can't be drawn.
   Make sure that the image has a resolution smaller than 400x300 or otherwise it won't fit on the screen.
   Format your SD card in standard FAT file format.

   This example will show you how you can make slideshow images from an SD card. Put your images on
   the SD card in a file and specify the file path in the sketch. If you don't want to wait defined delay time,
   you can press the wake button and the next image will be loaded on the screen. It will take some time until
   the image will be loaded.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   3 Jul 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

/******************CHANGE HERE***********************/

// Set the time between changing 2 images in seconds
// Note: It will take a couple of seconds more (or longer if there is a file that must be skipped because it can't be
// drawn) than the specified time because Inkplate needs time for loading and display the image
#define SECS_BETWEEN_PICTURES 60

// Path to the folder with pictures (e.g. there is a folder called images on the SD card)
const char folderPath[] = "/images/"; // NOTE: Must end with /

/****************************************************/

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile folder, file;  // Create SdFile objects used for accessing files on SD card

// Last image index stored in RTC RAM that stores variable even if deep sleep is used
RTC_DATA_ATTR uint16_t lastImageIndex = 0;

void setup()
{
    Serial.begin(115200);   // Init serial communication
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
}

void loop()
{
    // If the folder is empty print a message and go to the sleep
    if (!getFileCount())
    {
        Serial.println("The folder is empty");

        // Go to deep sleep and do nothing
        deepSleep();
    }

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
            skipHidden();

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
        Serial.printf("Error opening folder! Make sure that you have entered the proper name and add / to the end "
                      "of the path");
        deepSleep();
    }

    // If the index is equal to 0, it is the end of the file so repeat the code in loop again
    // If not, go into a deep sleep and wait for displaying next image
    if (lastImageIndex != 0)
    {
        // Set EPS32 to be woken up in 60 seconds (in this case)
        esp_sleep_enable_timer_wakeup(SECS_BETWEEN_PICTURES * 1000000LL);

        // Go to the deep sleep
        deepSleep();
    }
}

/**
 * @brief     Activate the SD card and count the files in the folder. Close the folder and file and return the number of
 *            files
 */
int getFileCount()
{
    // Init SD card
    if (!display.sdCardInit())
    {
        // If the SD card init is not successful, display an error on the screen
        Serial.println("SD Card error!");

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
            Serial.println("The folder doesn't exist");
            deepSleep();
        }

        // Return the number of files
        return fileCount;
    }
}

/**
 * @brief     Turn off all peripherals, go to deep sleep, and enable wakeup on the wake button.
 */
void deepSleep()
{
    // Turn off the power supply for the SD card
    display.sdCardSleep();

    // Enable wakeup from deep sleep on gpio 36 (wake button)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW);

    // Put ESP32 into deep sleep (low power mode)
    esp_deep_sleep_start();
}

/**
 * @brief     If it's the first file, the file open at index 0 won't work so skip this for the index zero (first file
 *            because the index is declared as 0).
 */
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

/**
 * @brief     Get name of the pucture, create path and draw image on the screen.
 *
 * @return    0 if there is an error, 1 if the image is drawn.
 */
bool displayImage()
{
    // Get the name of the picture
    int maxCharacters = 100;
    char pictureName[maxCharacters];
    file.getName(pictureName, maxCharacters);

    // Cpoy of the folder path just for creating path for the image
    char path[maxCharacters];
    strcpy(path, folderPath);

    // Create picture path (folder path + picture name)
    char *picturePath = strcat(path, pictureName);

    // Draw the image on the screen
    if (!display.drawImage(picturePath, 0, 0, 1, 0))
    {
        // Close folder and file
        file.close();
        folder.close();

        // Return 0 to signalize an error
        return 0;
    }

    // Display the picture on the screen
    display.display();
    return 1;
}

/**
 * @brief     Skip hidden files and subdirectories.
 */
void skipHidden()
{
    while (file.isHidden() || file.isSubDir())
    {
        file.close();

        // Opening the next file while the file is hidden or the file is the directory
        if (!file.openNext(&folder, O_RDONLY))
        {
            // It is the end of file
            lastImageIndex = 0;
        }
        else
        {
            // Save the index of the last opened file
            lastImageIndex = file.dirIndex();
        }
    }
}
