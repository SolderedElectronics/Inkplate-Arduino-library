/*
   Inkplate10_Wake_Up_On_Touchpads example for Soldered Inkplate 10
   For this example you will need USB cable and an Inkplate 10
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Here is shown how to use I/O Expander and ESP interrupts to wake up the MCU from deepsleep when touchpad is pressed.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 February 2021 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// bitmask for GPIO_34 which is connected to I/O Expander INT pin
#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

// Initiate Inkplate object
Inkplate display(INKPLATE_1BIT);

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    Serial.begin(115200);
    display.begin();

// Different I/O expander has been used on Soldered Inkplate V2 board and original Soldered Inkplate.
#ifdef ARDUINO_INKPLATE10
    display.setIntOutput(1, false, false, HIGH, IO_INT_ADDR);
    display.setIntPin(PAD1, RISING, IO_INT_ADDR);
    display.setIntPin(PAD2, RISING, IO_INT_ADDR);
    display.setIntPin(PAD3, RISING, IO_INT_ADDR);
#elif ARDUINO_INKPLATE10V2
    display.setIntPin(PAD1, IO_INT_ADDR);
    display.setIntPin(PAD2, IO_INT_ADDR);
    display.setIntPin(PAD3, IO_INT_ADDR);
#endif

    ++bootCount;

    // Our function declared below
    displayInfo();

    // Go to sleep for TIME_TO_SLEEP seconds
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    // Enable wakup from deep sleep on gpio 36
    esp_sleep_enable_ext1_wakeup((1ULL << 36), ESP_EXT1_WAKEUP_ALL_LOW);

    // Go to sleep
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Function that will write number of boots and boot reason to screen
void displayInfo()
{
    // First, lets delete everything from frame buffer
    display.clearDisplay();

    // Set text cursor and size
    display.setCursor(10, 280);
    display.setTextSize(2);

    display.print(F("Boot count: "));
    display.println(bootCount, DEC); // Print the number

    // Set next line cursor position
    display.setCursor(10, 320);

    // Display wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        display.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        display.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        display.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        display.println("Wakeup caused by touchpad");
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
