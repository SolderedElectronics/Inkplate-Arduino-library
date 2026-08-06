/**
 **************************************************
 * @file        Inkplate6COLOR_RTC_Interrupt_Alarm.ino
 * @brief       Demonstrates RTC alarm interrupt handling on Inkplate 6COLOR.
 *
 * @details     This example shows how to use the onboard PCF85063(A) RTC alarm
 *              together with its interrupt output on Inkplate 6COLOR. The
 *              sketch initializes the RTC, sets the current time using an epoch
 *              value, programs an alarm for 60 seconds later, and attaches an
 *              ESP32 interrupt handler to the RTC interrupt pin on GPIO39.
 *
 *              During normal operation, the sketch periodically reads the RTC
 *              time/date and prints it on the display using full refreshes with
 *              a 60-second delay between updates. When the RTC alarm occurs, the
 *              interrupt service routine sets a software flag, and the main loop
 *              detects that flag, clears the RTC alarm flag, and shows an
 *              "ALARM" message on the screen.
 *
 *              This example demonstrates RTC alarm + interrupt signaling, but
 *              it does not use deep sleep wake-up. The MCU remains awake and
 *              handles the interrupt while running normally. For deep sleep
 *              wake-up via RTC alarm, use the dedicated deep sleep RTC example.
 *
 *              Keep ISR logic minimal: in this example the interrupt handler
 *              only sets a volatile flag, while display updates and RTC flag
 *              clearing are handled in the main loop. Clearing the RTC alarm
 *              flag after the event is recommended to re-arm alarm-related
 *              behavior cleanly.
 *
 *              Expected output: the current time, weekday and date on the
 *              display, with "ALARM" appearing after the configured RTC alarm
 *              interrupt occurs.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     not used in this example
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Adjust the initial epoch/alarm configuration in the sketch if needed.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) On startup, the sketch resets the RTC, sets the current time from the
 *    configured epoch value and programs an RTC alarm 60 seconds in the future.
 * 5) GPIO39 is configured as the RTC interrupt input and an ISR is attached to
 *    detect the falling-edge alarm signal.
 * 6) The display refreshes periodically to show the current time and date.
 * 7) When the RTC alarm triggers, the ISR sets a flag and the main loop prints
 *    "ALARM" after clearing the RTC alarm flag.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-05-19
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"             // Include Inkplate library to the sketch
Inkplate display;  // Create an object on Inkplate library and also set library into 1-bit mode (BW)

volatile int _alarmFlag = 0;      // Variable to store alarm flag

void IRAM_ATTR alarmISR()         // This function will be called when alarm interrupt event happens
{                                 // NOTE: Function must be above setup() and loop()!
  _alarmFlag = 1;                 // Set alarm flag
}

void setup()
{
    pinMode(39, INPUT_PULLUP);      // Set RTC INT pin on ESP32 GPIO39 as input with pullup resistor enabled

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.rtc.reset();           //  reset RTC if there is some data in it
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 4 times bigger than classic 5x7 px text
  
    display.rtc.setEpoch(1650000000);
    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS);

    // display.rtc.setTime(6, 25, 0);        // Or you can use other way to set the time and date
    // display.rtc.setDate(6, 16, 5, 2020);
    // display.rtc.setAlarm(10, 25, 6, 16, 6); // Set alarm 10 seconds from now
  
    attachInterrupt(39, alarmISR, FALLING); // Set interrupt function and interrupt mode
}

void loop()
{
    display.clearDisplay();         // Clear frame buffer of display
    display.setCursor(60, 100);    // Set position of the text
    display.setTextColor(INKPLATE_RED, INKPLATE_WHITE);         //Set text color and background
    display.rtc.getRtcData();          // Get the time and date from RTC

    // Print the time on screen
    printTime(display.rtc.getHour(), display.rtc.getMinute(), display.rtc.getSecond(), display.rtc.getDay(), display.rtc.getWeekday(), display.rtc.getMonth(), display.rtc.getYear());
    
    if (_alarmFlag)     // Check alarm flag
    {
        // _alarmFlag = 0;              // Uncomment if you want to clear this flag
        display.rtc.clearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
        display.setCursor(200, 200);    // Set position of the text
        display.print("ALARM");         // Print text
    }
    
     display.display(); // Do a full refresh

     delay(60000); //Wait one minute
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint8_t _month,
               uint16_t _year)
{
    // Write time and date info on screen
    char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    print2Digits(_hour);
    display.print(':');
    print2Digits(_minutes);
    display.print(':');
    print2Digits(_seconds);

    display.print(' ');

    display.print(wday[_weekday]);
    display.print(", ");
    print2Digits(_day);
    display.print('/');
    print2Digits(_month);
    display.print('/');
    display.print(_year, DEC);
}

void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
