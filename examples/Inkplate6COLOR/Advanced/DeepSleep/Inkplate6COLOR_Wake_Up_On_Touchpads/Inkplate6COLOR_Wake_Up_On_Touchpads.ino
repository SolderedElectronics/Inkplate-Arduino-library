/*
   Inkplate6COLOR_Wake_Up_On_Touchpads example for Soldered Inkplate 6 COLOR
   For this example you will need USB cable and an Inkplate 6 COLOR
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Here is shown how to use I/O Expander and ESP interrupts to wake up the MCU from deepsleep when touchpad is pressed.
   
   Want to learn more about Inkplate? Visit www.inkplate.io   
   Looking to get support? Write on our forums: https://forum.soldered.com/
   13 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Include Inkplate library to the sketch
#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000

// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// Initiate Inkplate object
Inkplate display;

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin(); 

    // Set interrupt on each touchpad
    display.setIntPin(PAD1);
    display.setIntPin(PAD2);
    display.setIntPin(PAD3);

    ++bootCount;

    // Our function declared below
    displayInfo();

    // Go to sleep for TIME_TO_SLEEP seconds, but also enable wake up from gpio 34
    // Gpio 34 is where the I/O Expander interrupt is connected
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_34, LOW);

    // Go to sleep
    // Put the panel in the deep sleep
    display.setPanelDeepSleep(0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Function that will write number of boots and boot reason to screen
void displayInfo()
{
    // First, lets delete everything from frame buffer
    display.clearDisplay();

    // Set text cursor and size
    display.setCursor(10, 280);
    display.setTextSize(2);
    display.setTextColor(INKPLATE_BLACK);

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
        display.println("Wakeup caused by touchpads");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        display.println("Wakeup caused by timer");
        break;
    default:
        display.println("Wakeup was not caused by deep sleep");
        break;
    }

    // Show a message on the screen
    display.display();
}
