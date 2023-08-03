/*
   Inkplate4_SD_TXT_Write example for Soldered Inkplate 4
   For this example you will need only a USB-C cable, Inkplate 4, and an SD card.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how you can use Inkplate 4 to write in a .txt file.
   The file name is specified below and you can change it. Just make sure that
   you specified the .txt extension or otherwise it will create a file that you
   can't open. You can also change the content that will be written in the file
   by changing the array called dataToWrite.

   If you don't change anything, after uploading the example the Inkplate will
   create a file named test.txt and write "Hello! This is the file writing example
   for Inkplate 4." into the file. It will also add a new line at the end, so if
   you run the example again, it will write once again the same string in the next line.
   You can see on the Serial Monitor at 115200 baud rate what's happening.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   21 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

// The name of a file with the extension
char *fileName = "test.txt";

// The text you want to write in the file
char *dataToWrite = "Hello! This is the file writing example for Inkplate 4.\n"; // "\n" represents a new line

void setup()
{
    Serial.begin(115200);   // Init serial communication
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Set text to be 2 times bigger than original (5x7 px)

    // Init SD card. Display if SD card is init propery or not
    if (display.sdCardInit())
    {
        // Print a message on the display
        display.println("SD Card ok!");
        Serial.println("SD Card ok!");

        // Try to create text.txt file.
        if (!file.open(fileName, FILE_WRITE))
        {
            // If it fails to create, send an error message to display, otherwise write to file
            Serial.println("Error while creating the file!");
            display.println("Error while creating the file!");
            display.sdCardSleep();
        }
        else
        {
            Serial.println("Writing in the file...");
            display.println("Writing in the file...");
            file.write(dataToWrite); // Write data string in the file
            Serial.println("Data has been written successfully!");
            display.println("Data has been written\nsuccessfully!");
            file.close();          // Close the file
            display.sdCardSleep(); // Put sd card in sleep mode
        }
    }
    else
    {
        // If card init was not successful, display an error on the screen, and put the
        // SD card in sleep mode (turn off the MOSFET that turns off the power for SD).
        // It is desirable to stop the program after the SD card fails, but there is no
        // next part in this example, only an empty loop().You can do it, for example,
        // by calling while(1); at the end of this code block, after sdCardSleep();
        Serial.println("SD Card error!");
        display.println("SD Card error!");
        display.sdCardSleep();
    }

    // Also, show everything on the screen
    display.display();
}

void loop()
{
    // Nothing...
}
