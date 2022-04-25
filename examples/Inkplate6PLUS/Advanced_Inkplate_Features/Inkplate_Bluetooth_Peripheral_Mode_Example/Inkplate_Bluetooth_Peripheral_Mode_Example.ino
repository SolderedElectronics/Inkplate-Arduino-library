/*
   Inkplate_Bluetooth_Peripheral_Mode_Example example for e-radionica Inkplate 6
   For this example you will need USB cable and an Inkplate 6
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows how to use Inkplate as a peripheral device over Bluetooth.
   Note: for this to work you need to use ESP32 Wroover Board definition, as ours currently has a bug :(

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h"

#include "BluetoothSerial.h"

#define BUFFER_SIZE 1000

#include "Peripheral.h"

// Declare Inkplate and SerialBluetooth objects
Inkplate display(INKPLATE_1BIT);
BluetoothSerial SerialBT;

// Temporary buffer to send to Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];


// If your Inkplate doesn't have external (or second) MCP I/O expander, you should uncomment next line,
// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander
//#define ONE_MCP_MODE

void setup() // Initialize everything
{
    Serial.begin(115200);

    if (!SerialBT.begin("ESP32"))
    {
        Serial.println("An error occurred initializing Bluetooth");
    }

    display.begin();
}

void loop()
{
    while (SerialBT.available()) // When Bluetooth available save it and pass to Peripheral.h code
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

    delay(50);
}