/**
 **************************************************
 * @file        Inkplate5_Wake_Up_Button.ino
 * @brief       Wake-up button and timer deep sleep example for Soldered Inkplate 5.
 *
 * @details     Demonstrates how to wake the ESP32 from deep sleep on Inkplate 5
 *              using an external interrupt (WakeUp button) and a fallback timer.
 *              The example stores a boot counter in RTC memory, shows the boot
 *              count on the e-paper display, and prints the wake-up reason
 *              (button press vs. timer wake-up).
 *
 *              Deep sleep restarts the program from the beginning on every
 *              wake-up. bootCount is stored in RTC memory (RTC_DATA_ATTR) so it
 *              persists across deep sleep, and the WakeUp button uses EXT0
 *              wake-up on GPIO36. Expected output is an incrementing boot count
 *              on the display together with the wake-up reason (WakeUp button
 *              or timer).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      None
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5.
 * 3) After displaying boot info, the board enters deep sleep.
 * 4) Wake the board by pressing the WakeUp button, or wait 30 seconds for timer
 *    wake-up.
 * 5) On each wake, the display updates with the new boot count and wake-up
 *    reason.
 *
 * @note        Quick start guide: Inkplate 5 has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-12-14
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include <Inkplate.h>

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000ULL

// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    ++bootCount;

    // Our function declared below
    displayInfo();

    // Go to sleep for TIME_TO_SLEEP seconds
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    // Enable wakeup from deep sleep on gpio 36 (wake button)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you use deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Function that will write number of boots and boot reason to screen
void displayInfo()
{
    // First, lets delete everything from frame buffer
    display.clearDisplay();

    // Set text cursor and size
    display.setCursor(30, 40);
    display.setTextSize(3);

    display.print(F("Boot count: "));
    display.println(bootCount, DEC); // Print the number

    // Set next line cursor position
    display.setCursor(30, 100);

    // Display wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        display.println("Wakeup caused by WakeUp button");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        display.println("Wakeup caused by timer");
        break;
    default:
        display.println("Wakeup was not caused by deep sleep");
        break;
    }

    // Show everything on the screen
    display.display();
}
