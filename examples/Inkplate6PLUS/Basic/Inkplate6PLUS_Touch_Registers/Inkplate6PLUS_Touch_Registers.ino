/**
 **************************************************
 * @file        Inkplate6PLUS_Touch_Registers.ino
 * @brief       Touchscreen register debug example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to read raw touchscreen controller register
 *              data from the Inkplate 6PLUS. When a touch event occurs, the
 *              sketch reads the raw register values from the touchscreen
 *              controller and prints them to the Serial Monitor for debugging
 *              or development purposes.
 *
 *              touchscreen.available() checks if a touch event occurred and
 *              touchscreen.getRawData() reads the raw controller register data.
 *              Touchscreen coordinates are automatically adjusted when the
 *              display rotation changes. This example is mainly intended for
 *              debugging and low-level touchscreen development. Expected output
 *              is raw touchscreen register values in the Serial Monitor when the
 *              screen is touched, plus a small marker and label indicating the
 *              (0,0) screen position on the display.
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
 * 3) Open the Serial Monitor at 115200 baud.
 * 4) Touch the screen to generate touchscreen events.
 * 5) Raw touchscreen register values are printed in the Serial Monitor.
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

uint8_t touchRegs[8];

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
        display.touchscreen.getRawData(touchRegs);
        for(int i = 0; i < 8; ++i)
        {
            Serial.print("Reg ");
            Serial.println(touchRegs[i], BIN);
        }

        Serial.println("---------------------------");
        Serial.println();
    }
    delay(1000);
}
