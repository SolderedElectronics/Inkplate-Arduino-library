/*
   Inkplate6COLOR_Read_Touchpads example for Soldered Inkplate6COLOR
   For this example you will need only a micro USB cable and Inkplate6COLOR.
   Select "e-radionica Inkplate6COLOR" or "Soldered Inkplate6COLOR" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6COLOR" or "Soldered Inkplate6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use the built-in touchpads (on PCB marked with numbers 1, 2 and 3).
   These are basically touch sensitive switches. You can read state each of these with function readTouchpad()
   and the argument you need to pass to this function is PAD1 if you want to read the state of touchpad marked
   as "1" on PCB, PAD2 for second touchpad, PAD3 for third. You can also use numbers as arguments.
   For that you need to pass number 0 for touchpad that is marked as 1 on the PCB, 1 for the second touchpad and 2 for
   the third. The function readTouchpad() will return 1 if selected touchpad is pressed, zero if not.

   In this example, if you touch the first pad, it will decrese the number printed on serial, if you touch the third
   touch pad, it will increase the number, if you touch second touchpad, it will reset the number to zero.

   NOTE: You can not use touch pads when an enclosure is fitted on the Inkplate - they are not that sensitive!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   6 June 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create an object on Inkplate library

int number = 0; // Variable that stores our number
int n = 0;      // Variable that keeps track on how many times display is partially updated

void setup()
{
    Serial.begin(115200);   // Start serial to print the number
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Display text
    display.setCursor(100, 212);
    display.setTextColor(INKPLATE_RED);
    display.setTextSize(2);
    display.print("Open Serial monitor at 115200 baud!");
    display.display();
}

void loop()
{

    if (display.readTouchpad(PAD1))
    { // Check if first pad has been touched. If it is, decrement the number and print to serial
        number--;
        Serial.print("The number is: ");
        Serial.println(number);
    }

    if (display.readTouchpad(PAD2))
    { // If you touched second touchpad, set number to zero and print to serial
        number = 0;
        Serial.print("The number is: ");
        Serial.println(number);
    }

    if (display.readTouchpad(PAD3))
    { // If you touched third touchpad, incerement the number and print to serial
        number++;
        Serial.print("The number is: ");
        Serial.println(number);
    }
    delay(100); // Wait a little bit between readings.
}
