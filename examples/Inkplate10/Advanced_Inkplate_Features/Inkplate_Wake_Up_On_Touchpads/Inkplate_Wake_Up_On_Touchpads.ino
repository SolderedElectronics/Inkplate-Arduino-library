/*
   Inkplate_Wake_up_on_touchpads example for e-radionica.com Inkplate 10
   For this example you will need USB cable and an Inkplate 10
   Select "Inkplate 10(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 10(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   Here is shown how to use MCP and ESP interrupts to wake up the MCU from deepsleep when touchpad is pressed.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 30

// bitmask for GPIO_34 which is connected to MCP INTB
#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

// Initiate Inkplate object
Inkplate display(INKPLATE_1BIT);

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

// If your Inkplate doesn't have external (or second) MCP I/O expander, you should uncomment next line,
// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander

void setup()
{
    Serial.begin(115200);
    display.begin();

    // Setup mcp interrupts
    display.setIntOutputInternal(MCP23017_INT_ADDR, display.mcpRegsInt, 1, false, false, HIGH);
    display.setIntPinInternal(MCP23017_INT_ADDR, display.mcpRegsInt, PAD1, RISING);
    display.setIntPinInternal(MCP23017_INT_ADDR, display.mcpRegsInt, PAD2, RISING);
    display.setIntPinInternal(MCP23017_INT_ADDR, display.mcpRegsInt, PAD3, RISING);

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
