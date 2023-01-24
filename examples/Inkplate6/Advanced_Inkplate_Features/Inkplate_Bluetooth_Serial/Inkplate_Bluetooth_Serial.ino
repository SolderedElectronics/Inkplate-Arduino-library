/*
   Inkplate_Bluetooth_Serial example for Soldered Inkplate6
   For this example you will need USB cable, Inkplate6 and smartphone.
   Select "e-radionica Inkplate6" or "Soldered Inkplate6" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows how to use Bluetooth on Inkplate devices.
   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send anything from your phone to an Inkplate or vice versa.
   On the Serial Monitor, you will see what the phone sends while in the app you will see what the Inkplate
   sends over the Serial Monitor.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   24 January 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate6";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(7); // Scale text to be 7 times bigger then original (5x7 px)
    display.printf("\nBluetooth Serial \nExample\n");
    display.partialUpdate(); // Display on the screen
    display.setTextSize(5);

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
    display.partialUpdate();
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
