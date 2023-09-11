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

#ifdef ARDUINO_INKPLATEPLUS2

/**
 * @brief       Initiate the digipot which the buzzer uses to change pitch
 *
 * @returns     None
 */
void Buzzer::begin()
{
    // Init the pin for enabling the power to the buzzer
    pinModeInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, OUTPUT);
}

/**
 * @brief       Beep for a length of time
 *
 * @param       uint32_t length - the length of the beep in ms
 *              uint8_t freq - the frequency, from 0 to 100
 *
 * @note        This is a blocking function, it will exit only when beeping is done
 *
 * @returns     none
 */
void Buzzer::beep(uint32_t length, uint8_t freq)
{
    // Turn on the buzzer and wait for the length of time before turning it off
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, LOW);
    // Set the frequency
    setFrequencyInternal(freq);
    // Wait for the beep to be done
    delay(length);
    // Turn everything off
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, HIGH);
}

/**
 * @brief       Turn on the buzzer indefinitely
 *
 * @param       uint8_t freq - the frequency, from 0 to 100
 *
 * @returns     none
 */
void Buzzer::beepOn(uint8_t freq)
{
    // Turn on the buzzer
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, LOW);

    // Set the frequency
    setFrequencyInternal(freq);
}

/**
 * @brief       Turn off the buzzer
 *
 * @returns     none
 */
void Buzzer::beepOff()
{
    // Turn off the buzzer
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, HIGH);
}

/**
 * @brief       Set the frequency by setting the wiper percentage of the digipot
 *
 * @param       uint8_t freq - the frequency, from 0 to 100
 *
 * @returns     none
 */
void Buzzer::setFrequencyInternal(int freq)
{
    // Make sure the value is in the 0-100 range
    if (freq > 100 || freq < 0)
        return;

    // Init the digipot
    // It needs to be powered on from before for this to have effect
    // For that reason, this function is internal to this driver only
    digipot.begin(DIGIPOT_ADDR);
    
    // Set the resistor's value
    // Inverse, so that 0 is the lowest pitch and 100 the highest
    digipot.setWiperPercent(100 - freq);
}

#endif