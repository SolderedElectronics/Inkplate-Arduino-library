/**
 **************************************************
 * @file        Inkplate4TEMPERA_Touchscreen_Draw.ino
 * @brief       Simple touchscreen drawing demo: reads touch coordinates and
 *              draws on the e-paper display using fast partial updates.
 *
 * @details     This example demonstrates basic touchscreen usage on Inkplate 4
 *              TEMPERA by reading raw touch coordinates and rendering graphics
 *              directly to the display. After initializing the touchscreen, the
 *              sketch continuously checks for touch availability, retrieves up
 *              to two simultaneous touch points, and draws either:
 *              - A line segment from the previous touch point to the new point
 *                (DRAW_LINE), creating a "scribble" effect, or
 *              - A filled circle at the touch position (DRAW_CIRCLE).
 *
 *              The display runs in 1-bit black/white mode (INKPLATE_1BIT), where
 *              partial updates are supported, and keeps panel power enabled via
 *              display.partialUpdate(false, true) to make the interaction
 *              responsive at the cost of increased power consumption. Repeated
 *              partial updates accumulate ghosting artifacts, so for longer
 *              sessions add an occasional full refresh (display.display()) to
 *              clean the panel. The API can return up to 2 touch points, but this
 *              example uses only the first point (x[0], y[0]) for drawing.
 *
 *              Expected output: the text "Draw on the screen!" on startup, then
 *              drawing where you touch, updated quickly via partial refreshes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional, for init logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Select the drawing mode: #define DRAW_LINE (default) or #define
 *    DRAW_CIRCLE.
 * 3) Upload the sketch to Inkplate 4 TEMPERA.
 * 4) Touch and drag on the screen to draw.
 * 5) If DRAW_LINE is enabled, continuous strokes are drawn by connecting points;
 *    if DRAW_CIRCLE is enabled, tapping draws filled circles.
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

// Select to draw a line on screen or filled circle
#define DRAW_LINE
// #define DRAW_CIRCLE

#ifdef DRAW_LINE
uint16_t xOld, yOld;
#endif

// Create Inkplate object in monochrome mode
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    display.begin();
    display.setCursor(50,50);
    display.setTextSize(2);
    display.print("Draw on the screen!");

    display.display();

    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init OK!");
    }
    else
    {
        Serial.println("Touchscreen init failed!");
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
            // Do partial update with leaving the display ON
            // This makes the example run much more responsive
            display.partialUpdate(false, true);
        }
    }
}
