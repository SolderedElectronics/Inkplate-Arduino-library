/**
 * @file                          RTC.cpp
 * 
 * @brief                         Functions that emulate RTC.
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

#include "RTC.h"

/**
 * @brief                         The function that emulates setting alarm like on RTC with real alarm function.
 * 
 * @param ALARM_TIME alarmTime    The struct where is stored time for the alarm. 
 * 
 * @param rtcMatch match          RTC matching. Defined in RTC.h
 * 
 * @return                        Time from now until alarm in seconds.
 */
double RTC::setAlarm(ALARM_TIME alarmTime, rtcMatch match)
{
    struct tm timerTime, currentTime; // memset napraviti
    memset(&timerTime, 0, sizeof(struct tm));
    double timeUntilAlarmInSeconds;

    // Get current time from esp RTC
    time_t now = time(nullptr)  + timezone * 3600ULL;
    gmtime_r(&now, &currentTime);

    timerTime = currentTime;
    timerTime.tm_hour = (match & (1 << 2)) ? alarmTime.hour : 0;
    currentTime.tm_hour = (match & (1 << 2)) ? currentTime.tm_hour : 0;

    timerTime.tm_min = (match & (1 << 1)) ? alarmTime.mins : 0;
    currentTime.tm_min = (match & (1 << 1)) ? currentTime.tm_min : 0;

    timerTime.tm_sec = (match & (1 << 0)) ? alarmTime.secs : 0;
    currentTime.tm_sec = (match & (1 << 0)) ? currentTime.tm_sec : 0;

    timerTime.tm_mday = (match & (1 << 3)) ? alarmTime.day : 0;
    currentTime.tm_mday = (match & (1 << 3)) ? currentTime.tm_mday : 0;

    timerTime.tm_mon = (match & (1 << 4)) ? alarmTime.mon - 1 : 0; // Months are zero indexed
    currentTime.tm_mon = (match & (1 << 4)) ? currentTime.tm_mon - 1 : 0;

    timeUntilAlarmInSeconds = difftime(mktime(&timerTime), mktime(&currentTime));

    Serial.println(timeUntilAlarmInSeconds);

    // Set alarm time can't be in the past!
    if (timeUntilAlarmInSeconds > 0)
    {
        // Enable timer for wakeup
        // It's in uS, so * 1000000 to convert to seconds
        // Note: The device still has to be powered during sleep time to wake up
        esp_sleep_enable_timer_wakeup(timeUntilAlarmInSeconds * 1000000LL);
        return timeUntilAlarmInSeconds;
    }
    return -1;
}

/**
 * @brief                           Set timezone for RTC. The function to set the alarm takes the current time 
 *                                  without the time zone, so this function stores the time zone in the RTC class 
 *                                  so that when setting the alarm, the time zone will be added and the alarm will 
 *                                  be set correctly.
 *                                  
 * @param int timeZone              Time zone in hours.
 * 
 * @return                          None.
 */
void RTC::setTimezone(int timeZone)
{
  timezone = timeZone;
}
