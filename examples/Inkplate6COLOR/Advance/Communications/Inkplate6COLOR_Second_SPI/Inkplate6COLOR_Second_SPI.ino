/*
   Inkplate6COLOR_Second_SPI example for Soldered Inkplate 6COLOR
   For this example you will need a USB-C cable, Inkplate 6COLOR, MFRC522 RFID reader, RFID tag and few wires.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered InkplateCOLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Connect RFID reader to the following wiring diagram:
   3.3V - 3V3
   RST  - not used
   GND  - GND
   IRQ  - not used
   MISO - IO12
   MOSI - IO13
   SCK  - IO14
   SDA  - IO15

   This example will show you how you can read a tag ID and print it on the Serial Monitor.

   NOTE: you have to cut JP6 and connect the other 2 contacts in order to work!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   2 February 2023 by Soldered Electronics.
*/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "SPI.h"
#include "MFRC522.h"

// Pins for the MFRC522
#define RST_PIN         -1 // Not used
#define SS_PIN          15 // SDA pin

// Create MFRC522 object
MFRC522 rfid(SS_PIN, RST_PIN);

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display;

void setup() {
  // Init Inkplate library (you should call this function ONLY ONCE)
  display.begin();
  
  // Init SPI bus
  SPI.begin(14, 12, 13, 15);  

  // Init serial communication
  Serial.begin(115200);

  // Init MFRC522
  rfid.PCD_Init();

  // Print a message on serial
  Serial.println("Approximate a card to the reader!");
}

void loop() {
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
      for (int i = 0; i < rfid.uid.size; i++) {
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
