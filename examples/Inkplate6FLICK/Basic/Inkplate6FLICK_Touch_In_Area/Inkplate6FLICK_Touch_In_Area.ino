/*
   Inkplate6FLICK_Touch_In_Area example for Soldered Inkplate 6FLICK
   For this example you will need only USB cable and Inkplate 6FLICK
   Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows you how to use Inkplate 6 FLICK touchscreen.
   Once the code is uploaded, try to touch the rectangle on the screen :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   18 March 2024 by Soldered
*/

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
    display.fillRect(x_position, y_position, 100, 50, BLACK);
    display.display();
}

void loop()
{
    //Touch in area checks if touch ocured in given coordinates
    if(display.touchInArea(x_position, y_position, 100, 50))
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
