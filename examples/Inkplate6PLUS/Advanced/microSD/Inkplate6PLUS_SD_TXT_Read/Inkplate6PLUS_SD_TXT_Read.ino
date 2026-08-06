/**
 **************************************************
 * @file        Inkplate6PLUS_SD_TXT_Read.ino
 * @brief       Read and display text file from SD card on Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to open a .txt file from a FAT-formatted SD card
 *              and display its contents on the Inkplate 6PLUS e-paper display.
 *              The example reads a file named "text.txt" from the SD card and
 *              prints its content on screen.
 *
 *              The file name must be exactly "text.txt" for this example and the
 *              text length should not exceed 200 characters. Make sure the
 *              required SDFat library is properly installed. Expected output is
 *              the contents of text.txt shown on the Inkplate display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable, microSD card
 * - Extra:      text.txt file on SD card (max 200 characters)
 * - Library:    https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library
 * - SD card:    FAT / FAT32 format
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Copy a text file named "text.txt" (max 200 characters) to a FAT-formatted
 *    SD card.
 * 3) Insert the SD card into the Inkplate.
 * 4) Install the required SDFat library.
 * 5) Upload the sketch to Inkplate 6PLUS.
 * 6) The file contents will be read and displayed on the e-paper screen.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;                     // Create SdFile object used for accessing files on SD card

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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
