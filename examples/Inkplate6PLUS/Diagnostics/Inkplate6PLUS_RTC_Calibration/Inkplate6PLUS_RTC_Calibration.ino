/**
 **************************************************
 * @file        Inkplate6PLUS_RTC_Calibration.ino
 * @brief       Demonstrates RTC calibration on Inkplate 6PLUS by adjusting the
 *              RTC crystal load capacitor and clock offset registers, then
 *              showing a running clock using partial updates.
 *
 * @details     This example is intended to improve real-time clock accuracy on
 *              Inkplate 6PLUS by configuring the on-board RTC (PCF85063(A)). It
 *              demonstrates two calibration mechanisms:
 *
 *              1) Crystal load capacitance selection:
 *                 Some boards use external load capacitors for the 32.768 kHz
 *                 crystal. This sketch shows how to enable an internal load
 *                 capacitor (e.g., 7 pF or 12.5 pF). If you switch to the
 *                 internal capacitor, external capacitors must be removed for
 *                 correct operation.
 *
 *              2) Clock offset correction:
 *                 The RTC provides an offset register that periodically applies
 *                 small timing corrections. You can choose the correction mode
 *                 (applied every ~2 hours or every ~4 minutes) and a signed
 *                 offset value. The comments include a procedure to calculate
 *                 the required offset either from an oscilloscope frequency
 *                 measurement or from a multi-day drift comparison against a
 *                 reference clock.
 *
 *              After configuration, the sketch waits for a button press and
 *              starts the RTC at 00:00:00. It then reads the RTC once per
 *              second and updates the displayed time. To reduce flicker and
 *              speed up updates, it uses partial updates in 1-bit BW mode
 *              (INKPLATE_1BIT) and performs a full refresh after
 *              MAX_PARTIAL_UPDATES partial refreshes to reduce ghosting.
 *              Partial updates are performed with panel power kept enabled
 *              (e.g. via the partialUpdate(..., true) setting), which can
 *              increase power usage but makes repeated updates faster and more
 *              stable.
 *
 *              E-paper refresh latency can make the shown time appear to "jump"
 *              occasionally; the RTC time itself remains continuous and
 *              accurate. The RTC alarm/timer/interrupt features are separate
 *              from this calibration example - this sketch focuses on crystal
 *              load and offset trim.
 *
 *              Expected output: the prompt "Press the wake button to start
 *              RTC!", then a large HH:MM:SS clock updated roughly once per
 *              second.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      none (optional: reference clock / oscilloscope for calibration)
 * - Serial:     none
 * - Calibration settings (edit in sketch):
 *   - display.rtc.setInternalCapacitor(RTC_7PF) or RTC_12_5PF; if using
 *     external capacitors, do not enable the internal capacitor
 *   - display.rtc.setClockOffset(mode, value); follow the included procedure to
 *     compute mode/value from measured drift
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Decide whether you are using external crystal capacitors or internal RTC
 *    capacitance (hardware-dependent). Configure setInternalCapacitor() only
 *    if appropriate.
 * 3) For drift measurement runs, comment out setClockOffset() (and optionally
 *    setInternalCapacitor()) to measure baseline RTC error.
 * 4) Upload the sketch. On the display, press the wake button to start the RTC.
 * 5) Let the clock run and compare against a trusted reference over many hours
 *    or days to compute ppm error and required offset.
 * 6) Apply the calculated setClockOffset() value, re-upload, and verify.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-04-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

#define REFRESH_DELAY 1000 // Delay between refreshes
unsigned long time1;       // Time for measuring refresh in millis

#define MAX_PARTIAL_UPDATES 9 // How many partial updates to do before a full refresh

// Variable that keeps count on how much screen has been partially updated
int n = 0;

// Set initial time and date
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;

void setup()
{
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(4); // Set text to be 4 times bigger than classic 5x7 px text

    pinMode(GPIO_NUM_36, INPUT); // Set wake-up button as input

    // Some Inkplates has external capacitors for RTC crystal, but you can use internal one if you have issues with accuracy. 
    // IMPORTANT:
    //  - If you use an internal capacitor, you have to remove the external ones. 
    //  - If you use an external one, you don't have the next line of code. 
    // Here we setting internal capacitor value (7 pF):
    // display.rtc.setInternalCapacitor(RTC_7PF); 
    // Another option is 12.5 pF:
    display.rtc.setInternalCapacitor(RTC_12_5PF);

    // Set offset for RTC crystal
    // The first argument is a mode (0 or 1):
    // 0 means that the offset is made once every two hours (Each LSB introduces an offset of 4.34 ppm)
    // 1 means that the offset is made every 4 minutes (Each LSB introduces an offset of 4.069 ppm)
    // The second argument is the offset value in decimal (from -64 to 63).
    // The real offset depends on the mode and it is equal to the: offset in ppm for specific mode * offset value in
    // decimal. For example: mode 0 (4.34 ppm), offset value 15 = + 65.1 ppm every 2 hours
    // See 8.2.3 in the datasheet for more details
    
    display.rtc.setClockOffset(1, -63); 

    // How to calculate this offset?
    // 1. Measure the frequency on the clock pin of the RTC (let's call it fMeasured)
    // 2. Convert it to time (tMeasured = 1 / fMeasured)
    // 3. Calculate the difference to the ideal period of 1 / 32768.00: D = 1 / 32768 - tMeasured
    // 4. Calculate the ppm deviation compared to the measured value: Eppm = 1000000 * D / tMeasured
    // 5. Calculate the offset register value:
    // Mode = 0 -> Offset = Eppm / 4.34
    // Mode = 1 -> Offset = Eppm / 4.069
    // Round this number and this is the second parameter of the function

    // If you don't have an oscilloscope or something to measure the frequency, here is a procedure for you. 
    // NOTE: This is a longer, but more precise method to calibrate RTC.
    // When you run for the first time to see how much rtc misses, 
    // you MUST comment the display.rtc.setClockOffset() function above.
    // Once again, if you are using external capacitor, you don't need neither 
    // display.rtc.setInternalCapacitor(); so also comment this line.

    // First, upload the code to the Inkplate.
    // It would be best if you had a clock on the side (on a phone or computer).
    // Press the wake button at a certain time.
    // Remember that time or write it down somewhere because you will need it for the calculation.
    // Let the Inkplate count for a while.
    // The longer you wait, the more accurate the calculation will be. We recommend waiting 2-3 days.
    // When that time has passed, remember the current time and the time displayed on the Inkplate AT THE SAME TIME 
    // (We recommend taking pictures of them next to each other).
    // Now calculate how many seconds of real-time have passed and how many the Inkplate has counted.
    // Calculate the time counted by the Inkplate divided by the time that actually passed.
    // Then you will get a number that needs to be multiplied by 32768.
    // The number you get is the RTC frequency.
    // With that number, perform the above calculation as if you measured the frequency with an oscilloscope.
    // Pass the resulting number as the second argument of the function.
    
    // Print a message for waiting
    display.setCursor(90, 360);
    display.println("Press the wake button to start RTC!");
    display.partialUpdate();

    // Waiting for the button press
    while(digitalRead(GPIO_NUM_36) == HIGH)
    {
      // Waiting
    }

    // Set the RTC to begin
    display.rtc.setTime(hours, minutes, seconds);    // Send time to RTC
}

void loop()
{ 
    // Print new time every second
    // NOTE: The display needs some time to refresh, so the time will
    // sometimes seem wrong but that actual RTC time will be precise
    if ((unsigned long)(millis() - time1) > REFRESH_DELAY)
    {
        display.rtc.getRtcData();           // Get the time and date from RTC
        seconds = display.rtc.getSecond();  // Store senconds in a variable
        minutes = display.rtc.getMinute();  // Store minutes in a variable
        hours = display.rtc.getHour();      // Store hours in a variable

        display.clearDisplay();             // Clear content in frame buffer
        display.setTextSize(5);             // Set text to be 5 times bigger than classic 5x7 px text
        display.setCursor(380, 360);        // Set position of the text
        printTime(hours, minutes, seconds); // Print the time on screen

        if (n > MAX_PARTIAL_UPDATES) // Check if you need to do full refresh or you can do partial update
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

void printTime(uint8_t _hour, uint8_t _minutes, uint8_t _seconds)
{
    // Print time
    print2Digits(_hour);
    display.print(':');
    print2Digits(_minutes);
    display.print(':');
    print2Digits(_seconds);
}

// A function that adds 0 before one digit number
void print2Digits(uint8_t _d)
{
    if (_d < 10)
        display.print('0');
    display.print(_d, DEC);
}
