/**
 **************************************************
 * @file        Frontlight.cpp
 * @brief       Basic funtions for controling inkplate frontlight
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#include "Frontlight.h"
#include "defines.h"

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATE4TEMPERA) || defined(ARDUINO_INKPLATE6FLICK)

/**
 * @brief       setFrontlight function sets frontlight intensity for inkplate
 *
 * @param       uint8_t _v
 *              value to set frontlight to
 *
 * @note        can only be used in inkplate 6PLUS and 4TEMPERA, others don't suport
 * frontlight
 */
void Frontlight::setFrontlight(uint8_t _v)
{
    Wire.beginTransmission(0x5C >> 1);
    Wire.write(0);
    Wire.write(63 - (_v & 0b00111111));
    Wire.endTransmission();
}

/**
 * @brief       frontlight function turns frontlight on/off
 *
 * @param       bool _e
 *              enable value, 1 turns on, 0 off
 */
void Frontlight::frontlight(bool _e)
{
    if (_e)
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, FRONTLIGHT_EN, HIGH);
    }
    else
    {
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, FRONTLIGHT_EN, LOW);
    }
}

#endif