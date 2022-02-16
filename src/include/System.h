/**
 **************************************************
 * @file        System.h
 * @brief       Functionality for panel hardware interaction
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *              License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *              LICENSE file included with this example. If you have any questions about
 *              licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *              warranty is given.
 *
 * @authors     @ e-radionica.com
 ***************************************************/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "../libs/SdFat/SdFat.h"
#include "Arduino.h"
#include "SPI.h"
#include "time.h"

#include "Esp.h"
#include "NetworkClient.h"

#define I2C_ADDR 0x51

// registar overview - crtl & status reg
#define RTC_CTRL_1 0x0
#define RTC_CTRL_2 0x01
#define RTC_OFFSET 0x02
#define RTC_RAM_by 0x03
// registar overview - time & data reg
#define RTC_SECOND_ADDR 0x04
#define RTC_MINUTE_ADDR 0x05
#define RTC_HOUR_ADDR   0x06
#define RTC_DAY_ADDR    0x07
#define RTC_WDAY_ADDR   0x08
#define RTC_MONTH_ADDR  0x09
// years 0-99; calculate real year = 2000 + RCC reg year
#define RTC_YEAR_ADDR 0x0A
// registar overview - alarm reg
#define RTC_SECOND_ALARM 0x0B
#define RTC_MINUTE_ALARM 0x0C
#define RTC_HOUR_ALARM   0x0D
#define RTC_DAY_ALARM    0x0E
#define RTC_WDAY_ALARM   0x0F
// registar overview - timer reg
#define RTC_TIMER_VAL   0x10
#define RTC_TIMER_MODE  0x11
#define RTC_TIMER_TCF   0x08
#define RTC_TIMER_TE    0x04
#define RTC_TIMER_TIE   0x02
#define RTC_TIMER_TI_TP 0x01
// format
// set AEN_x registers
#define RTC_ALARM 0x80
// set AIE ; enable/disable interrupt output pin
#define RTC_ALARM_AIE 0x80
// set AF register ; alarm flag needs to be cleared for alarm
#define RTC_ALARM_AF       0x40
#define RTC_CTRL_2_DEFAULT 0x00
#define RTC_TIMER_FLAG     0x08

#define RTC_ALARM_MATCH_SS      0b00000001
#define RTC_ALARM_MATCH_MMSS    0b00000011
#define RTC_ALARM_MATCH_HHMMSS  0b00000111
#define RTC_ALARM_MATCH_DHHMMSS 0b00001111
#define RTC_ALARM_MATCH_WHHMSS  0b00011111

#ifdef ARDUINO_INKPLATE6PLUS
#include "Frontlight.h"
#include "Touch.h"
#endif

#ifndef ARDUINO_INKPLATE2
#include "Mcp.h"
#endif

#include "defines.h"

/**
 * @brief       System class for interaction with panel harware
 */
class System : public Esp,

#ifndef ARDUINO_INKPLATE2
               virtual public Mcp,
#endif

               virtual public NetworkClient,

#ifdef ARDUINO_INKPLATE6PLUS
               public Touch,
               public Frontlight
#endif


{
  public:
    void setPanelState(uint8_t s);
    uint8_t getPanelState();

#ifndef ARDUINO_INKPLATE2

    void setSdCardOk(int16_t s);
    int16_t getSdCardOk();

    int8_t readTemperature();

    uint8_t readTouchpad(uint8_t _pad);
    double readBattery();

    int16_t sdCardInit();

    SdFat getSdFat();
    SPIClass *getSPIptr();

    virtual int _getRotation() = 0; // required in Touch
    int getRotation()
    {
        return _getRotation();
    };

    enum rtcCountdownSrcClock
    {
        TIMER_CLOCK_4096HZ = 0,
        TIMER_CLOCK_64HZ = 1,
        TIMER_CLOCK_1HZ = 2,
        TIMER_CLOCK_1PER60HZ = 3
    };

    void rtcBegin();

    void rtcSetTime(uint8_t hour, uint8_t minute, uint8_t sec);
    void rtcSetDate(uint8_t weekday, uint8_t day, uint8_t month, uint16_t yr);
    void rtcSetEpoch(uint32_t _epoch);
    uint32_t rtcGetEpoch();
    void rtcGetRtcData();
    void rtcEnableAlarm(); // called on setAlarm()
    void rtcSetAlarm(uint8_t alarm_second, uint8_t alarm_minute, uint8_t alarm_hour, uint8_t alarm_day,
                     uint8_t alarm_weekday);
    void rtcSetAlarmEpoch(uint32_t _epoch, uint8_t _match);
    void rtcReadAlarm();
    bool rtcChangeTimeFormat(); // returns false for 24H format & true for 12H format
    void rtcTimerSet(rtcCountdownSrcClock source_clock, uint8_t value, bool int_enable, bool int_pulse);
    bool rtcCheckTimerFlag();
    bool rtcCheckAlarmFlag();
    void rtcClearAlarmFlag();
    void rtcClearTimerFlag();
    void rtcDisableTimer();
    bool rtcIsSet();
    void rtcReset();
    /* read RTC times */
    uint8_t rtcGetSecond();
    uint8_t rtcGetMinute();
    uint8_t rtcGetHour();
    uint8_t rtcGetDay();
    uint8_t rtcGetWeekday();
    uint8_t rtcGetMonth();
    uint16_t rtcGetYear();
    /* read alarm times */ // if return = 99, no alarm
    uint8_t rtcGetAlarmSecond();
    uint8_t rtcGetAlarmMinute();
    uint8_t rtcGetAlarmHour();
    uint8_t rtcGetAlarmDay();
    uint8_t rtcGetAlarmWeekday();

#endif
  private:
    uint8_t rtcDecToBcd(uint8_t val);
    uint8_t rtcBcdToDec(uint8_t val);
    /* alarm */
    uint8_t rtcAlarmSecond;
    uint8_t rtcAlarmMinute;
    uint8_t rtcAlarmHour;
    uint8_t rtcAlarmDay;
    uint8_t rtcAlarmWeekday;
    /* support */
    uint8_t rtcControl2;
    uint8_t _panelOn = 0;
    int16_t _sdCardOk = 0;
    /* time variables*/
    uint8_t rtcHour;
    uint8_t rtcMinute;
    uint8_t rtcSecond;
    uint8_t rtcDay;
    uint8_t rtcWeekday;
    uint8_t rtcMonth;
    uint16_t rtcYear;
};

#endif