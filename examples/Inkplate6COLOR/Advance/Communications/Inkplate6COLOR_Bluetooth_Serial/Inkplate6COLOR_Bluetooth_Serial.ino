/*
   Inkplate6COLOR_Bluetooth_Serial example for Soldered Inkplate 6COLOR
   For this example you will need USB cable, Inkplate 6COLOR and smartphone.
   Select "e-radionica Inkplate 6COLOR" or "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 6COLOR" or "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Bluetooth on Inkplate devices.
   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send anything from your phone to an Inkplate or vice versa.
   On the Serial Monitor, you will see what the phone sends while in the app you will see what the Inkplate
   sends over the Serial Monitor.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   24 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate 6COLOR";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display; // Create an object on Inkplate library
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(5); // Scale text to be 6 times bigger then original (5x7 px)
    display.setTextColor(INKPLATE_BLACK);
    display.printf("\nBluetooth Serial \nExample\n");
    display.setTextSize(4);

    // Init SerialBT
    if (!SerialBT.begin(btDeviceName))
    {
        // Something goes wrong, print an error message
        display.printf("\nAn error occurred initializing Bluetooth");
    }
    else
    {
        // Bluetooth started successfully, print the messages on the screen
        display.printf("\nThe device started, now \nyou can pair it with \nBluetooth!\n");
        display.printf("\nOpen Serial Monitor at \n115200 baud!");
    }

    // Init serial communication
    Serial.begin(115200);

    // Display messages on th escreen
    display.display();
}

void loop()
{
    // If you type something on the serial port, send that data via Bluetooth to the connected phone
    if (Serial.available())
    {
        SerialBT.write(Serial.read());
    }

    // If there are bytes available to read in the Bluetooth port, display those data to the Serial Monitor
    if (SerialBT.available())
    {
        Serial.write(SerialBT.read());
    }

    // Wait a bit
    delay(20);
}
