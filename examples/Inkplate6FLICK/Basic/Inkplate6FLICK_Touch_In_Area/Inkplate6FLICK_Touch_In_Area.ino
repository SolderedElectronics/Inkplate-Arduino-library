/**
 **************************************************
 * @file        Inkplate6FLICK_Touch_In_Area.ino
 * @brief       Touchscreen touch-in-area demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to use the Inkplate 6FLICK touchscreen by
 *              detecting touches inside a defined rectangular area. A black
 *              rectangle is drawn on the display; when the user touches inside
 *              the rectangle, it moves to a new position. Partial updates are
 *              used for fast redraws, with an occasional full refresh when the
 *              rectangle position resets.
 *
 *              Touch detection uses touchscreen.touchInArea(x, y, w, h), and the
 *              touchscreen is initialized and powered on with
 *              touchscreen.init(true). Expected output is the rectangle moving
 *              when touched inside its bounds, with the touchscreen init status
 *              printed in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) After initialization, a black rectangle appears on the screen.
 * 4) Touch inside the rectangle to move it diagonally across the display.
 * 5) When it reaches the lower area, the rectangle resets to the start and a
 *    full refresh is performed.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"

int x_position = 50;
int y_position = 50;

Inkplate display(INKPLATE_1BIT);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    display.begin();
    display.clearDisplay();
    display.setCursor(100, 300);
    display.setTextSize(3);
    display.print("Touch button example. Touch the black button.");
    display.display();
    delay(3000);
    display.clearDisplay();
    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        while (true);
    }

    //Draw initial rectangle
    display.fillRect(x_position, y_position, 100, 50, BLACK);
    display.display();
}

void loop()
{
    //Touch in area checks if touch ocured in given coordinates
    if(display.touchscreen.touchInArea(x_position, y_position, 100, 50))
    {
        x_position += 100;
        y_position += 100;

        if(y_position < 660)
        {
            display.clearDisplay();
            display.fillRect(x_position, y_position, 100, 50, BLACK);

            display.partialUpdate();
            delay(100);
        }
        else//Reseting rectangle position and doing full refresh
        {
            x_position = 50;
            y_position = 50;
            
            display.clearDisplay();
            display.fillRect(x_position, y_position, 100, 50, BLACK);
            display.display();
        }
    }

}
