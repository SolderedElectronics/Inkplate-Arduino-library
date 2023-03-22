/*
   Inkplate5_Bluetooth_Peripheral_Mode example for Soldered Inkplate 5
   For this example you will need USB cable, Inkplate 5 and smartphone.
   Select "Soldered Inkplate5" from Tools -> Board menu.
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Inkplate as a peripheral device over Bluetooth.
   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send commands from your phone.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   21 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

#include "Peripheral.h"

#define BUFFER_SIZE 1000

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

// Temporary buffer to send to Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];

void setup()
{
    Serial.begin(115200);   // Init serial communication for debugging
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.setTextSize(4); // Scale text to be 4 times bigger then original (5x7 px)

    // Init BT communication
    if (!SerialBT.begin("Inkplate5"))
    {
        Serial.println("An error occurred initializing Bluetooth");
        display.println("An error occurred initializing Bluetooth");
    }
    else
    {
        display.println("The device started, now you can pair it with Bluetooth and send commands!");
    }

    // Show a message on the screen
    display.display();
}

void loop()
{
    // When Bluetooth available save it and pass to Peripheral.h code
    while (SerialBT.available())
    {
        for (int i = 0; i < (BUFFER_SIZE - 1); i++)
        {
            commandBuffer[i] = commandBuffer[i + 1];
        }
        commandBuffer[BUFFER_SIZE - 1] = SerialBT.read();
        Serial.print(commandBuffer[BUFFER_SIZE - 1]);
    }

    // Function in peripheral.h
    run(commandBuffer, BUFFER_SIZE);

    // Wait a bit
    delay(50);
}
