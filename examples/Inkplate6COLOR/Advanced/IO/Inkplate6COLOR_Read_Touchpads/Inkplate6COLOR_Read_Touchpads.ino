/**
 **************************************************
 * @file        Inkplate6COLOR_Read_Touchpads.ino
 * @brief       Reads the built-in capacitive touchpads on Inkplate 6COLOR and
 *              reports their state via Serial.
 *
 * @details     This example demonstrates how to use the three capacitive
 *              touchpads integrated on the Inkplate 6COLOR PCB. These pads,
 *              labeled 1, 2, and 3 on the board, function as simple
 *              touch-sensitive input buttons.
 *
 *              The sketch continuously checks the state of each touchpad using
 *              the display.touchpad.read() function, which returns 1 when a pad
 *              is touched and 0 when it is not. When a pad is touched, a message
 *              is printed to the Serial Monitor indicating which pad was
 *              activated. A short delay between reads prevents excessive Serial
 *              output.
 *
 *              This example is useful as a starting point for implementing
 *              touch-based user interfaces such as menu navigation, simple
 *              controls, or interactive displays. The display is not used here
 *              (no screen updates occur). Capacitive touchpads can be affected
 *              by environmental conditions such as humidity, grounding, or thick
 *              enclosures placed over the PCB.
 *
 *              Expected output: Serial Monitor messages such as "Pad 1
 *              pressed!", "Pad 2 pressed!" or "Pad 3 pressed!" when the
 *              corresponding touchpad is activated.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6COLOR.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Touch the capacitive pads labeled 1, 2, or 3 on the Inkplate PCB.
 * 5) The Serial Monitor prints which pad has been touched.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/


#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display; // Create an object on Inkplate library

void setup()
{
    Serial.begin(115200);               // Init Serial documentation
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
}

void loop()
{
    if (display.touchpad.read(1))
    { 
        Serial.println("Pad 1 pressed!");
    }

    if (display.touchpad.read(2))
    { 
        Serial.println("Pad 2 pressed!");
    }

    if (display.touchpad.read(3))
    { 
        Serial.println("Pad 3 pressed!");
    }
    delay(100); // Wait a little bit between readings.
}
