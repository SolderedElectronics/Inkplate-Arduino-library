/*
   Inkplate_Clean_Screen sketch for e-radionica.com Inkplate 6
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   The burn-in effect is when you can see the previous picture on the screen faded in the background.
   It is not unusual or defect, it's normal behaviour of e-paper screen.
   Nevertheless, sometime the burn-in effect can be too strong and noticeable. In that case, you can
   run this sketch to clean your screen.

   The sketch didn't help? Refrigerate your Inkplate for 60 minutes!

   NOTE: burn-in effect is much stronger at the direct sun of when exposed to heat source.
   e-paper screens are not intented to be used at strong sun and in hot enviroments.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include <Inkplate.h>            //Add Inkplate library
Inkplate display(INKPLATE_1BIT); // Init the library

void setup()
{
    display.begin(); // Call this only once!

    // We will write "empty" screen to the Inkplate screen for 10 times in the row.
    // The result should be completly clean screen.
    for (int i = 0; i < 10; i++)
    {
        display.clean();   // First, clean the buffer so it's completly empty
        display.display(); // Now, send that empty buffer to the screen 10 times
        delay(1000);       // Just a random delay, not actually needed
    }
}

void loop()
{
}
