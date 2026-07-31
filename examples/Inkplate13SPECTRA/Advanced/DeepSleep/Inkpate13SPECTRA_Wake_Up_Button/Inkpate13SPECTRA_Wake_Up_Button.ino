/**
 **************************************************
 * @file        Inkpate13SPECTRA_Wake_Up_Button.ino
 * @brief       Wake-up button and timer deep sleep example for Soldered
 *              Inkplate 13SPECTRA.
 *
 * @details     Shows how to wake the ESP32 from deep sleep using the WakeUp
 *              button (external interrupt on GPIO18) and a fallback timer. A
 *              boot counter is stored in RTC memory (RTC_DATA_ATTR) so it
 *              survives deep sleep; on every wake the sketch prints the boot
 *              count and the wake-up reason (WakeUp button or timer) on the
 *              e-paper display and then sleeps again for TIME_TO_SLEEP seconds
 *              (120 s by default).
 *
 *              When using deep sleep, the whole program has to be in setup()
 *              because the board restarts on every wake-up, so loop() must
 *              remain empty.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) After the boot info is displayed, the board enters deep sleep.
 * 4) Press the WakeUp button, or wait 120 seconds for the timer wake-up.
 * 5) On each wake the display shows the new boot count and wake-up reason.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 **************************************************/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include <Inkplate.h>

// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000ULL
// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP 120

// Initiate Inkplate object
Inkplate inkplate;

// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount=0;

void setup() {
  inkplate.begin();
  inkplate.setTextColor(INKPLATE_BLACK);
  ++bootCount;

  // Our function delcared below
  displayInfo();

  // Go to sleep for TIME_TO_SLEEP seconds
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Enable wakeup from deep sleep on gpio 18 (wake button)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, 0);
  // Start deep sleep (this function does not return). Program stops here.
  esp_deep_sleep_start();

}

void loop() {
  // Never here! If you use deep sleep, the whole program should be in setup() because the board restarts each
  // time. loop() must be empty! 

}

void displayInfo(){
  // First, lets delete everything from frame buffer
  inkplate.clearDisplay();

  // Set text cursor and size
  inkplate.setCursor(10,180);
  inkplate.setTextSize(2);

  inkplate.print(F("Boot count: "));
  inkplate.println(bootCount, DEC); // Print the number

  // Set next line cursor position
  inkplate.setCursor(10,220);

  // Display wake up reason
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0:
      inkplate.println("Wakeup caused by WakeUp button");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      inkplate.println("Wakeup caused by timer");
      break;
    default:
      inkplate.println("Wakeuo was not caused by deep sleep");
      break;
  }
  inkplate.display();

}