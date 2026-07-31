/**
 **************************************************
 * @file        Inkplate2_Bluetooth_Serial.ino
 * @brief       Bluetooth Serial (SPP) example for Inkplate 2 with data
 *              forwarding between smartphone and Serial Monitor.
 *
 * @details     This example demonstrates classic Bluetooth Serial Port
 *              Profile (SPP) communication on the Inkplate 2 (ESP32-based).
 *              After pairing a smartphone with the device, a Bluetooth serial
 *              terminal app can be used to exchange text data.
 *
 *              Data typed in the Arduino Serial Monitor is sent to the phone
 *              over Bluetooth. Data received from the phone is forwarded to
 *              the Serial Monitor. The e-paper display is used only for status
 *              messages during initialization.
 *
 *              Inkplate 2 operates in 1-bit (black/white) display mode. This
 *              example performs a single full display refresh during setup and
 *              does not use partial updates. This is classic Bluetooth SPP (not
 *              BLE), so pairing is required, and no deep sleep is used - the
 *              ESP32 remains active while powered.
 *
 *              Expected output: initialization and pairing instructions shown
 *              once at startup on the display, all data received from the
 *              smartphone shown in the Serial Monitor, and all data sent from
 *              the Serial Monitor shown in the phone app.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Smartphone + Bluetooth serial terminal app
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Optionally edit btDeviceName to change the Bluetooth name, then upload the
 *    sketch.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Pair your phone with the device name defined in btDeviceName.
 * 5) Open a Bluetooth serial terminal app and connect to Inkplate 2.
 * 6) Type in Serial Monitor to send data to the phone.
 * 7) Type in the phone app to send data back to the Serial Monitor.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-17
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate2";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display;         // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT; // Create SerialBT object for Bluetooth communication


void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Scale text to be 2 times bigger then original (5x7 px)
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.println("Bluetooth Serial Example");
    display.println();
    display.setTextSize(1);

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

        display.println("Open Serial Monitor at 115200");
        display.println("baud!");
    }

    // Init serial communication
    Serial.begin(115200);

    // Display messages on the screen
    display.display();
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
            Serial.write(SerialBT.read());
        }
    }

    // Wait a bit
    delay(20);
}
