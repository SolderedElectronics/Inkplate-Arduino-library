/**
 **************************************************
 * @file        Inkplate6FLICK_Simple_Deep_Sleep.ino
 * @brief       Simple deep sleep slideshow demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates low-power operation using ESP32 deep sleep on
 *              Inkplate 6FLICK. The device wakes up at a fixed interval
 *              (20 seconds by default), updates the e-paper screen by drawing
 *              the next image in a small slideshow, then returns to deep sleep.
 *
 *              With deep sleep, the program restarts from setup() after each
 *              wake-up and RAM contents are erased, so partial updates cannot be
 *              used - this example uses 3-bit (grayscale) mode for image
 *              rendering. On older Inkplate hardware you may need to isolate
 *              GPIO12 to reduce sleep current. Expected output is a repeating
 *              3-image slideshow on the display, updating every 20 seconds
 *              (TIME_TO_SLEEP), with low power consumption between updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      Converted image headers (picture1.h / picture2.h / picture3.h)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Convert 3 images to Inkplate-compatible .h files (picture1/2/3.h).
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) Inkplate wakes periodically, displays the next image, and sleeps again.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"      //Include Inkplate library to the sketch
#include "driver/rtc_io.h" //ESP32 library used for deep sleep and RTC wake up pins
#include "picture1.h" //Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter
#include "picture2.h"
#include "picture3.h"
const uint8_t *pictures[] = {picture1, picture2,
                             picture3}; // This array of pinters holds address of every picture in the memory,
                                        // so we can easly select it by selecting index in array

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20      // How long ESP32 will be in deep sleep (in seconds)
RTC_DATA_ATTR int slide = 0;

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode (gray)

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.image.draw(pictures[slide], 0, 0, 1100, 825); // Display selected picture at location X=0, Y=0. All three pictures have resolution of 1100x825 pixels
    display.display(); // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
             // screen
    if (slide > 2)
        slide = 0; // We do not have more than 3 images, so roll back to zero

    // Uncomment this line if your Inkplate is older than Aug 2021 as older Inkplates have ESP32 wrover-e chips
    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    esp_deep_sleep_start();                                        // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}
