/**
 **************************************************
 * @file        Inkplate6FLICK_Partial_Update_With_Deep_Sleep.ino
 * @brief       Partial e-paper update with deep sleep demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to combine partial e-paper screen updates with
 *              ESP32 deep sleep on Inkplate 6FLICK. Because partial updates rely
 *              on RAM-stored screen state, the example shows how to reconstruct
 *              the previous screen content after waking from deep sleep before
 *              applying a new partial refresh.
 *
 *              Partial update works only in 1-bit (black & white) mode. After
 *              deep sleep, the previous screen content must be recreated before
 *              calling partialUpdate(), periodic full refreshes are recommended
 *              to maintain image quality, and partial update should be avoided
 *              immediately after power-on. Expected output is two variables
 *              displayed on the e-paper screen whose values update after each
 *              deep sleep cycle, with reduced power consumption thanks to ESP32
 *              deep sleep.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) The device displays variables on screen.
 * 4) The ESP32 enters deep sleep for a defined interval (10 s by default).
 * 5) After wake-up, the screen is reconstructed and partially refreshed with
 *    updated values.
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
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Inkplate.h"      // Include Inkplate library to the sketch
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10      // Time how long ESP32 will be in deep sleep (in seconds). In this case 10 seconds.

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Counter variable (stored in RTC RAM that stores variable even if deep sleep is used)
// Variables that are changed after each partial update has to be stored in RTC RAM in order to recreate screen before
// deep sleep
RTC_DATA_ATTR int counter = 0;
RTC_DATA_ATTR float decimal = PI;

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    createScreen();  // Function that contains everything that has to be written on screen

    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) // Check if ESP32 is reseted by deep sleep or power up / user manual
                                                    // reset (or some other reason)
    {
        display.preloadScreen(); // If is woken up by deep sleep, recreate whole screen to be same as was before deep sleep
        counter++;            // Update variable / variables
        decimal *= 1.23;
        display.clearDisplay();      // Clear everything in buffer
        createScreen();              // Create new screen with new variables
        display.partialUpdate(true); // Partial update of screen. (Use this only in this
                                     // scenario, otherwise YOU CAN DAMAGE YOUR SCRREN)
    }
    else // If is not deep sleep reset, that must be some thing else, so use normal update procerude (full screen
         // update)
    {
        display.display();
    }
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *
                                  uS_TO_S_FACTOR); // Set EPS32 to be woken up in 10 seconds (in this case)

    // Uncomment this line if your Inkplate is older than Aug 2021 as older Inkplates have ESP32 wrover-e chips
    // rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)

    esp_deep_sleep_start(); // Put ESP32 into deep sleep (low power mode)
}

void loop()
{
    // Nothing should be here while using deep sleep!
}

void createScreen()
{
    display.setCursor(200, 300);      // Set text cursor @ X = 200, Y = 300
    display.setTextSize(3);           // Set font to be scaled up three times
    display.print("First variable:"); // Write first variable to buffer
    display.print(counter, DEC);
    display.setCursor(200, 340);       // Set text cursor @ X = 200, Y = 340
    display.print("Second variable:"); // Write second variable to buffer (use two decimals places)
    display.print(decimal, 2);
}
