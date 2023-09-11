/*
   InkplatePLUS2_Buzzer example for Soldered Inkplate PLUS2
   For this example you will need only a USB-C cable and Inkplate PLUS2.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to beep the built-in buzzer and set it's frequency.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 Sep 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Variable for the frequency currently being beeped
int frequency = 10;

void setup()
{
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)
    display.display();    // Put clear image on display

    display.buzzer.begin(); // Init the buzzer
}

void loop()
{
    // Beep the buzzer with a set length and frequency
    // Parameters are length in ms and the frequency
    // The frequency is a scaled value from 0 to 100, 0 being the lowest, 100 being the highest
    display.buzzer.beep(500, frequency);
    delay(200); // Wait a bit

    // You can also control the timing as such:
    display.buzzer.beepOn(frequency); // Turn on the buzzer indefinitely
    delay(100); // Wait a bit
    display.buzzer.beepOff(); // Turn off the buzzer
    
    delay(950); // Wait a bit longer
    
    // Increment the frequency so the pitch increases from 10 to 100
    frequency += 10;
    if(frequency == 100) frequency = 10;
}