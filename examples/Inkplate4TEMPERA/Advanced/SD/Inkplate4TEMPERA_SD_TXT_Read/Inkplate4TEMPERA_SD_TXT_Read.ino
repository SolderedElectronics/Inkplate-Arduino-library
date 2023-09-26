/*
   Inkplate4TEMPERA_SD_TXT_Read example for Soldered Inkplate 4TEMPERA
   For this example you will need only a USB-C cable, Inkplate 4TEMPERA and a SD card
   loaded with text.txt file that can be found inside folder of this example.
   Select "Soldered Inkplate 4TEMPERA" from Tools -> Board menu.
   Don't have "Inkplate 4TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library

   You can open your own .txt file, but in order to this example works properly it should
   not have more than 200 chars and you should name it text.txt

   This example will show you how to open .txt files and display the content of that file on Inkplate epaper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   18 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4TEMPERA in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "SdFat.h"               //Include library for SD card
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setFrontlight(15);
    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.setTextSize(3);    // Set text to be 3 times bigger than classic 5x7 px text
        display.setCursor(30, 30); // Set position of text
        display.println("SD Card OK! Reading data...");
        display.partialUpdate();
        delay(3000); // Wait a bit so the user reads the message

        // Try to load text with max lenght of 200 chars.
        if (!file.open("/text.txt", O_RDONLY))
        { // If it fails to open, send error message to display, otherwise read the file.
            display.setCursor(30, 70);
            display.println("File open error!");
            display.display();
        }
        else
        {
            display.clearDisplay();    // Clear everything that is stored in frame buffer of epaper
            display.setCursor(0, 0);   // Set print position at the begining of the screen
            display.setTextSize(1);    // Reset text size
            char text[3001];           // Array where data from SD card is stored (max 200 chars here)
            int len = file.fileSize(); // Read how big is file that we are opening
            if (len > 3000)
                len = 3000;       // If it's more than 200 bytes (200 chars), limit to max 200 bytes
            file.read(text, len); // Read data from file and save it in text array
            text[len] = 0;        // Put null terminating char at the and of data
            display.setTextSize(2);
            display.print(text);   // Print data/text
            display.sdCardSleep(); // Put sd card in sleep mode
            display.display();     // Do a full refresh of display
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
        // (using infinite loop)
        display.setCursor(30, 30);
        display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text
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
