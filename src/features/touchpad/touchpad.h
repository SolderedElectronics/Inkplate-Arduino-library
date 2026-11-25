/**
 **************************************************
 * @file        touchpad.h
 * @brief       Class for interacting with the touchpads on older models of Inkplate devices
 *
 *              Available only for older Inkplate 6, 6COLOR and 10 devices
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *              License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *              LICENSE file included with this example. If you have any questions about
 *              licensing
 *
 * @authors     Soldered
 ***************************************************/
#if defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10)
#pragma once
#include "Arduino.h"

class Inkplate;

class Touchpad
{
  public:
    void begin(Inkplate *inkplateptr);
    uint8_t read(uint8_t pad);

  private:
    Inkplate *_inkplate;
};
#endif