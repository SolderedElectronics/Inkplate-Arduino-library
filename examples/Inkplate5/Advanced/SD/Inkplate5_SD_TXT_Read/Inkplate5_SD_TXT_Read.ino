/*
   Inkplate5_SD_TXT_Read example for Soldered Inkplate 5
   For this example you will need only a USB-C cable, Inkplate 5, and an SD card
   loaded with text.txt file that can be found inside folder of this example.
   Select "Soldered Inkplate5" from Tools -> Board menu.
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   You can open your own .txt file, but in order to this example works properly it should
   not have more than 500 chars and you should name it text.txt

   This example will show you how to open .txt files and display the content of that file on Inkplate epaper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   21 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 3 times bigger than original (5x7 px)

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok! Reading data...");
        display.partialUpdate();

        // Try to load text with max lenght of 200 chars.
        if (!file.open("/text.txt", O_RDONLY))
        { // If it fails to open, send error message to display and put sd card in sleep mode, otherwise read the file.
            display.println("File open error");            
            display.display();
            display.sdCardSleep();
        }
        else
        {
            display.clearDisplay();    // Clear everything that is stored in frame buffer of epaper
            display.setCursor(0, 0);   // Set print position at the begining of the screen
            char text[501];            // Array where data from SD card is stored (max 500 chars here)
            int len = file.fileSize(); // Read how big is file that we are opening
            if (len > 500)
                len = 500;         // If it's more than 500 bytes (500 chars), limit to max 500 bytes
            file.read(text, len);  // Read data from file and save it in text array
            text[len] = 0;         // Put null terminating char at the and of data
            display.print(text);   // Print data/text
            display.sdCardSleep(); // Put sd card in sleep mode
            display.display();     // Do a full refresh of display
        }
    }
    else
    { // If card init was not successful, display error on screen and stop the program (using infinite loop)
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
