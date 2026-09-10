/**
 **************************************************
 * @file        TouchElan.h
 * @brief       Touch screen functionality for panels that support touch
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#ifndef __TOUCH_ELAN_H__
#define __TOUCH_ELAN_H__

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATE4TEMPERA)

// Include Arduino main header file.
#include "Arduino.h"

// Include Arduino Wire library (for I2C communication with touchscreen driver).
#include "Wire.h"

// Include board specific defines.
#include "../../../system/defines.h"

class Inkplate;

/**
 * @brief       Touch class holds functionality for interaction with touchscreen
 * displays
 */
class Touch
{
  public:
    bool touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h);
    void begin(Inkplate *inkplatePtr);
    bool init(uint8_t _pwrState);
    void shutdown();
    bool available();
    void setPowerState(uint8_t _s);
    uint8_t getPowerState();
    uint8_t getData(uint16_t *xPos, uint16_t *yPos);
    void getRawData(uint8_t *b);

  private:
    const char hello_packet[4] = {0x55, 0x55, 0x55, 0x55};

    uint8_t tsWriteRegs(uint8_t _addr, const uint8_t *_buff, uint8_t _size);
    void tsReadRegs(uint8_t _addr, uint8_t *_buff, uint8_t _size);
    void power(bool _pwr);
    void tsHardwareReset();
    bool tsSoftwareReset();
    void tsGetXY(uint8_t *_d, uint16_t *x, uint16_t *y);
    void tsGetResolution(uint16_t *xRes, uint16_t *yRes);
    void end();

    uint8_t touchN;
    uint16_t touchX[2], touchY[2];
    uint32_t touchT = 0;
    bool _tsInitDone = false;

    Inkplate *_inkplate;
};

#endif

#endif