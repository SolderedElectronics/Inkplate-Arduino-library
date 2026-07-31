/**
 **************************************************
 * @file        Inkplate13SPECTRA_SD_TXT_Write.ino
 * @brief       Write a text file to an SD card on Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to write into .txt files on the SD card.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable, SD card
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Insert an SD card into the Inkplate.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) Open the Serial Monitor at 115200 baud to see what happens.
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
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

char *fileName = "test.txt"; // The name of a file with the extension.

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 13SPECTRA.\n"; // "\n" represents a new line

void setup()
{
    Serial.begin(115200); // Init serial communication
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        Serial.println("SD Card ok!");

        // Try to create text.txt file.
        if (!file.open(fileName, FILE_WRITE))
        { // If it fails to create, send an error message to display, otherwise write to file.
            Serial.println("Error while creating the file!");
            display.sdCardSleep();
        }
        else
        {
            Serial.println("Writing in the file...");
            file.write(dataToWrite); // Write data string in the file
            Serial.println("Data has been written successfully!");
            file.close();          // Close the file
            display.sdCardSleep(); // Put sd card in sleep mode
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
      // (using infinite loop)
        Serial.println("SD Card error!");
        display.sdCardSleep();
        while (true)
            ;
    }
}

void loop()
{
    // Nothing...
}
