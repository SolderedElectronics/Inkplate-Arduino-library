/**
 **************************************************
 * @file        Inkplate2_Simple_Deep_Sleep.ino
 * @brief       Deep sleep demo for Inkplate 2: display a sequence of images,
 *              then sleep and wake periodically using the ESP32 timer.
 *
 * @details     This example demonstrates low-power operation on Inkplate 2 by
 *              using ESP32 deep sleep with a timer wakeup. On each boot/wakeup,
 *              the sketch draws one of three pre-converted 1-bit images to the
 *              e-paper display, performs a full refresh, updates an RTC-persisted
 *              index, then immediately enters deep sleep again.
 *
 *              Deep sleep resets the ESP32 on every wake event, so the program
 *              always starts from setup() - keep the logic there and leave
 *              loop() empty. The currently selected image index is stored in RTC
 *              memory (RTC_DATA_ATTR) so it survives deep sleep cycles and the
 *              images rotate across wakeups; RTC_DATA_ATTR variables are reset
 *              by power loss or a full reset/flash.
 *
 *              Display mode is 1-bit (BW) and this sketch uses a full refresh
 *              (display()) on each wake cycle; no partial updates are used. For
 *              lowest power, avoid leaving GPIOs in a state that increases
 *              leakage - the optional rtc_gpio_isolate() line can further reduce
 *              sleep current on some hardware revisions.
 *
 *              Expected output: one of three images on the display, changing on
 *              each timed wakeup, with very low consumption during deep sleep
 *              (board-dependent).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Battery (recommended for measuring low-power behavior),
 *               picture1.h / picture2.h / picture3.h with Inkplate-compatible
 *               monochrome bitmap data (generated with Soldered Image Converter)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Convert three monochrome images using the Soldered Image Converter and
 *    include them as picture1.h, picture2.h, and picture3.h.
 * 3) Set the timer period TIME_TO_SLEEP (seconds).
 * 4) Upload the sketch to Inkplate 2.
 * 5) Power the board (battery preferred). The display updates once, then the
 *    ESP32 enters deep sleep.
 * 6) Every TIME_TO_SLEEP seconds the board wakes, shows the next image, and
 *    returns to deep sleep.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-29
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"      //Include Inkplate library to the sketch
#include "driver/rtc_io.h" //ESP32 library used for deep sleep and RTC wake up pins
#include "picture1.h" //Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter (https://tools.soldered.com/tools/image-converter/)
#include "picture2.h"
#include "picture3.h"

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20      // How long ESP32 will be in deep sleep (in seconds)

const uint8_t *pictures[] = {pic1, pic2, pic3}; // This array holds address of every picture in the memory, so we can
                                                // easly select it by selecting index in array
const uint8_t w[] = {159, 148, 186};            // Widths of each picture. Heights are the same for oll three pictures.
RTC_DATA_ATTR int slide = 0;

Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.image.draw(pictures[slide], 106 - w[slide] / 2, 0, w[slide],
                      104); // Display selected picture at location X=0, Y=0. All three pictures have different widths,
                            // but the same heights
    display.display();      // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
    // screen
    if (slide > 2)
        slide = 0; // We do not have more than 3 images, so roll back to zero

    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here

     
    esp_deep_sleep_start();          // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}
