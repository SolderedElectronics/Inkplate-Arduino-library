/**
 **************************************************
 * @file        rtc.cpp
 * @brief       Functionality for the onboard real time clock
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *              License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *              LICENSE file included with this example. If you have any questions about
 *              licensing, please contact assistance@soldered.com Distributed as-is; no
 *              warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#include "rtc.h"
#include "../../system/inkplateSemaphore.h"
#include <ctime>


/**
 * @brief                   Method to set time
 *
 * @param uint8_t rtcHour      Set the rtcHour
 * @param uint8_t rtcMinute    Set the minutes
 * @param uint8_t rtcSecond    Set the seconds
 */
void RTC::setTime(uint8_t rtcHour, uint8_t rtcMinute, uint8_t rtcSecond, bool isPM)
{
    if (_12hMode && rtcHour > 12) // user passed 24h value
    {
        _isPM = true;
        rtcHour = rtcHour - 12;
    }
    else if (_12hMode && rtcHour == 0) // midnight edge case
    {
        _isPM = false;
        rtcHour = 12;
    }
    else
    {
        _isPM = isPM;
    }

    uint8_t hourBcd = decToBcd(rtcHour);
    if (_12hMode && isPM)
        hourBcd |= (1 << 5);

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(RTC_SET); // Write in RAM to know that RTC is set
    Wire.write(decToBcd(rtcSecond));
    Wire.write(decToBcd(rtcMinute));
    Wire.write(hourBcd);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Method to set the date
 *
 * @param uint8_t rtcWeekday   Set the rtcWeekday
 * @param uint8_t rtcDay       Set the rtcDay
 * @param uint8_t rtcMonth     Set the rtcMonth
 * @param uint8_t yr        Set the rtcYear
 */
void RTC::setDate(uint8_t rtcWeekday, uint8_t rtcDay, uint8_t rtcMonth, uint16_t yr)
{
    Year = yr - 2000; // convert to RTC rtcYear format 0-99

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(RTC_SET); // Write in RAM to know that RTC is set
    Wire.endTransmission();
    i2cEnd();

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_DAY_ADDR);
    Wire.write(decToBcd(rtcDay));
    Wire.write(decToBcd(rtcWeekday));
    Wire.write(decToBcd(rtcMonth));
    Wire.write(decToBcd(Year));
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Method to set time and date using epoch
 *
 * @param uint32_t _epoch   Set RTC epoch
 */
void RTC::setEpoch(uint32_t _epoch)
{
    struct tm _t;
    time_t _e = _epoch;
    memcpy(&_t, localtime((const time_t *)&_e), sizeof(_t));

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.write(RTC_SET);
    Wire.write(decToBcd(_t.tm_sec));
    Wire.write(decToBcd(_t.tm_min));
    Wire.write(decToBcd(_t.tm_hour));
    Wire.write(decToBcd(_t.tm_mday));
    Wire.write(decToBcd(_t.tm_wday));
    Wire.write(decToBcd(_t.tm_mon + 1));
    Wire.write(decToBcd(_t.tm_year + 1900 - 2000));
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Method to get time and date using epoch
 *
 * @returns uint32_t        Returns the current epoch
 */
uint32_t RTC::getEpoch()
{
    updateTime();
    struct tm _t;

    _t.tm_sec = Second;
    _t.tm_min = Minute;
    _t.tm_hour = _12hMode ? (Hour % 12) + (_isPM ? 12 : 0) : Hour; // convert to 0-23
    _t.tm_mday = Day;
    _t.tm_wday = Weekday;
    _t.tm_mon = Month - 1;
    _t.tm_year = Year - 1900;
    _t.tm_isdst = -1;

    return (uint32_t)(mktime(&_t));
}

/**
 * @brief                   Reads time and date from the RTC
 */
void RTC::getRtcData()
{
    updateTime();
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current seconds
 */
uint8_t RTC::getSecond()
{
    updateTime();
    return Second;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current minutes
 */
uint8_t RTC::getMinute()
{
    updateTime();
    return Minute;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current hours
 */
uint8_t RTC::getHour()
{
    updateTime();
    return Hour;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcDay
 */
uint8_t RTC::getDay()
{
    updateTime();
    return Day;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcWeekday
 */
uint8_t RTC::getWeekday()
{
    updateTime();
    return Weekday;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcMonth
 */
uint8_t RTC::getMonth()
{
    updateTime();
    return Month;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current rtcYear
 */
uint16_t RTC::getYear()
{
    updateTime();
    return Year;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns hour status (AM or PM)
 */
bool RTC::isPM()
{
    updateTime();
    return _isPM;
}

/**
 * @brief                   Enables the alarm of the RTC
 */
void RTC::enableAlarm() // datasheet 8.5.6.
{
    // check Table 2. Control_2
    Control2 = RTC_CTRL_2_DEFAULT | RTC_ALARM_AIE; // enable interrupt
    Control2 &= ~RTC_ALARM_AF;                     // clear alarm flag

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(Control2);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                       Sets the alarm to all the params
 *
 * @param uint8_t AlarmSecond  Set the alarm seconds
 * @param uint8_t AlarmMinute  Set the alarm minutes
 * @param uint8_t AlarmHour    Set the alarm hours
 * @param uint8_t AlarmDay     Set the alarm rtcDay
 * @param uint8_t AlarmWeekday Set the alarm rtcWeekday
 */
void RTC::setAlarm(uint8_t AlarmSecond, uint8_t AlarmMinute, uint8_t AlarmHour, uint8_t AlarmDay, uint8_t AlarmWeekday)
{
    if (AlarmSecond < 99)
    { // rtcSecond
        AlarmSecond = constrain(AlarmSecond, 0, 59);
        AlarmSecond = decToBcd(AlarmSecond);
        AlarmSecond &= ~RTC_ALARM;
    }
    else
    {
        AlarmSecond = 0x0;
        AlarmSecond |= RTC_ALARM;
    }

    if (AlarmMinute < 99)
    { // rtcMinute
        AlarmMinute = constrain(AlarmMinute, 0, 59);
        AlarmMinute = decToBcd(AlarmMinute);
        AlarmMinute &= ~RTC_ALARM;
    }
    else
    {
        AlarmMinute = 0x0;
        AlarmMinute |= RTC_ALARM;
    }

    // handle hour format conversion
    if (AlarmHour < 99)
    {
        if (_12hMode && AlarmHour > 12) // user passed 24h value but is using 12h mode
        {
            AlarmHour = AlarmHour - 12;
            AlarmHour = decToBcd(AlarmHour);
            AlarmHour |= (1 << 5); // set PM bit
            AlarmHour &= ~RTC_ALARM;
        }
        else if (_12hMode && AlarmHour == 0) // midnight edge case
        {
            AlarmHour = 12;
            AlarmHour = decToBcd(AlarmHour);
            AlarmHour &= ~RTC_ALARM; // AM, no PM bit
        }
        else
        {
            AlarmHour = constrain(AlarmHour, 0, 23);
            AlarmHour = decToBcd(AlarmHour);
            AlarmHour &= ~RTC_ALARM;
        }
    }
    else
    {
        AlarmHour = 0x0;
        AlarmHour |= RTC_ALARM;
    }

    if (AlarmDay < 99)
    { // rtcDay
        AlarmDay = constrain(AlarmDay, 1, 31);
        AlarmDay = decToBcd(AlarmDay);
        AlarmDay &= ~RTC_ALARM;
    }
    else
    {
        AlarmDay = 0x0;
        AlarmDay |= RTC_ALARM;
    }

    if (AlarmWeekday < 99)
    { // rtcWeekday
        AlarmWeekday = constrain(AlarmWeekday, 0, 6);
        AlarmWeekday = decToBcd(AlarmWeekday);
        AlarmWeekday &= ~RTC_ALARM;
    }
    else
    {
        AlarmWeekday = 0x0;
        AlarmWeekday |= RTC_ALARM;
    }

    enableAlarm();

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM);
    Wire.write(AlarmSecond);
    Wire.write(AlarmMinute);
    Wire.write(AlarmHour);
    Wire.write(AlarmDay);
    Wire.write(AlarmWeekday);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Set alarm using epoch
 *
 * @param uint32_t _epoch   RTC Epoch alarm
 * @param uint8_t _match    RTC Match
 */
void RTC::setAlarmEpoch(uint32_t _epoch, uint8_t _match)
{
    struct tm _t;
    time_t _e = _epoch;

    memcpy(&_t, localtime((const time_t *)&_e), sizeof(_t));

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM);
    Wire.write(decToBcd(_t.tm_sec) & (~((_match & 1) << 7)));
    Wire.write(decToBcd(_t.tm_min) & (~(((_match >> 1) & 1) << 7)));
    Wire.write(decToBcd(_t.tm_hour) & (~(((_match >> 2) & 1) << 7)));
    Wire.write(decToBcd(_t.tm_mday) & (~(((_match >> 3) & 1) << 7)));
    Wire.write(decToBcd(_t.tm_wday) & (~(((_match >> 4) & 1) << 7)));
    Wire.endTransmission();
    i2cEnd();

    enableAlarm();
}


/**
 * @brief                   Reads the alarm of the RTC
 */
void RTC::readAlarm()
{
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ALARM); // datasheet 8.4.
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 5);

    while (Wire.available())
    {
        AlarmSecond = Wire.read();   // read RTC_SECOND_ALARM register
        if (RTC_ALARM & AlarmSecond) // check is AEN = 1 (rtcSecond alarm disabled)
        {
            AlarmSecond = 99; // using 99 as code for no alarm
        }
        else
        {                                                     // else if AEN = 0 (rtcSecond alarm enabled)
            AlarmSecond = bcdToDec(AlarmSecond & ~RTC_ALARM); // remove AEN flag and convert to dec number
        }

        AlarmMinute = Wire.read(); // rtcMinute
        if (RTC_ALARM & AlarmMinute)
        {
            AlarmMinute = 99;
        }
        else
        {
            AlarmMinute = bcdToDec(AlarmMinute & ~RTC_ALARM);
        }

        AlarmHour = Wire.read(); // rtcHour
        if (RTC_ALARM & AlarmHour)
        {
            AlarmHour = 99;
        }
        else
        {
            AlarmHour = bcdToDec(AlarmHour & 0x3F); // remove bits 7 & 6
        }

        AlarmDay = Wire.read(); // rtcDay
        if (RTC_ALARM & AlarmDay)
        {
            AlarmDay = 99;
        }
        else
        {
            AlarmDay = bcdToDec(AlarmDay & 0x3F); // remove bits 7 & 6
        }

        AlarmWeekday = Wire.read(); // rtcWeekday
        if (RTC_ALARM & AlarmWeekday)
        {
            AlarmWeekday = 99;
        }
        else
        {
            AlarmWeekday = bcdToDec(AlarmWeekday & 0x07); // remove bits 7,6,5,4 & 3
        }
    }
    i2cEnd();
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm seconds
 */
uint8_t RTC::getAlarmSecond()
{
    readAlarm();
    return AlarmSecond;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm minutes
 */
uint8_t RTC::getAlarmMinute()
{
    readAlarm();
    return AlarmMinute;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm hours
 */
uint8_t RTC::getAlarmHour()
{
    readAlarm();
    return AlarmHour;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm rtcDay
 */
uint8_t RTC::getAlarmDay()
{
    readAlarm();
    return AlarmDay;
}

/**
 * @brief                   Small user method
 *
 * @returns uint8_t         Returns the current alarm rtcWeekday
 */
uint8_t RTC::getAlarmWeekday()
{
    readAlarm();
    return AlarmWeekday;
}

/**
 * @brief                   Sets the timer countdown
 *
 * @param                   rtcCountdownSrcClock source_clock
 *                          timer clock frequency
 *
 * @param                   uint8_t value
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
void RTC::timerSet(rtcCountdownSrcClock source_clock, uint8_t value, bool int_enable, bool int_pulse)
{
    uint8_t timer_reg[2] = {0};

    // disable the countdown timer
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.write(0x18); // default
    Wire.endTransmission();
    i2cEnd();

    // clear Control_2
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(0x00); // default
    Wire.endTransmission();
    i2cEnd();

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
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_VAL);
    Wire.write(timer_reg[0]);
    Wire.write(timer_reg[1]);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Returns is the timer flag on
 *
 * @returns bool            Returns true if the timer flag is on
 */
bool RTC::checkTimerFlag()
{
    uint8_t _crtl_2 = RTC_TIMER_FLAG;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    _crtl_2 &= Wire.read();
    i2cEnd();

    return _crtl_2;
}

/**
 * @brief                   Returns is the alarm flag on
 *
 * @returns bool            Returns true if the alarm flag is on
 */
bool RTC::checkAlarmFlag()
{
    uint8_t _crtl_2 = RTC_ALARM_AF;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    _crtl_2 &= Wire.read();
    i2cEnd();

    return _crtl_2;
}

/**
 * @brief                   Clears alarm flag
 */
void RTC::clearAlarmFlag()
{
    uint8_t _crtl_2;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _crtl_2 = Wire.read() & ~(RTC_ALARM_AF);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(_crtl_2);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Clears timer flag
 */
void RTC::clearTimerFlag()
{
    uint8_t _crtl_2;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _crtl_2 = Wire.read() & ~(RTC_TIMER_FLAG);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_2);
    Wire.write(_crtl_2);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Disables the timer
 */
void RTC::disableTimer()
{
    uint8_t _timerMode;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);

    _timerMode = Wire.read() & ~(RTC_TIMER_TE);

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_TIMER_MODE);
    Wire.write(_timerMode);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Toggles RTC time format between 24H and 12H
 *
 * @returns bool            Returns false for 24H format, true for 12H
 */
bool RTC::changeTimeFormat()
{
    uint8_t reg;

    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    reg = Wire.read();

    _12hMode = !_12hMode;

    if (_12hMode)
        reg |= (1 << 3); // set 12_24 bit
    else
        reg &= ~(1 << 3); // clear 12_24 bit

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.write(reg);
    Wire.endTransmission();
    i2cEnd();

    return _12hMode;
}

/**
 * @brief                   Check if the RTC is already set
 *
 * @returns bool            Returns true if RTC is set, false if it's not
 */
bool RTC::isSet()
{
    uint8_t _ramByte;
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_RAM_by);
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 1);
    _ramByte = Wire.read();
    i2cEnd();
    return _ramByte == 170;
}

/**
 * @brief                   Resets the timer
 */
void RTC::reset() // datasheet 8.2.1.3.
{
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_CTRL_1);
    Wire.write(RTC_CTRL_1_DEFAULT);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief                   Set internal capacitor value.
 *
 * @param bool val          0 or 1 which represents 7pF or 12.5 pF.
 */
void RTC::setInternalCapacitor(bool val)
{
    i2cStart();
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
    i2cEnd();
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
void RTC::setClockOffset(bool mode, int offsetValue)
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
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_OFFSET);
    Wire.write(regValue);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief  Reads raw time/date registers from RTC via I2C into member variables
 */
void RTC::updateTime()
{
    i2cStart();
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(RTC_SECOND_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 7);

    Second = bcdToDec(Wire.read() & 0x7F);
    Minute = bcdToDec(Wire.read() & 0x7F);

    uint8_t hourReg = Wire.read();
    if (_12hMode)
    {
        // bit 5 defines the time format
        _isPM = (hourReg >> 5) & 0x01;
        Hour = bcdToDec(hourReg & 0x1F);
    }
    else
    {
        Hour = bcdToDec(hourReg & 0x3F);
    }

    Day = bcdToDec(Wire.read() & 0x3F);
    Weekday = bcdToDec(Wire.read() & 0x07);
    Month = bcdToDec(Wire.read() & 0x1F);
    Year = bcdToDec(Wire.read()) + 2000;
    i2cEnd();
}

/**
 * @brief                   Converts decimal to BCD
 *
 * @param                   uint8_t val
 *                          number which needs to be converted from decimal to Bcd value
 */
uint8_t RTC::decToBcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

/**
 * @brief                   Converts BCD to decimal
 *
 * @param                   uint8_t val
 *                          number which needs to be converted from Bcd to decimal value
 */
uint8_t RTC::bcdToDec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}
