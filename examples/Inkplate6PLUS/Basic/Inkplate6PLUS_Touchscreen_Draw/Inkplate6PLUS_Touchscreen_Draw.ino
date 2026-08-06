/**
 **************************************************
 * @file        Inkplate6PLUS_Touchscreen_Draw.ino
 * @brief       Touchscreen drawing example for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to use the Inkplate 6PLUS touchscreen to draw
 *              directly on the e-paper display. The sketch reads touch
 *              coordinates and renders graphics at the touched location.
 *
 *              Two drawing modes are available:
 *              - Line drawing: draws a continuous line following the finger.
 *              - Circle drawing: draws filled circles at touch points.
 *
 *              The display is refreshed using partial updates to allow faster
 *              drawing and improved responsiveness. The touchscreen supports up
 *              to two simultaneous touch points, but this example uses only the
 *              first detected touch. Touchscreen coordinates are automatically
 *              adjusted based on display rotation.
 *
 *              Expected behaviour: touch events are detected by the touchscreen
 *              controller, graphics are drawn at the detected coordinates, and
 *              partial updates refresh only the modified area for faster
 *              interaction.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6PLUS"
 *    from Tools -> Board.
 * 2) Select the drawing mode by enabling one of the macros: #define DRAW_LINE
 *    or #define DRAW_CIRCLE.
 * 3) Upload the sketch to Inkplate 6PLUS.
 * 4) Touch the screen to draw.
 * 5) In line mode a continuous line follows your finger; in circle mode filled
 *    circles are drawn where the screen is touched.
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

// Select to draw a line on screen or filled circle
#define DRAW_LINE
// #define DRAW_CIRCLE

Inkplate display(INKPLATE_1BIT);

#ifdef DRAW_LINE
uint16_t xOld, yOld;
#endif

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();
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
#ifdef DRAW_LINE // Draw line from old point to new
            display.drawLine(xOld, yOld, x[0], y[0], BLACK);

            // Save coordinates to use as old next time
            xOld = x[0];
            yOld = y[0];
#endif

#ifdef DRAW_CIRCLE // Draw circle on touch event coordinates
            display.fillCircle(x[0], y[0], 20, BLACK);
#endif
            display.partialUpdate();
        }
    }
}
