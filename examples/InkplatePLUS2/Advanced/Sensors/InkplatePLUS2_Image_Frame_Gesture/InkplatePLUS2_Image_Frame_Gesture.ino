/*
   InkplatePLUS2_Image_Frame_Gesture example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2 and an SD card with images.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read from the built-in APDS9960 gesture sensor
   and change pictures from the SD card accordingly. Swipe up to go forwards and swipe
   down to go backwards!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   13 Aug 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

/******************CHANGE HERE***********************/

// Path to the folder with pictures (e.g. there is a folder called images on the SD card)
const char folderPath[] = "/images/"; // NOTE: Must end with /

/****************************************************/

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile folder, file;             // Create SdFile objects used for accessing files on SD card

// Last image index stored in RTC RAM that stores variable even if deep sleep is used
RTC_DATA_ATTR int imageIndex = 0;

// Remember the total number of files in the directory
RTC_DATA_ATTR uint16_t numOfFiles = 0;

// bitmask for GPIO_34 which is connected to I/O expander 1 INT pin
#define APDS_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

void setup()
{
    Serial.begin(115200);

    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setCursor(30, 30);   // Set the cursor a bit further from the corner so the text is visible
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(4);      // Scale text to be four times bigger then original (5x7 px)

    Serial.println("Woken up again!");

    // Init the gesture sensor
    display.apds9960.begin();

    // Get the total number of files in the folder
    getNumFiles();

    // Display the first image
    displayImageAtIndex(imageIndex);
}

void loop()
{
    // Check if a gesture was detected on the sensor
    if (display.apds9960.gestureAvailable())
    {
        // A gesture was detected, read what it is
        int gesture = display.apds9960.readGesture();

        // Depending on the gesture, affect the imageIndex
        switch (gesture)
        {
        case GESTURE_UP:
            // Go to the next image
            imageIndex++;
            // If we went over the last file, reset to the beginning
            if (imageIndex >= numOfFiles)
                imageIndex = 0;
            // Show the image
            displayImageAtIndex(imageIndex);
            break;

        case GESTURE_DOWN:
            // Go to the previous image
            imageIndex--;
            // If we go into the negatives, go to the last image
            if (imageIndex < 0)
                imageIndex = numOfFiles - 1;
            // Show the image
            displayImageAtIndex(imageIndex);
            break;

        default:
            // Ignore
            break;
        }
    }

    // Done the correct action? Go to sleep to save power!
    deepSleep();
}

/**
 * @brief       Draw the image in the folder with the given index
 *
 * @param       uint16_t index, the index of the file in the folder where the images are which we want to draw
 */
void displayImageAtIndex(uint16_t index)
{
    // Clear what was previously in the buffer
    display.clearDisplay();

    // Read the folder from the beginning again
    folder.rewind();
    folder.open(folderPath, O_READ);

    int currentlyObservedIndex = 0;

    // Read files one after other
    while (file.openNext(&folder, O_RDONLY))
    {
        // If we have reached the file of interest
        if (currentlyObservedIndex == index)
        {
            // Get the filename
            char filename[100];
            file.getName(filename, sizeof(filename));

            // Copy of the folder path just for creating path for the image
            char fPath[100];
            strcpy(fPath, folderPath);

            // Combine the filename with the folder name to get the full path
            char *fullPathToImage = strcat(fPath, filename);
            Serial.println(fullPathToImage);

            // Draw the image on the screen
            if (!display.drawImage(fullPathToImage, 0, 0, 1, 0))
            {
                // Can't draw the image? Notify the user and go to sleep
                display.setCursor(30, 30);
                display.println("Image drawing failed!");
                display.display();

                // Also, close the file and folder
                file.close();
                folder.close();

                deepSleep();
            }

            // Show the image from the image buffer
            // TODO partial update
            display.display();

            // Close file and folder
            file.close();
            folder.close();

            // Break out of the loop
            break;
        }
        file.close();

        // If this isn't the file we 're looking for, go to the next one
        currentlyObservedIndex++;
    }
}

/**
 * @brief     Go to deep sleep and enable wakeup on the wake button.
 */
void deepSleep()
{
    // Turn off the power supply for the SD card
    display.sdCardSleep();

    display.apds9960.setInterruptPin(GPIO_NUM_34);

    display.setIntPin(10, IO_INT_ADDR);

    // Enable wakeup from deep sleep on GPIO 36 (APDS interrupt)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, LOW);

    Serial.println("Going tf to sleep!!");

    // Put ESP32 into deep sleep (low power mode)
    esp_deep_sleep_start();
}

/**
 * @brief     Get the number of image files in the set folder of the SD card
 */
void getNumFiles()
{
    // Check if we can init the SD card
    if (!display.sdCardInit())
    {
        // Error? Inform the user and go to sleep
        display.println("SD card initialization failed!");
        display.display();
        deepSleep();
    }

    // Check if we have the correct directory
    if (!folder.open(folderPath, O_READ))
    {
        // Error? Inform the user and go to sleep
        display.println("Failed to open directory!");
        display.display();
        deepSleep();
    }

    // Count total files in the directory
    while (file.openNext(&folder, O_READ))
    {
        numOfFiles++;
        file.close();
    }

    // Close directory as we are done reading
    folder.close();
}
