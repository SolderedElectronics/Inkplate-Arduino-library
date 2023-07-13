/*
   InkplatePLUS2_Bluetooth_Serial example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2 and smartphone.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
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
   13 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "InkplatePLUS2";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

BluetoothSerial SerialBT; // Create SerialBT object for Bluetooth communication

void setup()
{
    Serial.begin(115200);      // Init serial communication
    display.begin();           // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();    // Clear frame buffer of display
    display.setTextSize(3);    // Scale text to be 3 times bigger then original (5x7 px)
    display.setCursor(90, 20); // Set the cursor so text is centered
    display.println("Bluetooth Serial Example");
    display.setTextSize(2); // Scale text to be 2 times bigger then original (5x7 px)
    display.setCursor(90, 75); // Set the cursor so the following text is centered
    display.println("Open the Serial Monitor at 115200 baud");
    display.setCursor(90, 95);
    display.println("to see sent messages");
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
