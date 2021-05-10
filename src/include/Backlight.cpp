/**
 **************************************************
 * @file        Backlight.cpp
 * @brief       Basic funtions for controling inkplate backlight
 * 
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example.
 *              If you have any questions about licensing, please contact techsupport@e-radionica.com
 *              Distributed as-is; no warranty is given.
 * 
 * @authors     e-radionica.com
 ***************************************************/

#include "Backlight.h"
#include "defines.h"

#ifdef ARDUINO_INKPLATE6PLUS

/**
 * @brief       setBacklight function sets backlight intensity for inkplate
 * 
 * @param       uint8_t _v
 *              value to set backlight to
 * 
 * @note        can only be used in inkplate 6PLUS, others don't suport backlight
 */
void Backlight::setBacklight(uint8_t _v)
{
    Wire.beginTransmission(0x5C >> 1);
    Wire.write(0);
    Wire.write(63 - (_v & 0b00111111));
    Wire.endTransmission();
}

/**
 * @brief       backlight function turns backlight on/off
 * 
 * @param       bool _e
 *              enable value, 1 turns on, 0 off
 */
void Backlight::backlight(bool _e)
{
    if (_e)
    {
        pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, OUTPUT);
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, HIGH);
    }
    else
    {
        pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, OUTPUT);
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, LOW);
    }
}

#endif