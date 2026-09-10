/*
   Inkplate7SPECTRA_SD_TXT_Read example for Soldered Inkplate 7SPECTRA
   For this example you will need only a micro USB cable, Inkplate 7SPECTRA and a SD card
   loaded with text.txt file that can be found inside folder of this example.
   Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
   https://docs.soldered.com/inkplate/7spectra/quick-start-guide/
   
   You can open your own .txt file, but in order to this example works properly it should
   not have more than 200 chars and you should name it text.txt

   This example will show you how to open .txt files and display the content of that file on Inkplate epaper display.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   Looking to get support? Write on our forums: https://forum.soldered.com/
   22 January 2026 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setCursor(0, 0);
    display.setTextColor(INKPLATE_BLACK);
    display.setTextSize(1);
    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        // Try to load text with max lenght of 200 chars.
        if (!file.open("/text.txt", O_RDONLY))
        { // If it fails to open, send error message to display, otherwise read the file.
            display.println("File open error");
            display.display();
        }
        else
        {
            display.clearDisplay();    // Clear everything that is stored in frame buffer of epaper
            display.setCursor(0, 0);   // Set print position at the begining of the screen
            char text[3001];           // Array where data from SD card is stored (max 200 chars here)
            int len = file.fileSize(); // Read how big is file that we are opening
            if (len > 3000)
                len = 3000;        // If it's more than 200 bytes (200 chars), limit to max 200 bytes
            file.read(text, len);  // Read data from file and save it in text array
            text[len] = 0;         // Put null terminating char at the and of data
            display.print(text);   // Print data/text
            display.sdCardSleep(); // Put sd card in sleep mode
            display.display();     // Do a full refresh of display
        }
    }
    else
    { // If card init was not successful, display error on screen, put sd card in sleep mode, and stop the program
      // (using infinite loop)
        display.println("SD Card error!");
        display.display();
        display.sdCardSleep();
        while (true)
            ;
    }
}

void loop()
{
    // Nothing...
}