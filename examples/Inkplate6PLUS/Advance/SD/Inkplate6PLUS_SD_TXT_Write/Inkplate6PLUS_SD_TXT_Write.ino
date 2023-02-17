/*
   Inkplate6PLUS_SD_TXT_Write example for Soldered Inkplate 6Plus
   For this example you will need only a micro USB cable, Inkplate 6Plus and a SD card.
   Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to write in .txt file.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   23 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

char *fileName = "test.txt"; // The name of a file with the extension.

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 6Plus.\n"; // "\n" represents a new line

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5);

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok!");
        display.partialUpdate();

        // Try to create text.txt file.
        if (!file.open(fileName, FILE_WRITE))
        { // If it fails to create, send an error message to display, otherwise write to file.
            display.println("Error while creating the file!");
            display.partialUpdate();
            display.sdCardSleep();
        }
        else
        {
            display.println("Writing in the file...");
            display.partialUpdate();
            file.write(dataToWrite); // Write data string in the file
            display.println("Data has been written successfully!");
            display.partialUpdate();
            file.close();          // Close the file
            display.sdCardSleep(); // Put sd card in sleep mode
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
      // (using infinite loop)
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
