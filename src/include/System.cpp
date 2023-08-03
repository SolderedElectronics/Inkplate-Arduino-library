/**
 **************************************************
 * @file        System.cpp
 * @brief       Functionality for panel hardware interaction
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#include "System.h"
#include "Wire.h"

SPIClass spi2(HSPI);
SdFat sd(&spi2);

/**
 * @brief       setPanelState sets panel state (on/off)
 *
 * @param       uint8_t s
 *              panel state ON or OFF (1 or 0)
 */
void System::setPanelState(uint8_t s)
{
    _panelOn = s;
}

/**
 * @brief       getPanelState gets panel state (ON/OFF)
 *
 * @return      returns value that represents ON/OFF state of the panel
 */
uint8_t System::getPanelState()
{
    return _panelOn;
}

#if !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE4) && !defined(ARDUINO_INKPLATE7)

/**
 * @brief       readTemperature reads panel temperature
 *
 * @return      returns  temperature in range from -10 to 85 degree C with
 * accuracy of +-1 in range from 0 to 50
 */
int8_t System::readTemperature()
{
    int8_t temp;
    if (getPanelState() == 0)
    {
        WAKEUP_SET;
        PWRUP_SET;
        delay(5);
    }
    Wire.beginTransmission(0x48);
    Wire.write(0x0D);
    Wire.write(B10000000);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(0x48);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(0x48, 1);
    temp = Wire.read();
    if (getPanelState() == 0)
    {
        PWRUP_CLEAR;
        WAKEUP_CLEAR;
        delay(5);
    }
    return temp;
}

/**
 * @brief       readTouchpad reads touchpad pin to check if triggered
 *
 * @param       uint8_t _pad
 *              touchpad pin to check
 *
 * @return      value to indicate if touchpad is triggered (TRUE or FALSE)
 */
uint8_t System::readTouchpad(uint8_t _pad)
{
    return digitalReadInternal(IO_INT_ADDR, ioRegsInt, _pad);
}

#endif


#ifndef ARDUINO_INKPLATE2
/**
 * @brief       readBattery reads voltage of the battery
 *
 * @return      returns battery voltage value
 */
double System::readBattery()
{
    // Read the pin on the battery MOSFET. If is high, that means is older version of the board
    // that uses PMOS only. If it's low, newer board with both PMOS and NMOS.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 9, INPUT);
    int state = digitalReadIO(9, IO_INT_ADDR);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 9, OUTPUT);

    // If the input is pulled high, it's PMOS only.
    // If it's pulled low, it's PMOS and NMOS.
    if (state)
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, LOW);
    }
    else
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, HIGH);
    }

    // Wait a little bit after a MOSFET enable.
    delay(5);

    // Set to the highest resolution and read the voltage.
    analogReadResolution(12);
    int adc = analogReadMilliVolts(35);

    // Turn off the MOSFET (and voltage divider).
    if (state)
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, HIGH);
    }
    else
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, LOW);
    }

    // Calculate the voltage at the battery terminal (voltage is divided in half by voltage divider).
    return (double(adc) * 2.0 / 1000);
}

/**
 * @brief       sdCardInit initializes sd card trough SPI
 *
 * @return      0 if failed to initialise, 1 if successful
 */
int16_t System::sdCardInit()
{
// New Soldered Inkplate boards use P-MOS to disable supply to the uSD card to reduce power in deep sleep.
#if defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE6PLUSV2) ||               \
    defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE5) || defined(ARDUINO_INKPLATE4) ||                      \
    defined(ARDUINO_INKPLATE7) || defined(ARDUINO_INKPLATEPLUS2)
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, LOW);
    delay(50);
#endif
    spi2.begin(14, 12, 13, 15);
    setSdCardOk(sd.begin(15, SD_SCK_MHZ(25)));
    return getSdCardOk();
}

/**
 * @brief       sdCardSleep turns off the P-MOS which powers the sd card to save energy in deep sleep
 */
void System::sdCardSleep()
{
#if defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE6PLUSV2) ||               \
    defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE5) || defined(ARDUINO_INKPLATE4) ||                      \
    defined(ARDUINO_INKPLATE7) || defined(ARDUINO_INKPLATEPLUS2)
    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);

    // And also disable uSD card supply
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, INPUT);
#endif
}

/**
 * @brief       getSdFat gets sd card object
 *
 * @return      sd card class object
 */
SdFat System::getSdFat()
{
    return sd;
}

/**
 * @brief       getSPIptr gets SPI class object pointer
 *
 * @return      SPI class object
 */
SPIClass *System::getSPIptr()
{
    return &spi2;
}

/**
 * @brief       setSdCardOk sets sd card OK status
 *
 * @param       int16_t s
 *              sd card OK status, can be 1 or 0
 */
void System::setSdCardOk(int16_t s)
{
    _sdCardOk = s;
}

/**
 * @brief       setSdCardOk gets sd card OK status
 *
 * @return      sd card OK status, can be 1 or 0
 */
int16_t System::getSdCardOk()
{
    return _sdCardOk;
}

/**
 * @brief                   Method to set time
 *
 * @param uint8_t rtcHour      Set the rtcHour
 * @param uint8_t rtcMinute    Set the minutes
 * @param uint8_t rtcSecond    Set the seconds
 */
void System::rtcSetTime(uint8_t rtcHour, uint8_t rtcMinute, uint8_t rtcSecond)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(170); // Write in RAM 170 to know that RTC is set
    Wire.write(rtcDecToBcd(rtcSecond));
    Wire.write(rtcDecToBcd(rtcMinute));
    Wire.write(rtcDecToBcd(rtcHour));
    Wire.endTransmission();
}

/**
 * @brief                   Method to set the date
 *
 * @param uint8_t rtcWeekday   Set the rtcWeekday
 * @param uint8_t rtcDay       Set the rtcDay
 * @param uint8_t rtcMonth     Set the rtcMonth
 * @param uint8_t yr        Set the rtcYear
 */
void System::rtcSetDate(uint8_t rtcWeekday, uint8_t rtcDay, uint8_t rtcMonth, uint16_t yr)
{
    rtcYear = yr - 2000; // convert to RTC rtcYear format 0-99

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(170); // Write in RAM 170 to know that RTC is set
    Wire.endTransmission();

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_DAY_ADDR);
    Wire.write(rtcDecToBcd(rtcDay));
    Wire.write(rtcDecToBcd(rtcWeekday));
    Wire.write(rtcDecToBcd(rtcMonth));
    Wire.write(rtcDecToBcd(rtcYear));
    Wire.endTransmission();
}

/**
 * @brief                   Method to set time and date using epoch
 *
 * @param uint32_t _epoch   Set RTC epoch
 */
void System::rtcSetEpoch(uint32_t _epoch)
{
    struct tm _t;
    time_t _e = _epoch;
    memcpy(&_t, localtime((const time_t *)&_e), sizeof(_t));

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(170);
    Wire.write(rtcDecToBcd(_t.tm_sec));
    Wire.write(rtcDecToBcd(_t.tm_min));
    Wire.write(rtcDecToBcd(_t.tm_hour));
    Wire.write(rtcDecToBcd(_t.tm_mday));
    Wire.write(rtcDecToBcd(_t.tm_wday));
    Wire.write(rtcDecToBcd(_t.tm_mon + 1));
    Wire.write(rtcDecToBcd(_t.tm_year + 1900 - 2000));
    Wire.endTransmission();
}

/**
 * @brief                   Method to get time and date using epoch
 *
 * @returns uint32_t        Returns the current epoch
 */
uint32_t System::rtcGetEpoch()
{
    struct tm _t;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 7); // ignore bit 7
    _t.tm_sec = rtcBcdToDec(Wire.read() & 0x7F);
    _t.tm_min = rtcBcdToDec(Wire.read() & 0x7F);
    _t.tm_hour = rtcBcdToDec(Wire.read() & 0x3F);
    _t.tm_mday = rtcBcdToDec(Wire.read() & 0x3F);
    _t.tm_wday = rtcBcdToDec(Wire.read() & 0x07);
    _t.tm_mon = rtcBcdToDec(Wire.read() & 0x1F) - 1;
    _t.tm_year = rtcBcdToDec(Wire.read()) + 2000 - 1900;
    Wire.endTransmission();

    return (uint32_t)(mktime(&_t));
}

/**
 * @brief                   Reads time and date from the RTC
 */
void System::rtcGetRtcData()
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ADDR); // datasheet 8.4.
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 7);

    while (Wire.available())
    {
        rtcSecond = rtcBcdToDec(Wire.read() & 0x7F); // ignore bit 7
        rtcMinute = rtcBcdToDec(Wire.read() & 0x7F);
        rtcHour = rtcBcdToDec(Wire.read() & 0x3F); // ignore bits 7 & 6
        rtcDay = rtcBcdToDec(Wire.read() & 0x3F);
        rtcWeekday = rtcBcdToDec(Wire.read() & 0x07); // ignore bits 7,6,5,4 & 3
        rtcMonth = rtcBcdToDec(Wire.read() & 0x1F);   // ignore bits 7,6 & 5
        rtcYear = rtcBcdToDec(Wire.read()) + 2000;
    }
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current seconds
 */
uint8_t System::rtcGetSecond()
{
    return rtcSecond;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current minutes
 */
uint8_t System::rtcGetMinute()
{
    return rtcMinute;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current hours
 */
uint8_t System::rtcGetHour()
{
    return rtcHour;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcDay
 */
uint8_t System::rtcGetDay()
{
    return rtcDay;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcWeekday
 */
uint8_t System::rtcGetWeekday()
{
    return rtcWeekday;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcMonth
 */
uint8_t System::rtcGetMonth()
{
    return rtcMonth;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcYear
 */
uint16_t System::rtcGetYear()
{
    return rtcYear;
}

/**
 * @brief                   Enables the alarm of the RTC
 */
void System::rtcEnableAlarm() // datasheet 8.5.6.
{
    // check Table 2. Control_2
    rtcControl2 = RTC_CTRL_2_DEFAULT | RTC_ALARM_AIE; // enable interrupt
    rtcControl2 &= ~RTC_ALARM_AF;                     // clear alarm flag

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(rtcControl2);
    Wire.endTransmission();
}

/**
 * @brief                       Sets the alarm to all the params
 *
 * @param uint8_t rtcAlarmSecond  Set the alarm seconds
 * @param uint8_t rtcAlarmMinute  Set the alarm minutes
 * @param uint8_t rtcAlarmHour    Set the alarm hours
 * @param uint8_t rtcAlarmDay     Set the alarm rtcDay
 * @param uint8_t rtcAlarmWeekday Set the alarm rtcWeekday
 */
void System::rtcSetAlarm(uint8_t rtcAlarmSecond, uint8_t rtcAlarmMinute, uint8_t rtcAlarmHour, uint8_t rtcAlarmDay,
                         uint8_t rtcAlarmWeekday)
{
    if (rtcAlarmSecond < 99)
    { // rtcSecond
        rtcAlarmSecond = constrain(rtcAlarmSecond, 0, 59);
        rtcAlarmSecond = rtcDecToBcd(rtcAlarmSecond);
        rtcAlarmSecond &= ~RTC_ALARM;
    }
    else
    {
        rtcAlarmSecond = 0x0;
        rtcAlarmSecond |= RTC_ALARM;
    }

    if (rtcAlarmMinute < 99)
    { // rtcMinute
        rtcAlarmMinute = constrain(rtcAlarmMinute, 0, 59);
        rtcAlarmMinute = rtcDecToBcd(rtcAlarmMinute);
        rtcAlarmMinute &= ~RTC_ALARM;
    }
    else
    {
        rtcAlarmMinute = 0x0;
        rtcAlarmMinute |= RTC_ALARM;
    }

    if (rtcAlarmHour < 99)
    { // rtcHour
        rtcAlarmHour = constrain(rtcAlarmHour, 0, 23);
        rtcAlarmHour = rtcDecToBcd(rtcAlarmHour);
        rtcAlarmHour &= ~RTC_ALARM;
    }
    else
    {
        rtcAlarmHour = 0x0;
        rtcAlarmHour |= RTC_ALARM;
    }

    if (rtcAlarmDay < 99)
    { // rtcDay
        rtcAlarmDay = constrain(rtcAlarmDay, 1, 31);
        rtcAlarmDay = rtcDecToBcd(rtcAlarmDay);
        rtcAlarmDay &= ~RTC_ALARM;
    }
    else
    {
        rtcAlarmDay = 0x0;
        rtcAlarmDay |= RTC_ALARM;
    }

    if (rtcAlarmWeekday < 99)
    { // rtcWeekday
        rtcAlarmWeekday = constrain(rtcAlarmWeekday, 0, 6);
        rtcAlarmWeekday = rtcDecToBcd(rtcAlarmWeekday);
        rtcAlarmWeekday &= ~RTC_ALARM;
    }
    else
    {
        rtcAlarmWeekday = 0x0;
        rtcAlarmWeekday |= RTC_ALARM;
    }

    rtcEnableAlarm();

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM);
    Wire.write(rtcAlarmSecond);
    Wire.write(rtcAlarmMinute);
    Wire.write(rtcAlarmHour);
    Wire.write(rtcAlarmDay);
    Wire.write(rtcAlarmWeekday);
    Wire.endTransmission();
}

/**
 * @brief                   Set alarm using epoch
 *
 * @param uint32_t _epoch   RTC Epoch alarm
 * @param uint8_t _match    RTC Match
 */
void System::rtcSetAlarmEpoch(uint32_t _epoch, uint8_t _match)
{
    struct tm _t;
    time_t _e = _epoch;

    memcpy(&_t, localtime((const time_t *)&_e), sizeof(_t));

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM);
    Wire.write(rtcDecToBcd(_t.tm_sec) & (~((_match & 1) << 7)));
    Wire.write(rtcDecToBcd(_t.tm_min) & (~(((_match >> 1) & 1) << 7)));
    Wire.write(rtcDecToBcd(_t.tm_hour) & (~(((_match >> 2) & 1) << 7)));
    Wire.write(rtcDecToBcd(_t.tm_mday) & (~(((_match >> 3) & 1) << 7)));
    Wire.write(rtcDecToBcd(_t.tm_wday) & (~(((_match >> 4) & 1) << 7)));
    Wire.endTransmission();

    rtcEnableAlarm();
}


/**
 * @brief                   Reads the alarm of the RTC
 */
void System::rtcReadAlarm()
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM); // datasheet 8.4.
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 5);

    while (Wire.available())
    {
        rtcAlarmSecond = Wire.read();   // read RTC_SECOND_ALARM register
        if (RTC_ALARM & rtcAlarmSecond) // check is AEN = 1 (rtcSecond alarm disabled)
        {
            rtcAlarmSecond = 99; // using 99 as code for no alarm
        }
        else
        {                                                              // else if AEN = 0 (rtcSecond alarm enabled)
            rtcAlarmSecond = rtcBcdToDec(rtcAlarmSecond & ~RTC_ALARM); // remove AEN flag and convert to dec number
        }

        rtcAlarmMinute = Wire.read(); // rtcMinute
        if (RTC_ALARM & rtcAlarmMinute)
        {
            rtcAlarmMinute = 99;
        }
        else
        {
            rtcAlarmMinute = rtcBcdToDec(rtcAlarmMinute & ~RTC_ALARM);
        }

        rtcAlarmHour = Wire.read(); // rtcHour
        if (RTC_ALARM & rtcAlarmHour)
        {
            rtcAlarmHour = 99;
        }
        else
        {
            rtcAlarmHour = rtcBcdToDec(rtcAlarmHour & 0x3F); // remove bits 7 & 6
        }

        rtcAlarmDay = Wire.read(); // rtcDay
        if (RTC_ALARM & rtcAlarmDay)
        {
            rtcAlarmDay = 99;
        }
        else
        {
            rtcAlarmDay = rtcBcdToDec(rtcAlarmDay & 0x3F); // remove bits 7 & 6
        }

        rtcAlarmWeekday = Wire.read(); // rtcWeekday
        if (RTC_ALARM & rtcAlarmWeekday)
        {
            rtcAlarmWeekday = 99;
        }
        else
        {
            rtcAlarmWeekday = rtcBcdToDec(rtcAlarmWeekday & 0x07); // remove bits 7,6,5,4 & 3
        }
    }
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm seconds
 */
uint8_t System::rtcGetAlarmSecond()
{
    rtcReadAlarm();
    return rtcAlarmSecond;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm minutes
 */
uint8_t System::rtcGetAlarmMinute()
{
    rtcReadAlarm();
    return rtcAlarmMinute;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm hours
 */
uint8_t System::rtcGetAlarmHour()
{
    rtcReadAlarm();
    return rtcAlarmHour;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm rtcDay
 */
uint8_t System::rtcGetAlarmDay()
{
    rtcReadAlarm();
    return rtcAlarmDay;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm rtcWeekday
 */
uint8_t System::rtcGetAlarmWeekday()
{
    rtcReadAlarm();
    return rtcAlarmWeekday;
}

/**
 * @brief                   Sets the timer countdown
 *
 * @param                   rtcCountdownSrcClock source_clock
 *                          timer clock frequency
 *
 * @param                   timer clock frequency
 *                          value to write in timer register
 *
 * @param                   bool int_enable
 *                          timer interrupt enable, 0 means no interrupt generated from timer
 *                          , 1 means interrupt is generated from timer
 *
 * @param                   bool int_pulse
 *                          timer interrupt mode, 0 means interrupt follows timer flag
 *                          , 1 means interrupt generates a pulse
 */
void System::rtcTimerSet(rtcCountdownSrcClock source_clock, uint8_t value, bool int_enable, bool int_pulse)
{
    uint8_t timer_reg[2] = {0};

    // disable the countdown timer
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.write(0x18); // default
    Wire.endTransmission();

    // clear Control_2
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(0x00); // default
    Wire.endTransmission();

    // reconfigure timer
    timer_reg[1] |= RTC_TIMER_TE; // enable timer
    if (int_enable)
        timer_reg[1] |= RTC_TIMER_TIE; // enable interrupt
    if (int_pulse)
        timer_reg[1] |= RTC_TIMER_TI_TP; // interrupt mode
    timer_reg[1] |= source_clock << 3;   // clock source
    // timer_reg[1] = 0b00011111;

    timer_reg[0] = value;

    // write timer value
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_VAL);
    Wire.write(timer_reg[0]);
    Wire.write(timer_reg[1]);
    Wire.endTransmission();
}

/**
 * @brief                   Returns is the timer flag on
 *
 * @returns bool            Returns true if the timer flag is on
 */
bool System::rtcCheckTimerFlag()
{
    uint8_t _crtl_2 = RTC_TIMER_FLAG;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    _crtl_2 &= Wire.read();

    return _crtl_2;
}

/**
 * @brief                   Returns is the alarm flag on
 *
 * @returns bool            Returns true if the alarm flag is on
 */
bool System::rtcCheckAlarmFlag()
{
    uint8_t _crtl_2 = RTC_ALARM_AF;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    _crtl_2 &= Wire.read();

    return _crtl_2;
}

/**
 * @brief                   Clears alarm flag
 */
void System::rtcClearAlarmFlag()
{
    uint8_t _crtl_2;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _crtl_2 = Wire.read() & ~(RTC_ALARM_AF);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(_crtl_2);
    Wire.endTransmission();
}

/**
 * @brief                   Clears timer flag
 */
void System::rtcClearTimerFlag()
{
    uint8_t _crtl_2;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _crtl_2 = Wire.read() & ~(RTC_TIMER_FLAG);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(_crtl_2);
    Wire.endTransmission();
}

/**
 * @brief                   Disables the timer
 */
void System::rtcDisableTimer()
{
    uint8_t _timerMode;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _timerMode = Wire.read() & ~(RTC_TIMER_TE);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.write(_timerMode);
    Wire.endTransmission();
}

/**
 * @brief                   Check if the RTC is already set
 *
 * @returns bool            Returns true if RTC is set, false if it's not
 */
bool System::rtcIsSet()
{
    uint8_t _ramByte;
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 1);
    _ramByte = Wire.read();
    return ((_ramByte == 170) ? true : false);
}

/**
 * @brief                   Resets the timer
 */
void System::rtcReset() // datasheet 8.2.1.3.
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.write(0x58);
    Wire.endTransmission();
}

/**
 * @brief                   Set internal capacitor value.
 *
 * @param bool val          0 or 1 which represents 7pF or 12.5 pF.
 */
void System::rtcSetInternalCapacitor(bool val)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.endTransmission();

    uint8_t reg;
    Wire.requestFrom(I2C_ADDR, 1);

    if (Wire.available())
    {
        reg = Wire.read();
    }

    if (val)
    {
        reg |= (1 << 0);
    }
    else
    {
        reg &= ~(1 << 0);
    }

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.write(reg);
    Wire.endTransmission();
}

/**
 * @brief                   Offset used to correct the frequency of the crystal used for RTC.
 *                          8.2.3 in the datasheet.
 *
 * @param bool mode         0 - normal mode -> offset is made once every two hours.
 *                          Each LSB introduces an offset of 4.34 ppm.
 *                          1 - course mode -> offset is made every 4 minutes.
 *                          Each LSB introduces an offset of 4.069 ppm.
 *
 * @param byte offsetValue  The offset value is coded in two’s complement giving a
 *                          range of +63 LSB to -64 LSB.
 */
void System::rtcSetClockOffset(bool mode, int offsetValue)
{
    // Byte for writting in the register
    uint8_t regValue;

    // Check offset value
    if (offsetValue > 63 || offsetValue < -64)
    {
        return;
    }

    // Use two's complement
    if (offsetValue < 0)
    {
        offsetValue += 128;
    }

    // Save it in the byte for register
    regValue = (byte)offsetValue;

    // Write mode in the MSB
    if (mode)
    {
        regValue |= (1 << 7); // Set MSB to 1
    }
    else
    {
        regValue &= ~(1 << 7); // Set MSB to 0
    }

    // Send to the register
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_OFFSET);
    Wire.write(regValue);
    Wire.endTransmission();
}

/**
 * @brief                   Converts decimal to BCD
 *
 * @param                   uint8_t val
 *                          number which needs to be converted from decimal to Bcd value
 */
uint8_t System::rtcDecToBcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

/**
 * @brief                   Converts BCD to decimal
 *
 * @param                   uint8_t val
 *                          number which needs to be converted from Bcd to decimal value
 */
uint8_t System::rtcBcdToDec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

#endif
