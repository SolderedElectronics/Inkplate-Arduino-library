/**
 **************************************************
 *
 * @file        Inkplate7SPECTRADriver.h
 * @brief       Low level driver for the Inkplate 7 Spectra 6 e-paper panel
 *
 *              The Inkplate 7 uses a 7.3" 800x480 Spectra 6 colour e-paper panel driven by a
 *              single controller chip over 4-wire SPI (BS0 and BS1 pulled low select that
 *              interface mode). The panel supply is switched by a MOSFET on the panel power
 *              enable pin, and the peripherals are handled by the on board GPIO expander.
 *
 *              The frame buffer holds one 4 bit colour index per pixel, two pixels per byte,
 *              and supports the six Spectra colours: black, white, yellow, red, blue and
 *              green. Only full updates are supported. The register values and the refresh
 *              sequence follow the panel manufacturer sample code.
 *
 *              The panel is mounted rotated by 180 degrees inside the enclosure, so the
 *              default rotation of the driver compensates for that.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#ifndef __INKPLATE7SPECTRA_DRIVER_H__
#define __INKPLATE7SPECTRA_DRIVER_H__

// Header guard for the Arduino include
#ifdef ARDUINO_ESP32S3_DEV

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 7SPECTRA"

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


    void setSdCardOk(int16_t s);
    int16_t getSdCardOk();
    int16_t sdCardInit();
    void sdCardSleep();
    SdFat &getSdFat();
    SPIClass *getSPIptr();

    double readBattery();


    IOExpander expander1;

    RTC rtc;

    ImageColor image;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;

    uint8_t *DMemory4Bit;

    int16_t _sdCardOk = 0;


  private:
    void screenInit();
    uint8_t getPanelState();
    void setIO();
    void setPanelPinsToLow();
    void setPanelState(uint8_t state);
    void resetPanel();
    void sendCommand(uint8_t _cmd, const uint8_t *_parameters, uint32_t _n);
    void waitForBusy();
    uint8_t _panelState = 0;
    Inkplate *_inkplate;
};


#endif
#endif
