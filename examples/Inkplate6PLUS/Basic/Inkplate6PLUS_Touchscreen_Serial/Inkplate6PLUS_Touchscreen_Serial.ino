/**
 **************************************************
 * @file        Inkplate6PLUS_Touchscreen_Serial.ino
 * @brief       Touchscreen event monitoring via Serial for Inkplate 6PLUS.
 *
 * @details     This example demonstrates how to read touch events from the
 *              Inkplate 6PLUS touchscreen and print the detected touch
 *              coordinates to the Serial Monitor.
 *
 *              The touchscreen supports multi-touch (up to two simultaneous
 *              fingers). When a touch occurs, the number of detected fingers
 *              and their coordinates are printed to the serial output.
 *              When all fingers are released, a "Release" message is printed.
 *
 *              Coordinates are automatically adjusted if the display rotation is
 *              changed; this example sets the display rotation to orientation
 *              "2". Expected behaviour: when a finger touches the screen the
 *              (X,Y) coordinates are printed, if two fingers touch
 *              simultaneously both coordinates are printed, and when all fingers
 *              are lifted "Release" is printed.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6PLUS"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Touch the display with one or two fingers.
 * 5) The number of detected fingers and their coordinates appear in the Serial
 *    Monitor.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-27
 * @license     GNU GPL V3
 **************************************************/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();
    display.clearDisplay();
    display.display();
    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        while (true)
            ;
    }

    // NOTE!!!
    // Touchscreen cooridinates are automatically swapped and adjusted when screen is rotated
    display.setRotation(2);
    display.fillTriangle(10, 10, 20, 40, 40, 20, BLACK);
    display.setTextSize(3);
    display.setCursor(60, 60);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Check if there is any touch detected
    if (display.touchscreen.available())
    {
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
            Serial.println();
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
