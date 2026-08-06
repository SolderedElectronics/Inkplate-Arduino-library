/**
 **************************************************
 * @file        Inkplate4TEMPERA_Touch_In_Area.ino
 * @brief       Detect touchscreen taps inside a defined rectangle and move it
 *              across the e-paper display using partial updates.
 *
 * @details     This example initializes the Inkplate 4 TEMPERA in 1-bit (BW)
 *              mode and enables the touchscreen. A black rectangle is drawn on
 *              the display and the sketch continuously checks whether a touch
 *              occurred inside that rectangle using touchInArea().
 *
 *              When the rectangle is tapped, its position is shifted by a fixed
 *              offset and the new position is rendered using a partial update
 *              to reduce refresh time and flashing. When the rectangle reaches
 *              the bottom area of the screen, the position is reset and a full
 *              refresh is performed.
 *
 *              Partial updates are supported in BW mode. For best visual
 *              quality, perform a full refresh periodically (this example does a
 *              full refresh when resetting the rectangle). Touch detection is
 *              limited to the rectangle area via touchInArea(); touches
 *              elsewhere are ignored.
 *
 *              Expected output: intro text, then a black rectangle that moves
 *              when tapped, plus "Touchscreen init ok" (or "Touchscreen init
 *              fail") on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional, for init status messages)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) After the intro text, a black rectangle appears on the display.
 * 4) Tap inside the rectangle to move it diagonally.
 * 5) Keep tapping until it reaches the bottom; it will reset and do a full
 *    refresh.
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

// Coordinates where to draw the rectangle
int x_position = 50;
int y_position = 50;

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200); // Init Serial for debugging

    display.begin(); // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    
    // Print Info text on Inkplate
    display.setCursor(90, 280); 
    display.setTextSize(3);
    display.print("Touch button example.");
    display.setCursor(60, 350);
    display.print("Touch the black button.");
    display.display(); // Show the text
    delay(3000); // Wait a bit so the user can see the text

    // Clear the display buffer
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
    display.fillRect(x_position, y_position, 120, 70, BLACK);
    display.display();
}

void loop()
{
    //Touch in area checks if touch ocured in given coordinates
    if(display.touchscreen.touchInArea(x_position, y_position, 120, 70))
    {
        x_position += 100;
        y_position += 100;

        if(y_position < 540)
        {
            display.clearDisplay();
            display.fillRect(x_position, y_position, 120, 70, BLACK);

            display.partialUpdate();
            delay(100);
        }
        else//Reseting rectangle position and doing full refresh
        {
            x_position = 50;
            y_position = 50;
            
            display.clearDisplay();
            display.fillRect(x_position, y_position, 120, 70, BLACK);
            display.display();
        }
    }

}
