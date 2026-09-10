/**
 **************************************************
 *
 * @file        Inkplate10Driver.h
 * @brief       Low level driver for the Inkplate 10 e-paper panel
 *
 *              The Inkplate 10 uses a 9.7" 1200x825 monochrome e-paper panel driven over a
 *              parallel interface. The panel data lines are driven directly through the ESP32
 *              GPIO registers, while the control lines and the peripherals are handled by two
 *              MCP/PCAL GPIO expanders. The TPS65186 PMIC generates the e-paper driving rails
 *              and also provides the panel temperature reading.
 *
 *              The driver supports both display modes: 1 bit (black and white, with partial
 *              updates) and 3 bit (8 levels of grey, full updates only). The waveforms used
 *              for the greyscale refresh are in waveforms.h and are picked based on the
 *              waveform revision stored in the board EEPROM.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE10DRIVER_H__
#define __INKPLATE10DRIVER_H__

// Header guard for the Arduino include
#if defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE10)

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 10"

// Include main header file for the Arduino.
#include "Arduino.h"

#include "EEPROM.h"

// Include GPIO expander selection.
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


struct waveformData
{
    uint8_t header = 'W';
    uint8_t waveformId;
    uint8_t waveform[8][9];
    uint8_t temp = 20;
    uint8_t checksum;
};

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


    bool isPowerGood();

    bool setVCOM(double vcom);
    double getStoredVCOM();
    double getVCOMValue();

    bool setWaveform(uint8_t waveformNumber, bool burnToEEPROM = true);

    IOExpander expander1;
    IOExpander expander2;

    TPS65186 pmic;

    RTC rtc;

#ifdef ARDUINO_INKPLATE10
    Touchpad touchpad;
#endif

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

    void changeWaveform(uint8_t *_wf);
    uint8_t calculateChecksum(struct waveformData _w);
    bool getWaveformFromEEPROM(struct waveformData *_w);
    void burnWaveformToEEPROM(struct waveformData _w);

    static const uint8_t waveform1[8][9];
    static const uint8_t waveform2[8][9];
    static const uint8_t waveform3[8][9];
    static const uint8_t waveform4[8][9];
    static const uint8_t waveform5[8][9];

    static const uint8_t *const waveformList[5];
};

#endif
#endif