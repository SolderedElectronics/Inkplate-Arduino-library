/**
 **************************************************
 *
 * @file        Inkplate6COLORDriver.h
 * @brief       Low level driver for the Inkplate 6 COLOR e-paper panel
 *
 *              The Inkplate 6 COLOR uses a 5.83" 600x448 seven colour e-paper panel driven
 *              over SPI. The panel controller generates its own driving voltages, so the
 *              driver only handles the panel power enable, reset, the busy line and the SPI
 *              traffic, while the on board GPIO expander takes care of the peripherals.
 *
 *              The frame buffer holds one 4 bit colour index per pixel, two pixels per byte,
 *              and supports the seven panel colours.
 *              Only full updates are supported. The board also exposes the touchpad buttons.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE6COLORDRIVER_H__
#define __INKPLATE6COLORDRIVER_H__

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATECOLOR

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 6COLOR"

// Include main header file for the Arduino.
#include "Arduino.h"

// Include library for PCAL6416A GPIO expander.
#include "../../system/pcalExpander/pcalExpander.h"

#include "pins.h"


#include "../../graphics/ImageColor/ImageColor.h"

#include "Wire.h"

#include "../../graphics/GraphicsDefs.h"

#include "../../features/featureSelect.h"

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

    int einkOn();
    void einkOff();

    void setSdCardOk(int16_t s);
    int16_t getSdCardOk();
    int16_t sdCardInit();
    void sdCardSleep();
    SdFat &getSdFat();
    SPIClass *getSPIptr();

    double readBattery();


    IOExpander expander1;

    RTC rtc;

    Touchpad touchpad;

    ImageColor image;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;

    uint8_t *DMemory4Bit;

    int16_t _sdCardOk = 0;


  private:
    void calculateLUTs();
    void pmicBegin();
    uint8_t initializeFramebuffers();
    void gpioInit();
    uint8_t getPanelState();
    void setPanelState(uint8_t state);
    void clean();
    void resetPanel();
    void sendCommand(uint8_t _command);
    void sendData(uint8_t *_data, int _n);
    void sendData(uint8_t _data);
    bool setPanelDeepSleep(bool _state);
    void setIOExpanderForLowPower();
    uint8_t _panelState = 0;
    Inkplate *_inkplate;
};

#endif
#endif