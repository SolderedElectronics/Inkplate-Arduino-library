/*
   Inkplate5V2_Bluetooth_Serial example for Soldered Inkplate 5 V2
   For this example you will need a USB-C cable, Inkplate 5 and smartphone.
   Select "Soldered Inkplate5 V2" from Tools -> Board menu.
   Don't have "Soldered Inkplate5 V2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Bluetooth on Inkplate devices.
   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send anything from your phone to an Inkplate or vice versa.
   On the screen, you will see what the phone sends while in the app you will see what the Inkplate
   sends over the Serial Monitor.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 April 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate5V2";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

// Font scale for the text on the screen
#define BIG_TEXT_SCALE   4
#define SMALL_TEXT_SCALE 3

void setup()
{
    display.begin();                     // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();              // Clear frame buffer of display
    display.setTextSize(BIG_TEXT_SCALE); // Scale text to be 4 times bigger then original (5x7 px)
    display.println("Bluetooth Serial Example");
    display.println();
    display.display(); // Display on the screen

    // Init SerialBT
    if (!SerialBT.begin(btDeviceName))
    {
        // Something goes wrong, print an error message
        display.println("An error occurred initializing Bluetooth");
    }
    else
    {
        // Bluetooth started successfully, print the messages on the screen
        display.println("The device started, now you can ");
        display.println("pair it with Bluetooth and send");
        display.println("messages");
        display.println();
    }

    // Init serial communication
    Serial.begin(115200);

    // Display messages on the screen
    display.partialUpdate();

    display.setTextSize(SMALL_TEXT_SCALE);
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
        // Check if the content fits on the screen expecting one line of the text
        if (display.getCursorY() + SMALL_TEXT_SCALE * 7 > E_INK_HEIGHT)
        {
            // Clear the frame buffer of the display and set cursor to the beginning of the screen
            display.setCursor(0, 0);
            display.clearDisplay();

            // Do a full refresh
            display.display();
        }

        // Load the whole message and print it on the screen
        while (SerialBT.available())
        {
            char c = SerialBT.read();
            display.print(c);
        }

        // Actually display the message
        display.partialUpdate();
    }

    // Wait a bit
    delay(20);
}
