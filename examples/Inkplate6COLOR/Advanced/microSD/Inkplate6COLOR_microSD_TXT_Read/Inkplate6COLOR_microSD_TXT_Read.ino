/**
 **************************************************
 * @file        Inkplate6COLOR_microSD_TXT_Read.ino
 * @brief       Reads a text file from an SD card and displays its contents on
 *              Inkplate 6COLOR.
 *
 * @details     This example demonstrates how to access a microSD card on
 *              Inkplate 6COLOR, open a text file, read its contents, and render
 *              the text on the e-paper display.
 *
 *              The sketch initializes the SD card, opens a file named
 *              "text.txt", reads its contents into a memory buffer, and prints
 *              the text on the screen. The file is accessed using the SdFat
 *              file system interface provided by the Inkplate SD integration.
 *
 *              This example is intended as a simple demonstration of reading
 *              text data from removable storage. It can be adapted for use
 *              cases such as displaying configuration files, logs, notes,
 *              menus, or dynamically updated content stored on an SD card.
 *
 *              The example limits how much data is read into RAM to prevent
 *              excessive memory usage, so large files are truncated to fit the
 *              buffer. The SD card power rail is disabled with sdCardSleep()
 *              after reading to reduce power consumption. Display mode is
 *              Inkplate 6COLOR colour e-paper mode and a full display refresh is
 *              performed when showing the text.
 *
 *              Expected output: the contents of text.txt rendered on the screen,
 *              or an error message if the SD card or file cannot be opened.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      microSD card formatted as FAT/FAT32 containing text.txt
 * - Library:    SdFat SD library support, if not already included
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Format a microSD card using FAT or FAT32.
 * 3) Create a file named "text.txt" and place it in the root directory of the
 *    SD card.
 * 4) Insert the SD card into the Inkplate 6COLOR slot.
 * 5) Upload the sketch to the board.
 * 6) The sketch initializes the SD card, opens the file, reads its contents,
 *    and prints the text on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
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
