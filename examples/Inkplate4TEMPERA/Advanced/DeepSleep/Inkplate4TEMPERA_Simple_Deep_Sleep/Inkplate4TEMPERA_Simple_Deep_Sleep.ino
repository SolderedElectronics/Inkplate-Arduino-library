/*
   Inkplate4TEMPERA_Simple_Deep_Sleep example for Soldered Inkplate 4 TEMPERA
   For this example you will need USB-C cable and Inkplate 4 TEMPERA.
   Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use low power functionality of Inkplate board.
   In deep sleep, whole board will consume about 25uA from battery.
   Inkplate will wake every 20 seconds change content on screen.

   NOTE: Because we are using deep sleep, everytime the board wakes up, it starts program from begining.
   Also, whole content from RAM gets erased, so you CAN NOT use partial updates.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   16 July 2023 by Soldered

   In order to convert your images into a format compatible with Inkplate
   use the Soldered Image Converter available at:
   https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Soldered Inkplate4TEMPERA in the boards menu."
#endif

#include "Inkplate.h"      //Include Inkplate library to the sketch
#include "driver/rtc_io.h" //ESP32 library used for deep sleep and RTC wake up pins
#include "picture1.h" //Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter
#include "picture2.h"
#include "picture3.h"
const uint8_t *pictures[] = {picture1, picture2,
                             picture3}; // This array of pinters holds address of every picture in the memory,
                                        // so we can easly select it by selecting index in array

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20      // How long ESP32 will be in deep sleep (in seconds)
RTC_DATA_ATTR int slide = 0;

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode (gray)

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    // Turn frontlight on
    display.frontlight(1);
    display.setFrontlight(50);

    display.clearDisplay(); // Clear frame buffer of display
    display.drawImage(
        pictures[slide], 0, 0, 600, 600); // Display selected picture at location X=0, Y=0. All three pictures have resolution of 600x600 pixels
    display.display(); // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
             // screen
    if (slide > 2)
        slide = 0; // We do not have more than 3 images, so roll back to zero

    // Turn off touchscreen and frontlight to save energy duiring deep sleep
    display.tsShutdown();  // Turn off the display touchscreen
    display.frontlight(0); // Turn off the frontlight

    rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    esp_deep_sleep_start();                                        // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}
