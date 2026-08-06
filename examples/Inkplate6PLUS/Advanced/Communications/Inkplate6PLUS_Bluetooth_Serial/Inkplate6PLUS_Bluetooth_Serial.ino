/**
 **************************************************
 * @file        Inkplate6PLUS_Bluetooth_Serial.ino
 * @brief       Bluetooth Serial demo: exchange text between Inkplate and a
 *              smartphone, and show received data on the e-paper display.
 *
 * @details     This example starts a Bluetooth SPP (Serial Port Profile)
 *              service using ESP32 BluetoothSerial. Pair a smartphone with the
 *              Inkplate, then connect using a Bluetooth serial terminal app.
 *              Data typed in the Arduino Serial Monitor is forwarded to the
 *              phone over Bluetooth. Data received from the phone is rendered
 *              on the e-paper display.
 *
 *              The sketch uses 1-bit (black/white) display mode and relies on
 *              partial updates for fast refresh - partial update is supported
 *              only in 1-bit mode. When the text reaches the bottom of the
 *              screen, the framebuffer is cleared and a full refresh is
 *              performed to prevent ghosting buildup; for best image quality a
 *              periodic full refresh is recommended, which this sketch does
 *              automatically when the screen is full. This is classic Bluetooth
 *              SPP (not BLE), so pairing is required before use.
 *
 *              Expected output: a "Bluetooth Serial Example" header on the
 *              display, then received Bluetooth text appended line-by-line with
 *              partial updates and a full refresh when the screen fills; the
 *              phone app shows bytes sent from the Arduino Serial Monitor via
 *              Bluetooth.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      Smartphone + Bluetooth serial terminal app
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Optionally edit btDeviceName to change the advertised Bluetooth name, then
 *    upload the sketch.
 * 3) Open Serial Monitor at 115200 baud (optional, for sending text to phone).
 * 4) On your phone, pair with the advertised device name (btDeviceName).
 * 5) Open a Bluetooth serial terminal app and connect to the Inkplate.
 * 6) Type in the app to display text on Inkplate; type in Serial Monitor to
 *    send text back to the phone.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
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
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

// You can change the Bluetooth device name if you want
const char *btDeviceName = "Inkplate 6PLUS";

// Include Inkplate and BluetoothSerial library to the sketch
#include "BluetoothSerial.h"
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
BluetoothSerial SerialBT;        // Create SerialBT object for Bluetooth communication

// Font scale for the text on the screen. The scale of 1 is 7px height
#define BIG_TEXT_SCALE   4
#define SMALL_TEXT_SCALE 3

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();                     // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();              // Clear frame buffer of display
    display.setTextSize(BIG_TEXT_SCALE); // Scale text to be 5 times bigger then original (5x7 px)
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
