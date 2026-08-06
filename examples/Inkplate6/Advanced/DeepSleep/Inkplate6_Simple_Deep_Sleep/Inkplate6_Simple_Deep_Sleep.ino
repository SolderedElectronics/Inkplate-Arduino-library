/**
 **************************************************
 * @file        Inkplate6_Simple_Deep_Sleep.ino
 * @brief       Simple deep sleep slideshow example for Soldered Inkplate 6.
 *
 * @details     Demonstrates low-power operation on Inkplate 6 using ESP32 deep
 *              sleep. On each wake-up (timer-based), the board redraws the screen
 *              with the next image in a small slideshow, performs a full display
 *              refresh, and then returns to deep sleep.
 *
 *              Deep sleep restarts the program from the beginning on every
 *              wake-up. RAM contents are lost during deep sleep, so standard
 *              partial updates cannot be used. This example runs in 3-bit
 *              (grayscale) mode, which requires full refresh updates. Expected
 *              output is a new image on the display every 20 seconds, looping
 *              through all provided images.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable (or battery for low-power testing)
 * - Extra:      Converted image header files (picture1.h, picture2.h, picture3.h)
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Convert 3 images using the Soldered Image Converter and include them as
 *    picture1.h, picture2.h, and picture3.h.
 * 3) Upload the sketch to Inkplate 6.
 * 4) The board shows an image, goes to deep sleep, and wakes up every
 *    20 seconds.
 * 5) After each wake-up, the next image is shown (loops through 3 images).
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Image converter tool:
 *              https://tools.soldered.com/tools/image-converter/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"      //Include Inkplate library to the sketch
#include "driver/rtc_io.h" //ESP32 library used for deep sleep and RTC wake up pins
#include "picture1.h" //Include .h files of 3 pictures. All three pictures were converted using Inkplate Image Converter
#include "picture2.h"
#include "picture3.h"
const uint8_t *pictures[] = {pic1, pic2, pic3}; // This array of pinters holds address of every picture in the memory,
                                                // so we can easly select it by selecting index in array

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20      // How long ESP32 will be in deep sleep (in seconds)
RTC_DATA_ATTR int slide = 0;

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 3 Bit mode (gray)

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.image.draw(
        pictures[slide], 0, 0, 800,
        600); // Display selected picture at location X=0, Y=0. All three pictures have resolution of 800x600 pixels
    display.display(); // Refresh the screen with new picture
    slide++; // Update counter for pictures. With this variable, we choose what picture is going to be displayed on
             // screen
    if (slide > 2)
        slide = 0; // We do not have more than 3 images, so roll back to zero

    rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    esp_deep_sleep_start();                                        // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
    // Nothing! If you use deep sleep, whole program should be in setup() because each time the board restarts, not in a
    // loop()! loop() must be empty!
}
