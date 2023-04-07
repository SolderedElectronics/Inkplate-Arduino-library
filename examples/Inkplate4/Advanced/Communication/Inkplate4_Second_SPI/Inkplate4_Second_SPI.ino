/*
   Inkplate4_Second_SPI example for Soldered Inkplate 4
   For this example you will need a USB-C cable, Inkplate 4, MFRC522 RFID reader, RFID tag and few wires.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use a second SPI for any breakout that uses SPI communication.
   It's important to specify pins inside SPI.begin() or else the example won't work.
   This example will show you how you can read an RFID tag ID with the MFRC522 breakout
   and print it on the Serial MOnitor. When you approximate a card to the reader,
   if everything is connected properly, on the Serial Monitor will be printed the card type and ID.

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

   IMPORTANT: you have to cut JP2 and connect the other 2 contacts in order to work!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   6 April 2023 by Soldered Electronics.
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

// Include needed libraries
//#include "Inkplate.h"
#include "MFRC522-SOLDERED.h"
#include "SPI.h"

// Pins for the MFRC522
#define RST_PIN -1 // Not used
#define SS_PIN  15 // SDA pin

// Create MFRC522 object
MFRC522 rfid(SS_PIN, RST_PIN);

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
//Inkplate display;

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    //display.begin();

    // Init SPI bus
    SPI.begin(14, 12, 13, 15);

    // Init serial communication
    Serial.begin(115200);

    // Init MFRC522
    rfid.PCD_Init();

    // Print a message on serial
    Serial.println("Approximate a card to the reader!");
}

void loop()
{
    // If the new tag is available
    if (rfid.PICC_IsNewCardPresent())
    {
        // If the NUID has been read
        if (rfid.PICC_ReadCardSerial())
        {
            // Get tag type and print it
            MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
            Serial.print("RFID Tag Type: ");
            Serial.println(rfid.PICC_GetTypeName(piccType));

            // Print UID in Serial Monitor in the hex format
            Serial.print("UID:");
            for (int i = 0; i < rfid.uid.size; i++)
            {
                Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(rfid.uid.uidByte[i], HEX);
            }
            Serial.println();

            // Halt PICC
            rfid.PICC_HaltA();

            // Stop encryption on PCD
            rfid.PCD_StopCrypto1();
        }
    }
}
