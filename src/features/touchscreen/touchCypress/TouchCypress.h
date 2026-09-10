/**
 **************************************************
 * @file        TouchCypress.h
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
 * @authors     Borna Biro for Soldered
 ***************************************************/

#ifndef __TOUCH_CYPRESS_H__
#define __TOUCH_CYPRESS_H__

// Check for the board, do not include files if they are not needed.
#ifdef ARDUINO_INKPLATE6FLICK

// Include necessary Inkplate library files.
// Include Arduino main header file.
#include "Arduino.h"


// Include Arduino Wire library (for I2C communication with touchscreen driver).
#include "Wire.h"

// Include board specific defines.
#include "../../../system/defines.h"

// Include Cypress touch Typedefs.
#include "TouchCypressTypedefs.h"

// Cypress Touch IC I2C address (7 bit I2C address).
#define CYPRESS_TOUCH_I2C_ADDR 0x24

// GPIOs for touchscreen controller.
//#define CYPRESS_TOUCH_PWR_MOS_PIN   IO_PIN_B4
//#define CYPRESS_TOUCH_RST_PIN       IO_PIN_B2

// Cypress touchscreen controller I2C regs.
#define CYPRESS_TOUCH_BASE_ADDR       0x00
#define CYPRESS_TOUCH_SOFT_RST_MODE   0x01
#define CYPRESS_TOUCH_SYSINFO_MODE    0x10
#define CYPRESS_TOUCH_OPERATE_MODE    0x00
#define CYPRESS_TOUCH_LOW_POWER_MODE  0x04
#define CYPRESS_TOUCH_DEEP_SLEEP_MODE 0x02
#define CYPRESS_TOUCH_REG_ACT_INTRVL  0x1D

// Active Power state scanning/processing refresh interval
#define CYPRESS_TOUCH_ACT_INTRVL_DFLT 0x00 /* ms */
// Low Power state scanning/processing refresh interval
#define CYPRESS_TOUCH_LP_INTRVL_DFLT 0x0A /* ms */
// Touch timeout for the Active power */
#define CYPRESS_TOUCH_TCH_TMOUT_DFLT 0xFF /* ms */

// Max X and Y sizes reported by the TSC.
#define CYPRESS_TOUCH_MAX_X 682
#define CYPRESS_TOUCH_MAX_Y 1023

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
    uint8_t getData(uint16_t *xPos, uint16_t *yPos, uint8_t *z = NULL);

    void getRawData(uint8_t *b);

    // Do a handshake for Touchscreen Controller to acknowledge successfull touch report read.
    // Must be public due interrupts.
    void handshake();

  private:
    // Bootloader struct typedef.
    struct cyttspBootloaderData _blData;

    // System info data typedef.
    struct cyttspSysinfoData _sysData;

    // Method disables or enables power to the Touchscreen.
    void power(bool _pwr);

    // Get the new touch event data/report.
    bool getTouchData(struct cypressTouchData *_touchData);

    // Scale touch data report to fit screen (and also rotation).
    void scale(struct cypressTouchData *_touchData, uint16_t _xSize, uint16_t _ySize, bool _flipX, bool _flipY,
               bool _swapXY);

    // Disable touchscreen.
    void end();

    // Methods executes HW reset (with Touchscreen RST pin).
    void reset();

    // Method executes SW reset command for Touchscreen via I2C.
    void swReset();

    // Method loads bootloader register from Touchscreen IC via I2C.
    bool loadBootloaderRegs(struct cyttspBootloaderData *_blDataPtr);

    // Method forces Touchscreen Controller to exits bootloader mode and executes preloaded FW code.
    bool exitBootLoaderMode();

    // Force Touchscreen Controller into system info mode.
    bool setSysInfoMode(struct cyttspSysinfoData *_sysDataPtr);

    // Load system into register into their default values.
    bool setSysInfoRegs(struct cyttspSysinfoData *_sysDataPtr);

    // Try to ping Touchscreen controller via I2C (I2C test).
    bool ping(int _retries = 5);

    // Low-level I2C stuff.
    // Send command to the Touchscreen Controller via I2C.
    bool sendCommand(uint8_t _cmd);

    // Read Touchscreen Controller registers from the I2C by using Arduino Wire libary.
    bool readI2CRegs(uint8_t _cmd, uint8_t *_buffer, int _len);

    // Write into Touchscreen Controller registers with I2C by using Arduino Wire library.
    bool writeI2CRegs(uint8_t _cmd, uint8_t *_buffer, int _len);

    // Used in touchInArea method.
    uint8_t touchN;
    uint16_t touchX[2], touchY[2];
    uint32_t touchT = 0;

    // Checks status of the touchscreen initialization (for attachInterrupt funciton).
    bool _tsInitDone = false;

    Inkplate *_inkplate;
};

#endif

#endif