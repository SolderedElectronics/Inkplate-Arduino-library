/**
 * @file                          RTC.h
 * 
 * @brief                         Header file for RTC.
 *                                Inkplate Arduino library
 *                                Karlo Leksic for Soldered.com
 *                                January 5, 2023
 *                                https://github.com/e-radionicacom/Inkplate-6-Arduino-library
 *                                
 *  For support, please reach over forums: forum.e-radionica.com/en
 *  For more info about the product, please check: www.inkplate.io
 *  
 *  This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *  Please review the LICENSE file included with this example.
 *  If you have any questions about licensing, please contact techsupport@e-radionica.com
 *  Distributed as-is; no warranty is given.
 */

#include "Arduino.h"
#include "time.h"

// RTC matching possibility
enum rtcMatch
{
    RTC_SS = 0b00000001, 
    RTC_MMSS = 0b00000011,
    RTC_HHMMSS = 0b00000111,
    RTC_DHHMMSS = 0b00001111,
    RTC_MDHHMMSS = 0b00011111
};

#ifndef __RTC_H__
#define __RTC_H__

// All functions defined in RTC.cpp

class RTC
{
  public:
    double setAlarm(struct tm alarmTime, rtcMatch match);
    double setAlarmEpoch(time_t alarmTimeEpoch, rtcMatch match);
    void setTimezone(int timeZone);

    private:
      int timezone = 0;
};

#endif
