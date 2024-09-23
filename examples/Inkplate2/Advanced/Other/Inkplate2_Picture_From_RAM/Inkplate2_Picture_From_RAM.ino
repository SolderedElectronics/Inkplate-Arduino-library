/*
   Inkplate2_Picture_From_RAM example for Soldered Inkplate 2
   For this example you will need USB cable and Inkplate 2.
   Select "Soldered Inkplate2" from Tools -> Board menu.
   Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can show picture loaded in RAM.
   Inkplate will change content on the screen every 10 seconds and go
   into deep sleep mode after showing three images.

   In order to convert your images into a format compatible with Inkplate
   use the Soldered Image Converter available at:
   https://github.com/SolderedElectronics/Soldered-Image-Converter/releases

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   24 November 2022 by soldered.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "picture1.h" // Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter and have resolution of 212x104 picels
#include "picture2.h"
#include "picture3.h" // This are headers in which are saved pictures that needs to be stored in RAM for showing.
                      /* 
                        In order to convert your images into a format compatible with Inkplate
                        use the Soldered Image Converter available at:
                        https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
                      */


Inkplate display; // Create display object

void setup()
{
    Serial.begin(115200);
    Serial.print("Sketch begun!");

    
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.drawImage(picture1, 0, 0, 212,
                      104); // Display picture from RAM  at location X=0, Y=0. It is also needed to specify width and
                            // height of picture (212x104 in this case).
    display.display();      // Refresh the screen with new picture
    delay(10000);           // Wait a bit (in this case 10 seconds)

    display.clearDisplay();                      // Clear frame buffer of display
    display.drawImage(picture2, 0, 0, 212, 104); // Display selected picture at location X=0, Y=0.
    display.display();                           // Refresh the screen with new picture
    delay(10000);                                // Wait a bit (in this case 10 seconds)

    display.clearDisplay();                      // Clear frame buffer of display
    display.drawImage(picture3, 0, 0, 212, 104); // Display selected picture at location X=0, Y=0.
    display.display();                           // Refresh the screen with new picture

    // Put ESP32 into deep sleep. Program stops here
    esp_deep_sleep_start();
}


void loop()
{
    // Nothing! 
}
