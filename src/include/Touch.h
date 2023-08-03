/**
 **************************************************
 * @file        Touch.h
 * @brief       Touch screen functionality for panels that support touch
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
 * @authors     @ Soldered
 ***************************************************/

#ifndef __TOUCH_H__
#define __TOUCH_H__

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATEPLUS2)

#include "Arduino.h"
#include "Graphics.h"
#include "Wire.h"
#include "defines.h"

#if defined(ARDUINO_INKPLATE6PLUS)
#include "Mcp.h"
#elif defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATEPLUS2)
#include "Pcal.h"
#endif

/**
 * @brief       Touch class holds functionality for interaction with touchscreen
 * displays
 */
class Touch : virtual public Expander
{
  public:
    bool touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h);

    bool tsInit(uint8_t _pwrState);
    void tsShutdown();
    bool tsAvailable();
    void tsSetPowerState(uint8_t _s);
    uint8_t tsGetPowerState();
    uint8_t tsGetData(uint16_t *xPos, uint16_t *yPos);

    virtual int getRotation() = 0;

    void tsGetRawData(uint8_t *b);

  private:
    const char hello_packet[4] = {0x55, 0x55, 0x55, 0x55};

    uint8_t tsWriteRegs(uint8_t _addr, const uint8_t *_buff, uint8_t _size);
    void tsReadRegs(uint8_t _addr, uint8_t *_buff, uint8_t _size);
    void tsHardwareReset();
    bool tsSoftwareReset();
    void tsGetXY(uint8_t *_d, uint16_t *x, uint16_t *y);
    void tsGetResolution(uint16_t *xRes, uint16_t *yRes);

    uint8_t touchN;
    uint16_t touchX[2], touchY[2];
    uint32_t touchT = 0;
};

#endif

#endif