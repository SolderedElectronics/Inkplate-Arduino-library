/*
   Inkplate5V2_Second_SPI example for Soldered Inkplate 5 V2
   For this example you will need a USB-C cable, Inkplate5, MFRC522 RFID reader, RFID tag and few wires.
   Select "Soldered Inkplate5 V2" from Tools -> Board menu.
   Don't have "Soldered Inkplate5 V2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use a second SPI for any breakout that uses SPI communication. 
   It's important to specify pins inside SPI.begin() or else the example won't work. 
   This example will show you how you can read an RFID tag ID with the MFRC522 breakout 
   and print it on the Inkplate screen. When you approximate a card to the reader, 
   if everything is connected properly, on the Inkplate screen will be displayed the card type and ID.

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

   IMPORTANT: you have to cut JP3 and connect/solder the other 2 contacts in order to work!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 April 2024 by Soldered
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
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

// Font scale for the text on the screen. The scale of 1 is 7px in height and 5 pixels in width
#define BIG_TEXT_SCALE   4
#define SMALL_TEXT_SCALE 3

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Init SPI bus
    // IMPORTANT: you have to specify pins!
    // The pins are: SPI.begin(SCK, MISO, MOSI, SS);
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
        // If the ID has been read
        if (rfid.PICC_ReadCardSerial())
        {
            // Check if the content fits on the screen expecting 2 lines of the text
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
