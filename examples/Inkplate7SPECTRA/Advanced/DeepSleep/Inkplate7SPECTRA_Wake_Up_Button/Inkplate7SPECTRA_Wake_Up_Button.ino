#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
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
  inkplate.setTextSize(1);

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