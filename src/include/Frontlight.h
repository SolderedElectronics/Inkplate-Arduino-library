/**
 **************************************************
 * @file        Frontlight.h
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
#ifndef __FRONTLIGHT_H__
#define __FRONTLIGHT_H__

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATEPLUS2)

#include "Arduino.h"
#include "Wire.h"
#include "defines.h"

#if defined(ARDUINO_INKPLATE6PLUS)
#include "Mcp.h"
#elif defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATEPLUS2)
#include "Pcal.h"
#endif

/**
 * @brief       Frontlight class designed to work with inkplates that support
 * frontlight (Inkplate 6 plus and Inkplate PLUS2)
 */
class Frontlight : virtual public Expander
{
  public:
    void setFrontlight(uint8_t _v);
    void frontlight(bool _e);
};

#endif

#endif