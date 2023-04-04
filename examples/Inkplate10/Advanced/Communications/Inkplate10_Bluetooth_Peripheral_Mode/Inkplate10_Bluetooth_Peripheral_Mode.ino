/*
   Inkplate10_Bluetooth_Peripheral_Mode example for Soldered Inkplate 10
   For this example you will need USB cable and an Inkplate 10
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Inkplate as a peripheral device over Bluetooth.
   More about peripheral mode: https://inkplate.readthedocs.io/en/latest/peripheral-mode.html

   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send commands from your phone. Don't forget you need to send #L(1)* after
   each command to show it on the display (equal to display->display()).

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

// Include peripheral functions
#include "Peripheral.h"

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

// Create SerialBT object for Bluetooth communication
BluetoothSerial SerialBT;

// Size of buffer for receiving commands
#define BUFFER_SIZE 1000

// Temporary buffer to send to Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];

void setup() // Initialize everything
{
    // Init serial communication
    Serial.begin(115200);

    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Init BT communication
    if (!SerialBT.begin("Inkplate10"))
    {
        Serial.println("An error occurred initializing Bluetooth");
    }
    else
    {
        Serial.println("The device started, now you can pair it with Bluetooth and send commands!");
    }
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
    }

    // Function in peripheral.h
    run(commandBuffer, BUFFER_SIZE, &display, &SerialBT);

    // Wait a bit
    delay(50);
}
