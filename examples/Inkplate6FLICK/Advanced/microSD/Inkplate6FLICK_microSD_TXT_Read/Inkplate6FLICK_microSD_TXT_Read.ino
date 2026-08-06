/**
 **************************************************
 * @file        Inkplate6FLICK_microSD_TXT_Read.ino
 * @brief       Read and display a text file from an SD card on Soldered
 *              Inkplate 6FLICK.
 *
 * @details     Shows how to open .txt files and display the content of the file
 *              on the Inkplate e-paper display. You can open your own .txt file,
 *              but for this example to work properly it should not have more
 *              than 200 characters and it should be named text.txt.
 *
 *              To work with the SD card on Inkplate, you need to add one extra
 *              library - download and install it from
 *              https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, micro USB cable, SD card loaded with the
 *               text.txt file found inside the folder of this example
 * - Library:    https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library
 * - Extra:      text.txt must be max 200 characters
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Install the SDFat library linked above.
 * 3) Copy text.txt to the SD card and insert it into the Inkplate.
 * 4) Upload the sketch to Inkplate 6FLICK.
 * 5) The contents of text.txt are shown on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Adding the board definition to the Arduino IDE:
 *              https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/
 *
 * @author      Soldered
 * @date        2024-03-15
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setRotation(1);
    display.frontlight.setState(15);
    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        display.println("SD Card ok! Reading data...");
        display.partialUpdate();

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
