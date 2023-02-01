/*
   Inkplate_SPI example for Soldered Inkplate10
   For this example you will need a USB-C cable, Inkplate10, MFRC522 RFID reader, RFID tag and few wires.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

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
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   1 February 2023 by Soldered Electronics
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "MFRC522.h"
#include "SPI.h"

// Pins for the MFRC522
#define RST_PIN -1 // Not used
#define SS_PIN  15 // SDA pin

// Create MFRC522 object
MFRC522 rfid(SS_PIN, RST_PIN);

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

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
    display.setTextSize(6);

    // Clear the display and print the message
    display.clearDisplay();
    display.printf("Approximate a card to the \nreader!");
    display.display();
}

void loop()
{
    // If the new tag is available
    if (rfid.PICC_IsNewCardPresent())
    {
        // If the NUID has been read
        if (rfid.PICC_ReadCardSerial())
        {
            // Clear the frame buffer of the screen and set the cursor to the beginning
            display.clearDisplay();
            display.setCursor(0, 0);

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
            display.display();

            // Halt PICC
            rfid.PICC_HaltA();

            // Stop encryption on PCD
            rfid.PCD_StopCrypto1();
        }
    }
}
