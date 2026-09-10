/**
 **************************************************
 *
 * @file        Inkplate6PLUSDriver.h
 * @brief       Low level driver for the Inkplate 6 PLUS e-paper panel
 *
 *              The Inkplate 6 PLUS uses a 6" 1024x758 monochrome e-paper panel driven over a
 *              parallel interface. The control lines go through the on board GPIO expanders
 *              and the TPS65186 PMIC generates the e-paper driving rails - unlike the other
 *              boards, this driver keeps its own power mode handling and talks to the PMIC at
 *              a lower level instead of delegating the whole power up and power down sequence.
 *
 *              This driver covers both the Inkplate 6 PLUS and the Inkplate 6 PLUS V2. The
 *              board carries a capacitive touchscreen and a frontlight, and supports both the
 *              1 bit (partial updates) and 3 bit (8 levels of grey) display modes.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE6FLICKDRIVER_H__
#define __INKPLATE6FLICKDRIVER_H__

// Header guard for the Arduino include
#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 6PLUS"

// Include main header file for the Arduino.
#include "Arduino.h"

#include "EEPROM.h"

// Include library for GPIO expander.
#include "../../system/ExpanderSelect.h"

#include "pins.h"

// Include waveforms for EPD
#include "waveforms.h"

#include "../../graphics/Image/Image.h"
#include "../../graphics/Gif/Gif.h"

#include "Wire.h"

#include "../../graphics/GraphicsDefs.h"

#include "../../features/featureSelect.h"
#include "../../features/TPS65186/TPS65186.h"

#include "../../system/defines.h"


class Inkplate;


class EPDDriver
{
  public:
    void writePixelInternal(int16_t x, int16_t y, uint16_t color);
    int initDriver(Inkplate *_inkplatePtr);

    void display(bool _leaveOn = 0);
    void selectDisplayMode(uint8_t displayMode);
    void clearDisplay();
    uint32_t partialUpdate(bool _forced = false, bool leaveOn = false);
    void setFullUpdateThreshold(uint16_t _numberOfPartialUpdates);
    uint8_t getDisplayMode();
    void setInkplatePowerMode(uint8_t _mode);

    int einkOn();
    void einkOff();


    void setSdCardOk(int16_t s);
    int16_t getSdCardOk();
    int16_t sdCardInit();
    void sdCardSleep();
    SdFat &getSdFat();
    SPIClass *getSPIptr();

    int8_t readTemperature();

    double readBattery();

    void burnInClean(uint8_t clear_cycles, uint16_t cycles_delay);

    bool isPowerGood();

    bool setVCOM(double vcom);
    double getStoredVCOM();
    double getVCOMValue();

    IOExpander expander1;
    IOExpander expander2;

    TPS65186 pmic;

    RTC rtc;

    Image image;
    Gif gif;

    Touch touchscreen;

    Frontlight frontlight;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;


    uint32_t pinLUT[256];
    uint32_t *GLUT;
    uint32_t *GLUT2;
    uint8_t *DMemoryNew;
    uint8_t *_partial;
    uint8_t *DMemory4Bit;
    uint8_t *_pBuffer;
    uint8_t waveform3Bit[8][9] = WAVEFORM3BIT;
    uint16_t _partialUpdateLimiter = 10;
    uint16_t _partialUpdateCounter = 0;
    uint8_t _blockPartial = 1;
    int16_t _sdCardOk = 0;
    uint8_t pwrMode = INKPLATE_NORMAL_PWR_MODE;

  private:
    void calculateLUTs();
    void pmicBegin();
    void blockGpioPins();
    uint8_t initializeFramebuffers();
    void gpioInit();
    uint8_t readPowerGood();
    void pinsAsOutputs();
    void display1b(bool _leaveOn);
    void display3b(bool _leaveOn);
    void pinsZstate();
    uint8_t getPanelState();
    void setPanelState(uint8_t state);
    void clean(uint8_t c, uint8_t rep);
    void vscan_start();
    void hscan_start(uint32_t _d);
    void vscan_end();
    uint8_t _panelState = 0;
    Inkplate *_inkplate;
    bool writeVCOMToPanelEEPROM(double vcom);
    void writeReg(uint8_t reg, float data);
    uint8_t readReg(uint8_t reg);
};

#endif
#endif