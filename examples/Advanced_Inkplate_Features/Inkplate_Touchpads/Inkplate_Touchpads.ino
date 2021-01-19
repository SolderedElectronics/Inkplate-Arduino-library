/*
   Inkplate_Touchpads example for e-radionica.com Inkplate 6
   For this example you will need only a micro USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use built-in touchpads (on PCB marked with numbers 1, 2 and 3).
   These are basically touch sensitive switches. You can read state each of these with function readTouchpad()
   and the argument you need to pass to this function is PAD1 if you want to read the state of touchpad marked
   as "1" on PCB, PAD2 for second touchpad, PAD3 for third. You can also use numbers as arguments.
   For that you need to pass number 0 for touchpad that is marked as 1 on PCB, 1 for second touchpad and 2 for third.
   Function will return 1 if selected touchpad is pressed, zero if not.

   In this example, if you touch first pad, ti will decrese number showed on screen, if you touch thirs touch pad,
   it will increase the number, if you touch second touchpad, it will reset number to zero.

   NOTE: You can not use touch pads when enclosure is fitted on the Inkplate - they are not that sensitive!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

int number = 0; // Variable that stores our number
int n = 0;      // Variable that keeps track on how many times display is partially updated
void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextSize(5);             // Set text scaling to five (text will be five times bigger)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
    displayNumber();                    // Call our function to display nubmer on screen
}

void loop()
{
    if (display.readTouchpad(PAD1))
    { // Check if first pad has been touched. If it is, decrement the number and refresh the screen.
        number--;
        displayNumber();
    }

    if (display.readTouchpad(PAD2))
    { // If you touched second touchpad, set number to zero and refresh screen by calling our displayNumber() function
        number = 0;
        displayNumber();
    }

    if (display.readTouchpad(PAD3))
    { // If you touched third touchpad, incerement the number and refresh the screen.
        number++;
        displayNumber();
    }
    delay(100); // Wait a little bit between readings.
}

// Function that will write you number to screen
void displayNumber()
{
    display.clearDisplay();      // First, lets delete everything from frame buffer
    display.setCursor(385, 280); // Set print cursor at X=385, Y=280 (roughly in the middle of the screen)
    display.print(number, DEC);  // Print the number
    display.setCursor(255, 560); // Set new print position (right above first touchpad)
    display.print('-');          // Print minus sign
    display.setCursor(385, 560); // Set new print position (right above second touchpad)
    display.print('0');          // Print zero
    display.setCursor(520, 560); // Set new print position (right above third touchpad)
    display.print('+');          // Print plus sign
    if (n > 20)
    { // Chech if screen has been partially refreshed more than 20 times. If it is, do a full refresh. If is not, do a
      // partial refresh
        display.display();
        n = 0;
    }
    else
    {
        display.partialUpdate();
        n++;
    }
}
