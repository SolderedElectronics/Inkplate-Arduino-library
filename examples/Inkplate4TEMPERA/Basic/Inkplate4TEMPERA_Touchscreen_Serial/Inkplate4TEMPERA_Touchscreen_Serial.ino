/**
 **************************************************
 * @file        Inkplate4TEMPERA_Touchscreen_Serial.ino
 * @brief       Demonstrates basic touchscreen input on Inkplate 4 TEMPERA by
 *              reporting touch events over the Serial Monitor.
 *
 * @details     This example shows how to initialize and use the capacitive
 *              touchscreen on Inkplate 4 TEMPERA and read touch events in
 *              real time. After uploading the sketch, the touchscreen is
 *              powered and configured, and all detected touch interactions
 *              (press, release, coordinates, and gestures if supported by the
 *              controller configuration) are printed to the Serial Monitor.
 *
 *              The example is intended as a debugging and learning tool for
 *              understanding raw touchscreen behavior before integrating touch
 *              input into a graphical user interface. No drawing or visual
 *              feedback is shown on the e-paper display itself, and no deep
 *              sleep is used - the MCU runs continuously. The touchscreen must
 *              be successfully initialized, otherwise no touch events will be
 *              reported. For low-power designs, combine touchscreen interrupts
 *              with the deep sleep wake-up examples instead of continuous
 *              polling.
 *
 *              Expected output: Serial Monitor messages indicating touchscreen
 *              events, including touch detection and coordinate information.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) Open the Serial Monitor and set the baud rate to 115200.
 * 4) Touch the screen with a finger or stylus.
 * 5) Observe touch event data printed in the Serial Monitor.
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

// Include Inkplate library
#include "Inkplate.h"

// Create Inkplate object in monochrome mode
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Init Serial so we can see the outputs
    Serial.begin(115200);

    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear the image buffer
    display.display();      // Show the cleared screen

    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init OK!");
    }
    else
    {
        Serial.println("Touchscreen init fail!");
        while (true)
            ;
    }

    // Show where the touchscreen 0, 0 position is to the user
    display.fillTriangle(13, 13, 23, 43, 43, 23, BLACK);
    display.setTextSize(3);
    display.setCursor(65, 65);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Periodically check if there is any touch detected
    if (display.touchscreen.available())
    {
        // Variables for storing the touchscreen data
        uint8_t n;
        uint16_t x[2], y[2];

        // See how many fingers are detected (max 2) and copy x and y position of each finger on touchscreen
        n = display.touchscreen.getData(x, y);
        if (n != 0)
        {
            // Print number of fingers to serial monitor, along with their coordinates
            Serial.printf("%d finger%c ", n, n > 1 ? 's' : NULL);
            for (int i = 0; i < n; i++)
                Serial.printf("X=%d Y=%d ", x[i], y[i]);
            Serial.println(); // Newline
        }
        else
        {
            // If touchscreen driver returns us a zero, it means that there are no more touch events pressent on the
            // screen
            x[0] = 0;
            x[1] = 0;
            y[0] = 0;
            y[1] = 0;
            Serial.println("Release");
        }
    }
}
