/*
   Inkplate10_RTC_Calibration example for Soldered Inkplate 10
   For this example you will need USB cable and Inkplate 10.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to Calibrate RTC to be more precise and accurate.
   If you have any issues with the time precision, in this way you can change the internal capacitor value, 
   and set the clock offset.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   18 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

#define REFRESH_DELAY 1000 // Delay between refreshes
unsigned long time1;       // Time for measuring refresh in millis

// Set clock
uint8_t hours = 8;
uint8_t minutes = 8;
uint8_t seconds = 00;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 menas Monday, ...)
uint8_t weekday = 2;
uint8_t day = 25;
uint8_t month = 4;
uint8_t year = 23;

// Variable that keeps count on how much screen has been partially updated
int n = 0;

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(5); // Set text to be 5 times bigger than classic 5x7 px text

    // Some Inkplates has external capacitors for RTC crystal, but you can use internal one if you have issues with
    // accuracy. Here we setting internal capacitor value (7 pF):
    // display.rtcSetInternalCapacitor(RTC_7PF); 
    // Another option is 12.5 pF:
    display.rtcSetInternalCapacitor(RTC_12_5PF);

    // Set offset for RTC crystal
    // The first argument is a mode (0 or 1):
    // 0 means that the offset is made once every two hours (Each LSB introduces an offset of 4.34 ppm)
    // 1 means that the offset is made every 4 minutes (Each LSB introduces an offset of 4.069 ppm)
    // The second argument is the offset value in decimal (from -64 to 63).
    // The real offset depends on the mode and it is equal to the: offset in ppm for specific mode * offset value in
    // decimal. For example: mode 0 (4.34 ppm), offset value 15 = +65.1 ppm every 2 hours
    // See 8.2.3 in the datasheet for more details
    display.rtcSetClockOffset(1, 63);

    // How to calculate this offset?
    // 1. Measure the frequency on the clock pin of the RTC (let's call it fMeasured)
    // 2. Convert it to time (tMeasured = 1 / fMeasured)
    // 3. Calculate the difference to the ideal period of 1 / 32768.00: D = 1 / 32768 - tMeasured
    // 4. Calculate the ppm deviation compared to the measured value: Eppm = 1000000 * D / tMeasured
    // 5. Calculate the offset register value:
    // Mode = 0 -> Offset = Eppm / 4.34
    // Mode = 1 -> Offset = Eppm / 4.069
    // Round this number and this is the second parameter of the function

    // You can leave this sketch running some time (a few days) and see if the clock is now more accurate

    // Set the time and date
    display.rtcSetTime(hours, minutes, seconds);
    display.rtcSetDate(weekday, day, month, year);
}

void loop()
{
    // Print new time every second
    // NOTE: The display needs some time to refresh, so the time will
    // sometimes seem wrong but that actual RTC time will be precise
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        display.rtcGetRtcData();           // Get the time and date from RTC
        seconds = display.rtcGetSecond();  // Store senconds in a variable
        minutes = display.rtcGetMinute();  // Store minutes in a variable
        hours = display.rtcGetHour();      // Store hours in a variable
        day = display.rtcGetDay();         // Store day of month in a variable
        weekday = display.rtcGetWeekday(); // Store day of week in a variable
        month = display.rtcGetMonth();     // Store month in a variable
        year = display.rtcGetYear();       // Store year in a variable

        display.clearDisplay();                                        // Clear content in frame buffer
        display.setCursor(200, 380);                                   // Set position of the text
        printTime(hours, minutes, seconds, day, weekday, month, year); // Print the time on screen

        if (n > 9) // Check if you need to do full refresh or you can do partial update
        {
            display.display(true); // Do a full refresh
            n = 0;
        }
        else
        {
            display.partialUpdate(false, true); // Do partial update and keep e-papr power supply on
            n++;                                // Keep track on how many times screen has been partially updated
        }

        time1 = millis(); // Store current millis
    }
}

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds, uint8_t _day, uint8_t _weekday, uint8_t _month,
               uint16_t _year)
{
    // Write time and date info on screen
    char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    // Print time
    print2Digits(_hour);
    display.print(':');
    print2Digits(_minutes);
    display.print(':');
    print2Digits(_seconds);

    display.print(' ');

    // Print date
    display.print(wday[_weekday]);
    display.print(", ");
    print2Digits(_day);
    display.print('/');
    print2Digits(_month);
    display.print('/');
    display.print(_year, DEC);
}

// A function that adds 0 before one digit number
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
