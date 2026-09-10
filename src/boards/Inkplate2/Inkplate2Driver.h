/**
 **************************************************
 *
 * @file        Inkplate2Driver.h
 * @brief       Low level driver for the Inkplate 2 e-paper panel
 *
 *              The Inkplate 2 uses a 2.13" 212x104 three colour (black, white and red)
 *              e-paper panel driven over SPI. Unlike the larger Inkplate boards, this one has
 *              no GPIO expander and no PMIC - the panel controller generates its own driving
 *              voltages, so the driver only handles reset, the busy line and the SPI traffic.
 *
 *              The frame buffer is split into a black and a red plane, and both planes are
 *              sent to the panel on every display() call. Only full updates are supported.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE2DRIVER_H__
#define __INKPLATE2DRIVER_H__

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE2

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 2"

// Include main header file for the Arduino.
#include "Arduino.h"

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

    double readBattery();

    // RTC rtc;

    ImageColor image;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;

    uint8_t *DMemory4Bit;


  private:
    uint8_t getPanelState();
    void setPanelState(uint8_t state);
    bool waitForEpd(uint16_t _timeout);
    void clean();
    void resetPanel();
    void sendCommand(uint8_t _command);
    void sendData(uint8_t *_data, int _n);
    void sendData(uint8_t _data);
    bool setPanelDeepSleep(bool _state);
    uint8_t _panelState = 0;
    Inkplate *_inkplate;
};

#endif
#endif