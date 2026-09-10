/**
 **************************************************
 *
 * @file        Inkplate5Driver.h
 * @brief       Low level driver for the Inkplate 5 e-paper panel
 *
 *              The Inkplate 5 uses a 5.2" 960x540 monochrome e-paper panel driven over a
 *              parallel interface. The panel data lines are driven directly through the ESP32
 *              GPIO registers, the control lines go through the on board GPIO expander, and
 *              the TPS65186 PMIC generates the e-paper driving rails and reports the panel
 *              temperature.
 *
 *              Both 1 bit (black and white, partial updates supported) and 3 bit (8 levels of
 *              grey) display modes are available. The greyscale waveforms are in waveforms.h.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE5

#ifndef __INKPLATE5DRIVER_H__
#define __INKPLATE5DRIVER_H__

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 5"

// Include main header file for the Arduino.
#include "Arduino.h"

#include "EEPROM.h"

// Include library for PCAL6416A GPIO expander.
#include "../../system/pcalExpander/pcalExpander.h"

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

    bool setVCOM(double vcom);
    double getStoredVCOM();
    double getVCOMValue();

    bool isPowerGood();


    IOExpander expander1;

    TPS65186 pmic;

    RTC rtc;

    Image image;
    Gif gif;

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

  private:
    void calculateLUTs();
    void pmicBegin();
    uint8_t initializeFramebuffers();
    void gpioInit();
    uint8_t readPowerGood();
    void blockGpioPins();
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