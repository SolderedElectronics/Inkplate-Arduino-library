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

#ifdef ARDUINO_INKPLATE4TEMPERA

/**
 * @brief       Initiate the digipot which the buzzer uses to change pitch
 *
 * @returns     None
 */
void Buzzer::initBuzzer()
{
    // Init the digipot
    digipot.begin();
}

/**
 * @brief       Beep for a length of time
 *
 * @param       uint32_t length - the length of the beep in ms
 *              uint8_t freq - the frequency in Hz
 *
 * @note        This is a blocking function, it will exit only when beeping is done
 *              If the frequency entered is out of range, it will get capped to the range
 *
 * @returns     none
 */
void Buzzer::beep(uint32_t length, int freq)
{
    // Turn on the buzzer at set frequency
    beepOn(freq);

    // Wait for the beep to be done
    delay(length);

    // Turn everything off
    beepOff();
}

/**
 * @brief       Beep for a length of time on the default frequency
 *
 * @param       uint32_t length - the length of the beep in ms
 *
 * @note        This is a blocking function, it will exit only when beeping is done
 *
 * @returns     none
 */
void Buzzer::beep(uint32_t length)
{
    // Turn on the buzzer
    beepOn();

    // Wait for the beep to be done
    delay(length);

    // Turn everything off
    beepOff();
}

/**
 * @brief       Turn on the buzzer indefinitely
 *
 * @param       uint8_t freq - the frequency in Hz
 *
 * @returns     none
 */
void Buzzer::beepOn(int freq)
{
    // Turn on the buzzer
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, LOW);

    // Calculate the Wiper% based on the desired frequency
    int wiperPercent = freqToWiperPercent(freq);

    // Set the frequency
    digipot.setWiperPercent(wiperPercent);
}

/**
 * @brief       Turn on the buzzer indefinitely at default frequency
 *
 * @returns     none
 */
void Buzzer::beepOn()
{
    // Turn on the buzzer
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, LOW);

    // Set the default frequency
    digipot.setWiperPercent(50);
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
 * @brief       Calculate the digipot position to get a certain tone frequency
 *              Approximated by a quadratic regression
 *
 * @param       uint8_t freq - the desired frequency
 *
 * @returns     int of the wiper percent
 */
int Buzzer::freqToWiperPercent(int freq)
{
    // First cap the value to the interval
    int constrainedFreq = constrain(freq, BEEP_FREQ_MIN, BEEP_FREQ_MAX);

    // Calculate the value
    return 156.499576 + (-0.130347337 * constrainedFreq);
}

#endif