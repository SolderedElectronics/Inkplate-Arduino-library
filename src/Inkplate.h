/**
 **************************************************
 *
 * @file        Inkplate.h
 * @brief       Basic funtions for controling inkplate
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors    Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE_H__
#define __INKPLATE_H__

#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#include "boards/InkplateBoardFile.h"
#include "graphics/Graphics.h"
#include "system/InkplateBoards.h"
#include "system/NetworkController/NetworkController.h"
#include "system/defines.h"
#include "system/inkplateSemaphore.h"

class Inkplate : public Graphics, public InkplateBoardClass, public NetworkController
{
  public:
#ifdef MULTIPLE_DISPLAY_MODES
    Inkplate(uint8_t mode);
    void preloadScreen();
#else
    Inkplate();
#endif
    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void setRotation(uint8_t r);

    // Bus mutex helpers — use in user FreeRTOS tasks that share I2C or SPI.
    // Always call the matching unlock after the locked section.
    inline void i2cLock()
    {
        i2cStart();
    }
    inline void i2cUnlock()
    {
        i2cEnd();
    }
    inline void spiLock()
    {
        spiStart();
    }
    inline void spiUnlock()
    {
        spiEnd();
    }
    inline void displayLock()
    {
        displayStart();
    }
    inline void displayUnlock()
    {
        displayEnd();
    }


  protected:
  private:
    uint8_t _rotation = 0;
    uint8_t _beginDone = 0;
    uint8_t _mode;
    void writePixel(int16_t x, int16_t y, uint16_t color);
};
#endif