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

#ifndef __RTC_H__
#define __RTC_H__

// All functions defined in RTC.cpp

class RTC
{
  public:
    void setWakeUpTimer(int wakeHours, int wakeMinutes, struct tm currentTime);
    int secondsUntilAlarm(int alarmHour, int alarmMins, int alarmSecs, int alarmDay, int alarmMon, struct tm currentTime);
};

#endif
