/**
 **************************************************
 * @file        Inkplate6COLOR_Simple_Deep_Sleep.ino
 * @brief       Demonstrates timed deep sleep with rotating full-screen images
 *              on Inkplate 6COLOR.
 *
 * @details     This example shows a simple low-power slideshow workflow on
 *              Inkplate 6COLOR. On each boot, the sketch initializes the
 *              display, draws one full-screen image from an array of converted
 *              image assets, performs a display refresh, updates the slide
 *              index stored in RTC memory, and then enters ESP32 deep sleep.
 *
 *              A timer wake-up is used to restart the board after a fixed
 *              delay, allowing the next image to be shown automatically. The
 *              selected slide index is preserved across deep sleep cycles using
 *              RTC_DATA_ATTR, while normal RAM contents are lost. Because deep
 *              sleep resets the ESP32 and framebuffer contents do not survive
 *              sleep, this workflow uses full refreshes only, and all runtime
 *              logic must be placed in setup() with loop() left empty.
 *
 *              This is a practical starting point for battery-powered signage,
 *              periodic status boards, and image rotation projects where the
 *              display only needs to update occasionally. Full-screen image
 *              refreshes on colour e-paper are slower and consume more energy
 *              than monochrome partial-update workflows on supported boards.
 *              Older boards with older ESP32 modules may benefit from GPIO12
 *              isolation to minimize sleep current, as noted in the example.
 *              Make sure converted image data fits available memory and uses the
 *              correct format/resolution for Inkplate 6COLOR.
 *
 *              Expected output: a rotating sequence of full-screen images, one
 *              image per wake-up, with the board sleeping between updates and
 *              waking automatically using the internal timer.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      battery optional, image header files (picture1.h, picture2.h,
 *               picture3.h) converted for Inkplate at 600x448 resolution
 * - Serial:     115200 baud (optional for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Convert compatible images using the Soldered Image Converter and include
 *    them as header files in the sketch project.
 * 3) Adjust TIME_TO_SLEEP in the sketch if a different sleep interval is
 *    needed.
 * 4) Upload the sketch to Inkplate 6COLOR.
 * 5) On boot, the sketch displays one image full screen and refreshes the panel,
 *    then increments the slide index stored in RTC memory.
 * 6) The ESP32 enters deep sleep for the configured interval.
 * 7) After wake-up, the ESP32 restarts from setup(), displays the next image,
 *    and repeats the cycle.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/

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

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  30      // How long ESP32 will be in deep sleep (in seconds)
RTC_DATA_ATTR int slide = 0;

Inkplate display; // Create an object on Inkplate library and also set library into 3 Bit mode (gray)

void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.image.draw(
        pictures[slide], 0, 0, 600,
        448); // Display selected picture at location X=0, Y=0. All three pictures have resolution of 600x448 pixels
    display.display(); // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
             // screen
    if (slide > 2)
        slide = 0; // We do not have more than 3 images, so roll back to zero

    // Uncomment this line if your Inkplate is older than Aug 2021 as older Inkplates have ESP32 wrover-e chips
    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)

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