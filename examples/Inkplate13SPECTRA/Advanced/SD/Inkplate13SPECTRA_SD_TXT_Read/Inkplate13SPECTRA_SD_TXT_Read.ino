/**
 **************************************************
 * @file        Inkplate13SPECTRA_SD_TXT_Read.ino
 * @brief       Read and display a text file from an SD card on Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Shows how to open .txt files and display the content of the file
 *              on the Inkplate e-paper display. You can open your own .txt file,
 *              but for this example to work properly it should not have more than
 *              200 characters and it should be named text.txt.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable, SD card loaded with the
 *               text.txt file found inside the folder of this example
 * - Extra:      text.txt must be max 200 characters
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Copy text.txt to the SD card and insert it into the Inkplate.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The contents of text.txt are shown on the e-paper display.
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

#include "Inkplate.h" //Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library
SdFile file;          // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setCursor(0, 0);
    display.setTextColor(INKPLATE_BLACK);
    display.setTextSize(2);
    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok! Reading data...");
        display.display();

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
