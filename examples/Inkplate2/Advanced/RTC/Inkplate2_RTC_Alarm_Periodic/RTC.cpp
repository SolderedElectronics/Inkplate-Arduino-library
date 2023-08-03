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
 * @brief                         Function to set wake up timer. The function adds a wake-up 
 *                                period on the current time and sets the wake-up time at this time.
 *                                
 * @param int wakeHours           Hours for periodic wake-up.
 * 
 * @param int wakeMinutes         Minutes for periodic wake-up.
 * 
 * @param struct tm currentTime   Struct of the current time.
 * 
 * @return                        None.
 */
void RTC::setWakeUpTimer(int wakeHours, int wakeMinutes, struct tm currentTime)
{
    struct tm wakePeriod;

    // Calculate time until the wake
    wakePeriod = currentTime;
    wakePeriod.tm_hour += wakeHours;
    wakePeriod.tm_min += wakeMinutes;
    int timeUntilWakeInSeconds = difftime(mktime(&wakePeriod), mktime(&currentTime));


    // Set how much the internal RTC should wait before waking the device up
    // It's in uS, so to get seconds we need to * 1000000
    // Note: The device still has to be powered during sleep time to wake up
    esp_sleep_enable_timer_wakeup(1000000LL * timeUntilWakeInSeconds);
}

/**
 * @brief                         This function returns time in seconds until the alarm. 
 *                                If the time is smaller than 0, the alarm time is reached, 
 *                                if it is greater, it needs to wait for the alarm.
 * 
 * @param int alarmHour           Hours of alarm.
 * 
 * @param int alarmMins           Minutes of alarm.
 * 
 * @param int alarmSecs           Seconds of alarm.
 * 
 * @param int alarmDay            Day of alarm.
 * 
 * @param int alarmMon            Month of alarm.
 * 
 * @param struct tm currentTime   Struct of the current time.
 * 
 * @return                        Time from now until alarm in seconds
 */
int RTC::secondsUntilAlarm(int alarmHour, int alarmMins, int alarmSecs, int alarmDay, int alarmMon, struct tm currentTime)
{
    struct tm timerTime;

    // Copy currentTime and set the correct hours, mins and date
    timerTime = currentTime;
    timerTime.tm_hour = alarmHour;
    timerTime.tm_min = alarmMins;
    timerTime.tm_sec = alarmSecs;
    timerTime.tm_mday = alarmDay;
    timerTime.tm_mon = alarmMon - 1; // Months are zero indexed

    // Calculate the difference between times and return it
    return difftime(mktime(&timerTime), mktime(&currentTime));
}
