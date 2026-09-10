/**
 **************************************************
 * @file        Frontlight.h
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
#ifndef __FRONTLIGHT_H__
#define __FRONTLIGHT_H__

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATE4TEMPERA) ||         \
    defined(ARDUINO_INKPLATE6FLICK)

#include "Arduino.h"
#include "Wire.h"

class Inkplate;

/**
 * @brief       Frontlight class designed to work with inkplates that support
 * frontlight (Inkplate 6 plus and Inkplate 4TEMPERA)
 */
class Frontlight
{
  public:
    void setBrightness(uint8_t _v);
    void setState(bool _e);
    void begin(Inkplate *inkplatePtr);

  private:
    Inkplate *_inkplate;
};

#endif

#endif