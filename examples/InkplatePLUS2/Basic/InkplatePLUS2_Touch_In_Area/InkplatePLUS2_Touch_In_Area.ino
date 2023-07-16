/*
   InkplatePLUS2_Touch_In_Area.ino example for Soldered Inkplate PLUS2
   For this example you will need only a USB-C cable and Inkplate PLUS2.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows you how to use Inkplate 6Plus touchscreen.
   Once the code is uploaded, try to touch the rectangle on the screen :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   12 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
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
    if (display.tsInit(true))
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
    if(display.touchInArea(x_position, y_position, 120, 70))
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
