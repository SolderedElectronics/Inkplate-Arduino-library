/*
   Inkplate4TEMPERA_Image_Frame_Gesture example for Soldered Inkplate 4 TEMPERA
   For this example you will need a USB-C cable, Inkplate 4TEMPERA and a SD card loaded with images.
   Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   You can open .bmp, .jpeg, or .png files that have a color depth of 1-bit (BW bitmap), 4-bit, 8-bit and
   24 bit, but there are some limitations of the library. It will skip images that can't be drawn.
   Make sure that the image has a resolution smaller than 600x600 or otherwise it won't fit on the screen.
   Format your SD card in standard FAT file format.

   NOTE: the maximum number of images on the SD card is 512 due to the limitations of the RTC memory.

   This example will show you how you can make slideshow images from an SD card. Put your images on
   the SD card in a file and specify the file path in the sketch. Images will cycle on gestures
   LEFT and RIGHT on the APDS9960 gesture sensor, and the rest of the time, Inkplate will be in deep sleep,
   making this example battery-friendly.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   2 Oct 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

/******************CHANGE HERE***********************/

// Path to the folder with pictures (e.g. there is a folder called images on the SD card)
const char folderPath[] = "/images/"; // NOTE: Must end with /

/****************************************************/

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile folder, file;             // Create SdFile objects used for accessing files on SD card

// These variables are stored in RTC's memory, so they stay alive even if Inkplate goes to deep sleep

// Remember if we completed the first boot process
// The reason for this is that the APDS needs to only be configured once, as it stays on during sleep
RTC_DATA_ATTR bool firstBootcompleted = false;

// A flag to see if a gesture woke up Inkplate which wasn't left or right
RTC_DATA_ATTR bool skip = false;

// Last image index stored in RTC RAM that stores variable even if deep sleep is used
RTC_DATA_ATTR uint16_t lastImageIndex = 0;
// Dir indexes of all the images which are in the given folder
RTC_DATA_ATTR uint16_t imageIndexes[512];
// How many images are there in total
RTC_DATA_ATTR int numFiles = 0;
// Which file in imageIndexes are we currently observing
RTC_DATA_ATTR int currentImageIndex = 0;

void setup()
{
    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setCursor(30, 30);   // Set the cursor a bit further from the corner so the text is visible
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(5);      // Scale text to be five times bigger then original (5x7 px)

    // Init SD card
    if (!display.sdCardInit())
    {
        // If the SD card init is not successful, display an error on the screen
        display.println("SD Card error!");
        display.display();

        // Go to deep sleep and do nothing
        deepSleep();
    }

    if (!firstBootcompleted)
    {
        // If this is the first boot...

        // Init APDS and enable the gesture sensor
        display.apds9960.init();
        display.apds9960.enableGestureSensor(true); // Setting 'true' here enables interrupts

        // Make sure gesture is at lowest sensitivity so it doesn't accidentially get triggered
        display.apds9960.setGestureGain(0);

        // Get the file count and file dir indexes and save them to RTC memory
        getFileCount();

        // Remember that this config was completed
        firstBootcompleted = true;
    }
    else
    {
        // This happens from the second boot onwards
        // This must mean Inkplate was woken up by an interrupt from the APDS
        // Let's read which gesture woke it up
        if (display.apds9960.isGestureAvailable())
        {
            // Read the gesture and act accordingly
            switch (display.apds9960.readGesture())
            {
            case DIR_LEFT:
                skip = false;        // Picture should be changed
                currentImageIndex++; // Go to next picture
                // Reset counter if it overflowed
                if (currentImageIndex > numFiles - 1)
                    currentImageIndex = 0;
                break;
            case DIR_RIGHT:
                skip = false;        // Picture should be changed
                currentImageIndex--; // Go to previous picture
                // Loop back counter if it went below 0
                if (currentImageIndex < 0)
                    currentImageIndex = numFiles - 1;
                break;
            default:
                // If it's not a left or right gesture, do nothing
                skip = true;
            }
        }
    }
}

void loop()
{
    // If the gesture was either left or right
    if (!skip)
    {
        // Open the folder with pictures
        if (folder.open(folderPath))
        {
            // Open the according file
            if (!file.open(&folder, imageIndexes[currentImageIndex], O_READ))
            {
                // If it can't open the next file, there is an end of the file so set the index of the last file to 0
                // because it's used later for restart
                lastImageIndex = 0;
            }
            else
            {
                // The file was opened successfully!
                // Save the index of the last opened file
                lastImageIndex = file.dirIndex();

                // Skip hidden files and subdirectories
                skipHidden();

                // Get name of the picture, create path and draw image on the screen
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

            // That's it, go to sleep!
            deepSleep();
        }
        else
        {
            display.printf(
                "Error opening folder! Make sure \nthat you have entered the proper \nname and add / to the end "
                "of the \npath");
            display.display();
            deepSleep();
        }

        // If the index is equal to 0, it is the end of the file so repeat the code in loop again
        // If not, go into a deep sleep and wait for displaying next image
        if (lastImageIndex != 0)
        {
            // Go to deep sleep
            deepSleep();
        }
    }
    else
    {
        // If a gesture was made which isn't left or right, just go to sleep
        deepSleep();
    }
}

/**
 * @brief     Activate the SD card and count the files in the folder. Close the folder and file and return the number of
 *            files
 */
void getFileCount()
{
    // Open the folder
    if (folder.open(folderPath))
    {
        // Opening the next file until it reaches the end
        while (file.openNext(&folder, O_READ))
        {
            // If the file is not hidden, increase the counter
            if (!file.isHidden())
            {
                imageIndexes[numFiles] = file.dirIndex();
                Serial.println("just added to image indexes: ");
                Serial.println(imageIndexes[numFiles]);
                numFiles++;
            }

            // Close the file
            file.close();
        }
        // Close the folder
        folder.close();

        Serial.print("just counted the files, there are : ");
        Serial.println(numFiles);
    }
    else
    {
        display.println("The folder doesn't exist");
        display.display();
        deepSleep();
    }
}

/**
 * @brief     Go to deep sleep and enable wakeup on the wake button.
 */
void deepSleep()
{
    // Turn off the power supply for the SD card
    display.sdCardSleep();

    // First, configure the interrupt from APDS to the GPIO expander
    display.pinModeIO(9, INPUT_PULLUP, IO_INT_ADDR);
    display.setIntPin(9, IO_INT_ADDR);

    // Now, the internal GPIO expander will fire an interrupt on it's INT pin
    // On pin change of pin 9

    // Now, configure the interrupt from the GPIO expander to ESP32
    // Set it as an input
    pinMode(GPIO_NUM_34, INPUT);
    // Enable wakeup from deep sleep on GPIO 34 when it goes LOW
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, LOW);

    // Just in case, wait until the APDS interrupt clears
    while (!display.digitalReadIO(9, IO_INT_ADDR))
    {
        delay(10);
    }

    // Finally, put ESP32 into deep sleep (low power mode)
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

    // Copy of the folder path just for creating path for the image
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
