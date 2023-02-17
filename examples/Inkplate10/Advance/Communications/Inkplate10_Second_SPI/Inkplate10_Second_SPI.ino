/*
   Inkplate10_Second_SPI example for Soldered Inkplate 10
   For this example you will need a USB-C cable, Inkplate10, MFRC522 RFID reader, RFID tag and few wires.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   For this example you have to install Soldered MFRC522 library:
   https://github.com/SolderedElectronics/Soldered-MFRC522-RFID-Reader-Arduino-Library

   Connect RFID reader to the following wiring diagram:
   3.3V - 3V3
   RST  - not used
   GND  - GND
   IRQ  - not used
   MISO - IO12
   MOSI - IO13
   SCK  - IO14
   SDA  - IO15

   This example will show you how you can read a tag ID and print it on the Inkplate screen.

   NOTE: you have to cut JP3 and connect the other 2 contacts in order to work!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   17 February 2023 by Soldered Electronics
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "MFRC522-SOLDERED.h"
#include "SPI.h"

// Pins for the MFRC522
#define RST_PIN -1 // Not used
#define SS_PIN  15 // SDA pin

// Create MFRC522 object
MFRC522 rfid(SS_PIN, RST_PIN);

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

// Font scale for the text on the screen. The scale of 1 is 7px height
#define BIG_TEXT_SCALE   4
#define SMALL_TEXT_SCALE 3

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Init SPI bus
    SPI.begin(14, 12, 13, 15);

    // Init MFRC522
    rfid.PCD_Init();

    // Set text setings
    display.setTextColor(BLACK);
    display.setTextSize(BIG_TEXT_SCALE);

    // Clear the display and print the message
    display.clearDisplay();
    display.println("Approximate a card to the reader!");
    display.display();

    // Set a smaller font size for displaying the card information
    display.setTextSize(SMALL_TEXT_SCALE);
    display.println();
}

void loop()
{
    // If the new tag is available
    if (rfid.PICC_IsNewCardPresent())
    {
        // If the NUID has been read
        if (rfid.PICC_ReadCardSerial())
        {
            // Check if the content fits on the screen expecting 2 line of the text
            if (display.getCursorY() + 2 * SMALL_TEXT_SCALE * 7 > E_INK_HEIGHT)
            {
                // Clear the frame buffer of the display and set cursor to the beginning of the screen
                display.setCursor(0, 0);
                display.clearDisplay();

                // Do a full refresh
                display.display();
            }

            // Get tag type and print it
            MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
            display.print("RFID Tag Type: ");
            display.println(rfid.PICC_GetTypeName(piccType));

            // Print tag ID to the display in the hex format
            display.print("ID:");
            for (int i = 0; i < rfid.uid.size; i++)
            {
                display.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
                display.print(rfid.uid.uidByte[i], HEX);
            }
            display.println();
            display.println();
            display.partialUpdate();

            // Halt PICC
            rfid.PICC_HaltA();

            // Stop encryption on PCD
            rfid.PCD_StopCrypto1();
        }
    }
}
