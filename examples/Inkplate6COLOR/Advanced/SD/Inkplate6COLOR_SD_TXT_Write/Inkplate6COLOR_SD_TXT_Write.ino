/*
   Inkplate6COLOR_SD_TXT_Write example for Soldered Inkplate 6COLOR
   For this example you will need only a micro USB cable, Inkplate 6COLOR and a SD card.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to write in .txt files.
   NOTE: Open the Serial monitor at 115200 baud to see what happens.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   23 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

char *fileName = "test.txt"; // The name of a file with the extension.

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 6COLOR.\n"; // "\n" represents a new line

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
