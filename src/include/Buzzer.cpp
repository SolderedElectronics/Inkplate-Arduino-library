/**
 **************************************************
 *
 * @file        Buzzer.cpp
 * @brief       Functions file for controlling Inkplate's buzzer.
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Robert @ Soldered
 ***************************************************/

#include "Buzzer.h"
#include "defines.h"

/**
 * @brief       Initiate the digipot which the buzzer uses to change pitch
 *
 * @returns     None
 */
void Buzzer::begin()
{
    digipot.begin(DIGIPOT_ADDR);
}

/**
 * @brief       Beep for a length of time
 *
 * @param       uint32_t length - the length of the beep in ms
 *
 * @note        This is a blocking function, it will exit only when beeping is done
 *
 * @returns     none
 */
void Buzzer::beep(uint32_t length)
{
    // Turn on the buzzer and wait for the length of time before turning it off
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, LOW);
    delay(length);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, HIGH);
}