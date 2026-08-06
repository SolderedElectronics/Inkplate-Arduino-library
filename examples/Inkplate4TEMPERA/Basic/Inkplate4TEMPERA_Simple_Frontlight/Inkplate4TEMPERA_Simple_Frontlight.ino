/**
 **************************************************
 * @file        Inkplate4TEMPERA_Simple_Frontlight.ino
 * @brief       Control the Inkplate 4 TEMPERA frontlight brightness via the
 *              Serial Monitor.
 *
 * @details     This example enables the Inkplate 4 TEMPERA frontlight circuit
 *              and lets you change brightness interactively over Serial. Send
 *              '+' to increase brightness and '-' to decrease it. The current
 *              level is printed back to the Serial Monitor as a value from
 *              0 to 63.
 *
 *              Sending 's' runs a simple "light show" that ramps the frontlight
 *              up and down several times to demonstrate the full brightness
 *              range. The demo uses delays, so Serial input is not processed
 *              during the ramp animation.
 *
 *              This sketch runs the display in 1-bit (BW) mode, but it does not
 *              draw anything on the e-paper panel; only the frontlight is
 *              controlled. Brightness is clamped to the valid range (0-63).
 *
 *              Expected output: Serial prints the current brightness as
 *              "Frontlight:<value>/63" after a change or after the demo
 *              completes, and the frontlight intensity changes immediately.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud, Newline: "No line ending" recommended
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) Open the Serial Monitor at 115200 baud.
 * 4) Send '+' to increase brightness (max 63).
 * 5) Send '-' to decrease brightness (min 0).
 * 6) Send 's' to run the brightness ramp demo.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-12
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the frontlight

void setup()
{
    Serial.begin(115200);    // Set up a serial communication of 115200 baud
    display.begin();         // Init Inkplate library
    display.frontlight.setState(true); // Enable frontlight circuit
    display.frontlight.setBrightness(b); // Set frontlight intensity
}

void loop()
{
    if (Serial.available()) // Change frontlight value by sending "+" sign into serial monitor to increase frontlight or
                            // "-" sign to decrese frontlight
                            // try to find hidden lightshow ;)
    {
        bool change = false;    // Variable that indicates that frontlight value has changed and intessity has to be updated
        char c = Serial.read(); // Read incomming serial data

        if (c == '+' && b < 63) // If is received +, increase frontlight
        {
            b++;
            change = true;
        }
        if (c == '-' && b > 0) // If is received -, decrease frontlight
        {
            b--;
            change = true;
        }

        if (c == 's')
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int i = 0; i < 64; ++i)
                {
                    display.frontlight.setBrightness(i);
                    delay(30);
                }

                for (int i = 63; i >= 0; --i)
                {
                    display.frontlight.setBrightness(i);
                    delay(30);
                }
            }

            change = true;
        }

        if (change) // If frontlight valuse has changed, update the intensity and show current value of frontlight
        {
            display.frontlight.setBrightness(b);
            Serial.print("Frontlight:");
            Serial.print(b, DEC);
            Serial.println("/63");
        }
    }
}

