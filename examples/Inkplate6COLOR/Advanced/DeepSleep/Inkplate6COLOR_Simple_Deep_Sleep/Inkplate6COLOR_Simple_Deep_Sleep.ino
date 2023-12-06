/*
   Inkplate6COLOR_Simple_Deep_Sleep  example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use low power functionality of Inkplate board.
   In deep sleep, whole board will consume about 25uA from battery.
   Inkplate will wake every 20 seconds change content on screen.

   NOTE: Because we are using deep sleep, everytime the board wakes up, it starts program from begining.
   Also, whole content from RAM gets erased, so you CAN NOT use partial updates.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
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
#define TIME_TO_SLEEP  120      // How long ESP32 will be in deep sleep (in seconds)
RTC_DATA_ATTR int slide = 0;

Inkplate display; // Create an object on Inkplate library and also set library into 3 Bit mode (gray)

void setup()
{
    pinMode(25, OUTPUT);
    
    Serial.begin(115200);
    Serial.println("about to enter begin...");
    

    delay(1000);

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)

    delay(1000);


    // Uncomment this line if your Inkplate is older than Aug 2021 as older Inkplates have ESP32 wrover-e chips
    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)

    // Activate wake-up timer -- wake up after 20s here
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    blinkLED();
    blinkLED();
    display.sleepColorPanel();
    delay(1000);

    blinkLED();
    blinkLED();
    blinkLED();
    // Put ESP32 into deep sleep. Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}

void blinkLED()
{
    digitalWrite(25,HIGH);
    delay(100);
    digitalWrite(25,LOW);
    delay(100);

    delay(100);

    
}