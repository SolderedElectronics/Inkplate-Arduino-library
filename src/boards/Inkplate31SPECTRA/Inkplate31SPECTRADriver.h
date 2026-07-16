#ifndef __INKPLATE31DRIVER_H__
#define __INKPLATE31DRIVER_H__

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE31SPECTRA

// Inkplate Board name.
#define INKPLATE_BOARD_NAME "Inkplate 31"

// Include main header file for the Arduino.
#include "Arduino.h"

// Include library for PCAL6416A GPIO expander.
#include "../../system/pcalExpander/pcalExpander.h"

#include "pins.h"

#include "../../graphics/ImageColor/ImageColor.h"

#include "../../features/IST9201/IST9201.h"

#include "Wire.h"
#include "driver/spi_master.h"

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
    void displayPartial(int16_t x, int16_t y, int16_t w, int16_t h, bool _leaveOn = 0);
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
    IOExpander expander2; // Present on the board, reserved/unused by this driver for now.

    RTC rtc;

    ImageColor image;

    IST9201 pmic;

    uint8_t _beginDone = 0;
    uint8_t _displayMode;

    uint8_t *DMemory4Bit;

    int16_t _sdCardOk = 0;


  private:
    uint8_t initializeFramebuffers();
    void screenInit();
    uint8_t getPanelState();
    void setPanelState(uint8_t state);
    void resetPanel();
    void waitForBusy();
    uint8_t checkDriverICStatus();
    void setPanelPinsToLow();

    // Sends a command (+ optional parameter bytes) to the chips selected by csMask
    // (bit i = logical CS i). Handles CS assert/deassert internally.
    void sendPanelCommand(uint8_t cmd, const uint8_t *params, uint32_t n, uint8_t csMask);

    // CS control. All 8 CS lines live on expander1 - csExpanderBit[] (pins.h) maps the
    // logical CS index used throughout this driver to the actual expander bit.
    void csSelect(uint8_t csIndex);
    void csDeselect(uint8_t csIndex);
    void csSelectMask(uint8_t csMask);
    void csDeselectMask(uint8_t csMask);
    void csDeselectAll();

    // Quad-SPI transport (ESP-IDF driver, not Arduino SPIClass - see CLAUDE.md for why).
    void qspiInit();
    void qspiWriteCommand(uint8_t cmd);
    void qspiWriteData(const uint8_t *data, uint32_t len);
    void qspiReadData(uint8_t cmd, uint8_t *data, uint32_t len);

    spi_device_handle_t _qspiDevice = nullptr;

    // Per-column lookup tables built once in initDriver(), so writePixelInternal can
    // address the per-chip-strip buffer in O(1) instead of scanning chipXOffset[].
    uint32_t _colChipByteBase[E_INK_WIDTH]; // chip index * SPECTRA315_CHIP_BUFFER_BYTES
    uint16_t _colByteInRow[E_INK_WIDTH];    // local column / 2 (byte offset within the row)
    uint8_t _colNibble[E_INK_WIDTH];        // local column & 1 (which nibble of the byte)

    uint8_t _panelState = 0;
    Inkplate *_inkplate;
};


#endif
#endif
