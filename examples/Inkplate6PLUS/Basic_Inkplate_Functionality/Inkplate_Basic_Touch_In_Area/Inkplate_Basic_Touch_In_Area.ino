/*
   Basic_Touch_In_Area example for e-radionica Inkplate 6PLUS
   For this example you will need only USB cable and Inkplate 6PLUS
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows you how to use Inkplate 6PLUS touchscreen.
   Once the code is uploaded, try to touch the rectangle on the screen :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select Inkplate 6PLUS or Inkplate 6PLUS V2 in the boards menu."
#endif

#include "Inkplate.h"

int x_position = 50;
int y_position = 50;

Inkplate display(INKPLATE_1BIT);

// If your Inkplate doesn't have external (or second) MCP I/O expander, you should uncomment next line,
// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander

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