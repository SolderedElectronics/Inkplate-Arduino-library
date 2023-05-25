/**
 **************************************************
 * @file        InkplateEasyCTester.ino
 *
 * @brief       Sketch for converting Dasduino Board (Dasduino Core or Dasduino ConnectPlus
 *              into slave I2C device used for testing easyC connector.
 *
 *              It will respond on 0x30 I2C address by I2C ACK and it will also blink an built-in
 *              WS2812 LED green, showing that it received something on I2C bus.
 *
 *              You will need:
 *              - Dasduino Core (https://soldered.com/product/dasduino-core/) or
 *                Dasduino ConnectPlus (https://soldered.com/product/dasduino-connectplus/)
 *              - easyC cable (https://soldered.com/product/easyc-cable-20cm/)
 *              - Soldered Library For WS2812 LEDs
 *                (https://github.com/SolderedElectronics/Soldered-WS2812-Smart-Leds-Arduino-Library)
 *              - Dasduino Board Definition installed on your Arduino:
 *                (https://github.com/SolderedElectronics/Dasduino-Board-Definitions-for-Arduino-IDE/blob/master/README.md)
 *
 * License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 * LICENSE file included with this example. If you have any questions about
 * licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 * warranty is given.
 *
 * Want to learn more about Inkplate? Visit www.inkplate.io
 * Looking to get support? Write on our forums: https://forum.soldered.com/
 * 25 April 2023 by Soldered
 *
 * @authors     Borna Biro for Soldered
 ***************************************************/

// Block usage of this code on Inkplate boards.
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)

// Include a Wire library (for I2C communication)
#include "Wire.h"

// Include WS2812 LED Library (for built-in LED)
#include "WS2812-SOLDERED.h"

// Define I2C address (this address MUST match the address defined in Inkplate Factory Programming example!)
#define I2C_DEV_ADDR 0x30

// Create an object for WS2812 LEDs (use only one LED).
WS2812 pixels(1, LEDWS_BUILTIN);

// Flag for incoming I2C data (must be volatile!).
volatile uint8_t flag = 0;

// Variable for LED timeout.
unsigned long timeout = 0;

// How long to keep LED on (in milliseconds)
#define LED_TIMEOUT 150UL

// Function is automatically called when there is a request for reading the I2C data.
void onRequest()
{
    // Set the flag (needed in the loop).
    flag = 1;

    // Send some dummy data.
    Wire.write(0);
}
// Function is automatically called when there is incoming I2C data.
void onReceive(int len)
{
    // Set the flag (needed in the loop).
    flag = 1;

    // Flush the RX buffer.
    while (Wire.available())
    {
        Wire.read();
    }
}

void setup()
{
    // Init the WS2812 LED library.
    pixels.begin();

    // Set callbacks for receive and for request on I2C.
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    // Init the Wire library by setting slave I2C address.
    Wire.begin((uint8_t)I2C_DEV_ADDR);
}

void loop()
{
    // If there was something on 0x30 I2C address, show a green light with an LED.
    if (flag == 1)
    {
        // Clear the flag.
        flag = 0;

        // Show green color on the LED.
        pixels.setPixelColor(0, pixels.Color(0, 80, 0));
        pixels.show();

        // Set the timeout for LED (how long LED needs to be turned on).
        timeout = millis();
    }

    // If a timeout for LED occurs, turn off the LED.
    if (((unsigned long)(millis() - timeout) > LED_TIMEOUT) && timeout != 0)
    {
        timeout = 0;
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.show();
    }
}
#else
// Just to pass compile test.
void setup()
{

}

void loop()
{

}
#endif