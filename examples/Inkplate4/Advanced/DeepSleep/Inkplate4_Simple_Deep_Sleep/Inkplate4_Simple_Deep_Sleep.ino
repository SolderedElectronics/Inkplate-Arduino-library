/*
   Inkplate4_Simple_Deep_Sleep example for Soldered Inkplate 4
   For this example you will need a USB-C cable and Inkplate 4.
   Select "Soldered Inkplate4" from Tools -> Board menu.
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use low power functionality of Inkplate board.
   In deep sleep, whole board will consume about 25uA from battery.
   Inkplate will wake every 20 seconds change content on screen.

   NOTE: Because we are using deep sleep, everytime the board wakes up, it starts program from begining.
   Also, the whole content from RAM gets erased. You can store data that don't wanna lose in the RTC 
   memory by adding RTC_DATA_ATTR before the variable name like the variable "slide" below.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   5 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h" 

// Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter
#include "picture1.h" 
#include "picture2.h"
#include "picture3.h"

// This array of pinters holds address of every picture in the memory, so we can easly select it by selecting index in array
const uint8_t *pictures[] = {pic1, pic2, pic3}; 

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20      // How long ESP32 will be in deep sleep (in seconds)

// The variable that will be "alive" during deep sleep, it won't lose its value
RTC_DATA_ATTR int slide = 0;

Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.drawImage(pictures[slide], 0, 0, E_INK_WIDTH,
                      E_INK_HEIGHT); // Display selected picture at location X=0, Y=0. All three pictures have
                                     // resolution of 400x300 pixels
    display.display();               // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
             // screen
    if (slide > 2)
        slide = 0; // We don't have more than 3 images, so roll back to zero

    // Put panel into deep sleep
    display.setPanelDeepSleep(true); 

    // Activate wake-up timer -- wake up after 20s here
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); 

    // Put ESP32 into deep sleep. Program stops here.
    esp_deep_sleep_start();                                        
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}
