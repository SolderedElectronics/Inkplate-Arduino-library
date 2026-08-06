/**
 **************************************************
 * @file        Inkplate4TEMPERA_Wake_Up_On_Touchscreen.ino
 * @brief       Wakes Inkplate 4 TEMPERA from deep sleep using the wake button
 *              or touchscreen interrupt, and displays the wake-up reason.
 *
 * @details     This example demonstrates low-power wake-up behavior on Inkplate
 *              4 TEMPERA using ESP32 deep sleep and external wake sources. The
 *              touchscreen controller is initialized and kept powered, and its
 *              interrupt line is routed through the board's I/O expander to an
 *              ESP32-capable wake input. A press on the wake button or a
 *              touchscreen event can then wake the MCU from deep sleep.
 *
 *              On each boot, a counter stored in RTC memory (RTC_DATA_ATTR) is
 *              incremented so it persists across deep sleep cycles. The sketch
 *              prints the boot count and the ESP32 wake-up cause on the e-paper
 *              display. It then arms a timer wake-up as a fallback and enters
 *              deep sleep again.
 *
 *              Deep sleep resets the MCU, so all application logic is placed in
 *              setup(), while loop() remains empty. Normal RAM does not survive
 *              deep sleep - only RTC_DATA_ATTR variables (bootCount) do. Display
 *              mode is 1-bit BW (INKPLATE_1BIT).
 *
 *              Wake sources: EXT0 wake is enabled on GPIO 36 (wake button) in
 *              this sketch. Touchscreen interrupt wiring may vary by hardware
 *              revision; some older boards require an additional 10k pull-up
 *              resistor for reliable operation. If touchscreen initialization
 *              fails, the sketch halts to avoid entering a sleep state without a
 *              functional wake source.
 *
 *              Expected output: the display shows "Boot count: <number>" and
 *              "Wakeup caused by timer / external signal / other cause", while
 *              Serial prints the touchscreen init status.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional, for init logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Optionally adjust TIME_TO_SLEEP (seconds) to change the timer wake
 *    interval.
 * 3) Upload the sketch to Inkplate 4 TEMPERA.
 * 4) The screen shows the current boot count and the wake-up reason.
 * 5) Wait for the timer wake-up, or press the wake button / touch the screen to
 *    wake the device.
 * 6) Observe the boot count increment and the reported wake-up cause.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-19
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// Include Inkplate library
#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000ULL
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// bitmask for GPIO_34 which is connected to I/O expander INT pin
#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

// Initiate Inkplate object
Inkplate display(INKPLATE_1BIT);

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    Serial.begin(115200);
    display.begin();

    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.touchscreen.init(true))
    {
        Serial.println("Touchscreen init OK");
    }
    else
    {
        Serial.println("Touchscreen init fail!");
        while (true)
            ;
    }

    ++bootCount;

    // Our function declared below
    displayInfo();

    // Go to sleep for TIME_TO_SLEEP seconds
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    // Enable wakeup from deep sleep on gpio 36 (wake button)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW);

    // Go to sleep
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
    display.setCursor(30, 280);
    display.setTextSize(3);

    display.print(F("Boot count: "));
    display.println(bootCount, DEC); // Print the number

    // Set next line cursor position
    display.setTextSize(2);
    display.setCursor(30, 320);

    // Display wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        display.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        display.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        display.println("Wakeup caused by ULP program");
        break;
    default:
        display.println("Wakeup was not caused by deep sleep");
        break;
    }

    display.display();
}
