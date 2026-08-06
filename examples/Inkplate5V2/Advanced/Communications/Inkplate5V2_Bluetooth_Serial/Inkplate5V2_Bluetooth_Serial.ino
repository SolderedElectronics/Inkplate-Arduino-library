
/**
 **************************************************
 * @file        Inkplate5V2_Bluetooth_Serial.ino
 * @brief       Bluetooth SPP serial bridge example for Soldered Inkplate 5v2.
 *
 * @details     Demonstrates how to use classic Bluetooth (SPP profile) on the
 *              Inkplate 5v2 to exchange text data between the board and a
 *              smartphone. Text received from the phone is shown on the
 *              Inkplate display and logged to the Serial Monitor, while text
 *              sent from the Inkplate (via Serial Monitor) is forwarded to
 *              the connected phone.
 *
 *              Expected output is text received over Bluetooth shown on the
 *              Inkplate display, with sent and received messages logged in the
 *              Serial Monitor. Bluetooth pairing and app behavior may differ
 *              between Android and iOS; some iOS apps do not support classic
 *              Bluetooth SPP.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      Bluetooth serial terminal app (e.g. Serial Bluetooth Terminal)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5v2.
 * 3) Pair the Inkplate with your phone in Bluetooth settings.
 * 4) Open a Bluetooth serial terminal app and connect to Inkplate.
 * 5) Send text from the phone and verify it appears on the Inkplate display
 *    and in the Serial Monitor.
 * 6) Send text from the Serial Monitor and verify it appears in the phone app.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
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
