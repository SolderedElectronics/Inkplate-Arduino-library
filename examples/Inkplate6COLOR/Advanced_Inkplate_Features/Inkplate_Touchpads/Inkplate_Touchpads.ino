/*
   Inkplate_Touchpads example for Soldered Inkplate 6COLOR
   For this example you will need only a micro USB cable and an Inkplate 6COLOR.
   Select "Inkplate 6COLOR(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6COLOR(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use built-in touchpads (on PCB marked with numbers 1, 2 and 3).
   These are basically touch sensitive switches. You can read state each of these with function readTouchpad()
   and the argument you need to pass to this function is PAD1 if you want to read the state of touchpad marked
   as "1" on PCB, PAD2 for second touchpad, PAD3 for third. You can also use numbers as arguments.
   For that you need to pass number 0 for touchpad that is marked as 1 on PCB, 1 for second touchpad and 2 for third.
   Function will return 1 if selected touchpad is pressed, zero if not.

   In this example, if you touch first pad, ti will decrese number showed on screen, if you touch thirs touch pad,
   it will increase the number, if you touch second touchpad, it will reset number to zero.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

int number = 0; // Variable that stores our number
int n = 0;      // Variable that keeps track on how many times display is partially updated
void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.print("Open Serial to monitor number change");
    display.display();
}

void loop()
{
    if (display.readTouchpad(PAD1))
    { // Check if first pad has been touched. If it is, decrement the number and refresh the screen.
        number--;
        Serial.println(number);
    }

    if (display.readTouchpad(PAD2))
    { // If you touched second touchpad, set number to zero and refresh screen by calling our displayNumber() function
        number = 0;
        Serial.println(number);
    }

    if (display.readTouchpad(PAD3))
    { // If you touched third touchpad, incerement the number and refresh the screen.
        number++;
        Serial.println(number);
    }
    delay(200); // Wait a little bit between readings.
}