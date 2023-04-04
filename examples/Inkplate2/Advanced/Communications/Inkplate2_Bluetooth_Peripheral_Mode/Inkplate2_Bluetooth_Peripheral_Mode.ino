/*
    Inkplate2_Bluetooth_Peripheral_Mode example for Soldered Inkplate 2
    For this example you will need USB cable, an Inkplate 2 and bluetooth supporting device
    like phone or laptop. If you are using phone, you will need to download app which
    allows you to send commands via bluetooth like "Bluetooth serial monitor".
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example shows how to use Inkplate as a peripheral device over Bluetooth.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    29 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"

#include "BluetoothSerial.h"

// Size of the receive buffer for commands (in bytes).
#define BUFFER_SIZE 1000

#include "Peripheral.h"

// Declare Inkplate and SerialBluetooth objects
Inkplate display;
BluetoothSerial SerialBT;

// Temporary data buffer for Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];

void setup() // Initialize everything
{
    Serial.begin(115200); // Initialize UART communication with PC

    if (!SerialBT.begin("Inkplate2")) // Check if bluetooth is initialized successfully
    {
        Serial.println("An error occurred initializing Bluetooth"); // If not, print error message
    }

    display.begin(); // Initialize Inkplate library.
}

void loop()
{
    while (SerialBT.available()) // When there is some data available save it and pass to Peripheral.h code
    {
        for (int i = 0; i < (BUFFER_SIZE - 1); i++) // Loop through all commands
        {
            commandBuffer[i] = commandBuffer[i + 1];
        }
        commandBuffer[BUFFER_SIZE - 1] = SerialBT.read(); // Save incoming command in buffer
        Serial.print(commandBuffer[BUFFER_SIZE - 1]);
    }

    // Function in peripheral.h
    run(commandBuffer, BUFFER_SIZE);

    delay(50);
}
