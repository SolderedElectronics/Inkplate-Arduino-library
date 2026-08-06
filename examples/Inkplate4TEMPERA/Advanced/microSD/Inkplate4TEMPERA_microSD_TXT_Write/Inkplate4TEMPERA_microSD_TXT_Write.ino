/**
 **************************************************
 * @file        Inkplate4TEMPERA_microSD_TXT_Write.ino
 * @brief       Write text file to SD card on Soldered Inkplate 4TEMPERA.
 *
 * @details     Demonstrates how to initialize the SD card, create a .txt file,
 *              write data into it, and properly close the file using the
 *              Inkplate SD card interface. The example writes a short text
 *              string into "test.txt" stored on a FAT-formatted SD card.
 *
 *              The SD card must be properly formatted (FAT/FAT32). Always close
 *              files after writing to prevent corruption. The SD card is put
 *              into sleep mode after the operation to reduce power consumption.
 *              Expected output is status messages on the Inkplate display and a
 *              "test.txt" file created on the SD card with the written content.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4TEMPERA
 * - Hardware:   Inkplate 4TEMPERA, USB cable, microSD card
 * - Extra:      None
 * - SD card:    FAT / FAT32 format
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate4TEMPERA"
 *    from Tools -> Board.
 * 2) Insert a FAT-formatted SD card into the Inkplate.
 * 3) Upload the sketch to Inkplate 4TEMPERA.
 * 4) The program initializes the SD card.
 * 5) A file named "test.txt" is created (or appended if it exists).
 * 6) The text string defined in the code is written into the file.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-01-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

char *fileName = "test.txt"; // The name of a file with the extension.

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 4TEMPERA.\n"; // "\n" represents a new line

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setCursor(30,30); // Set position of text
    display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.print("SD Card OK!");
        display.partialUpdate();

        // Try to create text.txt file.
        if (!file.open(fileName, FILE_WRITE))
        { // If it fails to create, send an error message to display, otherwise write to file.
            display.setCursor(30,70); // Set position of text
            display.print("Error while creating the file!");
            display.partialUpdate();
            display.sdCardSleep();
        }
        else
        {
            display.setCursor(30,70); // Set position of text
            display.print("Writing in the file...");
            display.partialUpdate();
            file.write(dataToWrite); // Write data string in the file
            display.setCursor(30,110); // Set position of text
            display.print("Data has been written!");
            display.partialUpdate();
            file.close();          // Close the file
            display.sdCardSleep(); // Put sd card in sleep mode
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
      // (using infinite loop)
        display.setCursor(30,70); // Set position of text
        display.println("SD Card error!");
        display.partialUpdate();
        display.sdCardSleep();
        while (true)
            ;
    }
}

void loop()
{
    // Nothing...
}
