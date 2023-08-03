/*
   Inkplate7_Bluetooth_Serial example for Soldered Inkplate 7
   For this example you will need a USB-C cable, Inkplate 7 and smartphone.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Bluetooth on Inkplate devices.
   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send anything from your phone to an Inkplate or vice versa.
   On the Serial Monitor you will see what the phone sends while in the app you will see what the Inkplate
   sends over the Serial Monitor.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   25 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate7";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display;         // Create an object on Inkplate library
BluetoothSerial SerialBT; // Create SerialBT object for Bluetooth communication

void setup()
{
    Serial.begin(115200);   // Init serial communication
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(3); // Scale text to be 3 times bigger then original (5x7 px)
    display.println("Bluetooth Serial Example\n");
    display.setTextSize(2);
    display.println("Open the Serial Monitor at 115200 baud to see sent\nmessages");
    display.display(); // Display on the screen

    // Init SerialBT
    if (!SerialBT.begin(btDeviceName))
    {
        // Something goes wrong, print an error message
        Serial.println("An error occurred initializing Bluetooth");
    }
    else
    {
        // Bluetooth started successfully, print the messages on the screen
        Serial.println("The device started, now you can pair it with Bluetooth and send messages");
    }

    // Init serial communication
    Serial.begin(115200);
}

void loop()
{
    // If you type something on the serial port, send that data via Bluetooth to the connected phone
    if (Serial.available())
    {
        // Load the whole message and send it
        while (Serial.available())
        {
            SerialBT.write(Serial.read());
        }
    }

    // If there are bytes available to read in the Bluetooth port, display those data on the screen
    if (SerialBT.available())
    {
        // Load the whole message and print it on the screen
        while (SerialBT.available())
        {
            char c = SerialBT.read();
            Serial.print(c);
        }
    }

    // Wait a bit
    delay(20);
}
