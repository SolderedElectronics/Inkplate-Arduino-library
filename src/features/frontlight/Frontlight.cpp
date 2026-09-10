/**
 **************************************************
 * @file        Frontlight.cpp
 * @brief       Basic funtions for controling inkplate frontlight
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#include "Frontlight.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATE4TEMPERA) ||         \
    defined(ARDUINO_INKPLATE6FLICK)

/**
 * @brief       setFrontlight function sets frontlight intensity for inkplate
 *
 * @param       uint8_t _v
 *              value to set frontlight to
 *
 * @note        can only be used in inkplate 6PLUS and 4TEMPERA, others don't suport
 * frontlight
 */
void Frontlight::setBrightness(uint8_t _v)
{
    i2cStart();
    Wire.beginTransmission(0x5C >> 1);
    Wire.write(0);
    Wire.write(63 - (_v & 0b00111111));
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief       setState function turns frontlight on/off
 *
 * @param       bool _e
 *              enable value, 1 turns on, 0 off
 */
void Frontlight::setState(bool _e)
{
    if (_e)
    {
        _inkplate->expander1.digitalWrite(FRONTLIGHT_EN, HIGH);
    }
    else
    {
        _inkplate->expander1.digitalWrite(FRONTLIGHT_EN, LOW);
    }
}

/**
 * @brief       begin function forwards the current inkplate instance to be used inside the class
 *
 * @param       Inkplate* inkplatePtr
 *              pointer to the instance of the current Inkplate display object
 */
void Frontlight::begin(Inkplate *inkplatePtr)
{
    _inkplate = inkplatePtr;
}

#endif