/*
   Inkplate_Bluetooth_Peripheral_Mode_Example example for Soldered Inkplate 10
   For this example you will need USB cable and an Inkplate 10
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows how to use Inkplate as a peripheral device over Bluetooth.
   Note: for this to work you need to use ESP32 Wroover Board definition, as ours currently has a bug :(

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"

#include "BluetoothSerial.h"

#define BUFFER_SIZE 1000

#include "Peripheral.h"

// Declare Inkplate and SerialBluetooth objects
Inkplate display(INKPLATE_1BIT);
BluetoothSerial SerialBT;

// Temporary buffer to send to Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];

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