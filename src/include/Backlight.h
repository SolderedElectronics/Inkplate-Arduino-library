/**
 **************************************************
 * @file        Backlight.h
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
#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#ifdef ARDUINO_INKPLATE6PLUS

#include "Arduino.h"
#include "Mcp.h"
#include "Wire.h"
#include "defines.h"

/**
 * @brief       Backlight class designed to work with inkplates that support backlight (Inkplate 6 plus)
 */
class Backlight : virtual public Mcp
{
  public:
    void setBacklight(uint8_t _v);
    void backlight(bool _e);
};

#endif

#endif