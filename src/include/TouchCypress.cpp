/**
 **************************************************
 * @file        TouchCypress.h
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
 * @authors     Borna Biro for Soldered
 ***************************************************/

#include "TouchCypress.h"

#ifdef ARDUINO_INKPLATE6FLICK

// Macro helpers.
#define TS_GET_BOOTLOADERMODE(reg)		(((reg) & 0x10) >> 4)

/**
 * @brief       touchInArea checks if touch occured in given rectangle area
 *
 * @param       int16_t x1
 *              rectangle top left corner x plane
 * @param       int16_t y1
 *              rectangle top left corner y plane
 * @param       int16_t w
 *              rectangle width
 * @param       int16_t h
 *              rectangle height
 *
 * @return      true if successful, false if failed
 */
bool Touch::touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h)
{

}

/**
 * @brief       tsInit starts touchscreen and sets ts registers
 *
 * @param       uint8_t _pwrState
 *              power state for touchScreen
 */
bool Touch::tsInit(uint8_t _pwrState)
{
    // Set GPIO pins.
    pinModeInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TOUCHSCREEN_EN, OUTPUT);
    pinModeInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, OUTPUT);

    // Check power state.
    if (_pwrState)
    {
        // Enable the power to the touch.
        tsPower(_pwrState);

        // Do a HW reset.
        tsReset();

        // Try to ping it.
        if(!tsPing(5)) return false;

        // Issue a SW reset.
        tsSendCommand(0x01);

        // Read bootloader data.
        tsLoadBootloaderRegs(&_blData);

        // Exit bootloader mode. - Does not exit bootloader propery!
        if (!tsExitBootLoaderMode())
            return false;

        // Set mode to system info mode.
        if (!tsSetSysInfoMode(&_sysData))
            return false;

        // Set system info regs.
        if (!tsSetSysInfoRegs(&_sysData))
            return false;

        // Switch it into operate mode (also can be in deep sleep mode as well as low power mode).
        tsSendCommand(CYPRESS_TOUCH_OPERATE_MODE);

        // Set dist value for detection?
        uint8_t _distDefaultValue = 0xF8;
        tsWriteI2CRegs(0x1E, &_distDefaultValue, 1);

        // Add interrupt callback.
        pinMode(TS_INT, INPUT);
        attachInterrupt(TS_INT, tsInt, FALLING);

        // Clear the interrpt flag.
        _tsFlag = false;
    }
    else
    {
        tsEnd();
    }

    // Everything went ok? Return 1 for success.
    return 1;
}

/**
 * @brief       tsShutdown turns off touchscreen power
 */
void Touch::tsShutdown()
{
    // Turn off the touchscreen power supply.
    tsPower(false);
}

/**
 * @brief       tsGetRawData gets touchscreen register content
 *
 * @param       uint8_t *b
 *              pointer to store register content
 * 
 * @note        Array must be at least 15 bytes long.
 */
void Touch::tsGetRawData(uint8_t *b)
{
    
}

/**
 * @brief       tsGetData checks x, y position and returns number of fingers on
 * screen
 *
 * @param       uint16_t *xPos
 *              pointer to store x position of finger
 * @param       uint16_t *yPos
 *              pointer to store y position of finger
 *
 * @return      returns number of fingers currently on screen
 *
 * @note        touch screen doesn't return data for two fingers when fingers
 * are align at the y axis, or one above another
 */
uint8_t Touch::tsGetData(uint16_t *xPos, uint16_t *yPos)
{

}

/**
 * @brief       Set power mode of the Touchscreen Controller. There are 3 modes
 *              CYPRESS_TOUCH_OPERATE_MODE - Normal mode (fast response, higher accuracy, higher power consumption).
 *                                           Current ~ 15mA.
 *              CYPRESS_TOUCH_LOW_POWER_MODE - After few seconds of inactivity, TSC goes into low power ode and periodically
 *                                             goes into operating mode to check for touch event. Current ~4mA.
 *              CYPRESS_TOUCH_DEEP_SLEEP_MODE - Disable TSC. Current ~25uA.
 * 
 * @param       uint8_t _s
 *              Power mode - Can only be CYPRESS_TOUCH_OPERATE_MODE, CYPRESS_TOUCH_LOW_POWER_MODE or CYPRESS_TOUCH_DEEP_SLEEP_MODE.
 *              [defined in TouchCypress.h]
 */
void Touch::tsSetPowerState(uint8_t _s)
{
    // Check for the parameters.
    if ((_s == CYPRESS_TOUCH_DEEP_SLEEP_MODE) || (_s == CYPRESS_TOUCH_LOW_POWER_MODE) || (_s == CYPRESS_TOUCH_OPERATE_MODE))
    {
        // Set new power mode setting.
        tsSendCommand(_s);
    }
}

/**
 * @brief       tsGetPowerState gets current power mode of the touchscreen.
 *
 * @return      Returns CYPRESS_TOUCH_OPERATE_MODE, CYPRESS_TOUCH_LOW_POWER_MODE or CYPRESS_TOUCH_DEEP_SLEEP_MODE
 *              depending on the current power mode of the touchscreen controller.
 */
uint8_t Touch::tsGetPowerState()
{
    // Send subaddress for System Info.
    Wire.beginTransmission(CPYRESS_TOUCH_I2C_ADDR);
    Wire.write(CYPRESS_TOUCH_BASE_ADDR);
    Wire.endTransmission();

    // Fist byte represents current power mode.
    Wire.requestFrom(CPYRESS_TOUCH_I2C_ADDR, 1);
    return Wire.read();
}

/**
 * @brief       tsAvailable checks for touch screen functionality
 *
 * @return      tsflag, 1 for available touchscreen, 0 if not
 */
bool Touch::tsAvailable()
{
    // Check for the handshake.
    if (_tsFlag) tsHandshake();
    bool _temp = _tsFlag;
    _tsFlag = false;
    return _temp;
}


// Private members.
/**
 * @brief       Enable or disable power to the Touchscreen Controller.
 * 
 * @param       bool _pwr
 *              true - Enable power to the Touchscreen/Touchscreen Controller.
 *              false - Disable power to the Touchscreen/Touchscreen Controller to reduce power
 *              consunption in sleep or to do power cycle.
 */
void Touch::tsPower(bool _pwr)
{
    if (_pwr)
    {
        // Enable the power MOSFET.
        digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TOUCHSCREEN_EN, HIGH);

        // Wait a little bit before proceeding any further.
        delay(50);

        // Set reset pin to high.
        digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, HIGH);

        // Wait a little bit.
        delay(50);
    }
    else
    {
        // Disable the power MOSFET switch.
        digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TOUCHSCREEN_EN, LOW);

        // Wait a bit to discharge caps.
        delay(50);

        // Set reset pin to low.
        digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, LOW);
    }
}

/**
 * @brief       Disable touchscreen. Detach interrupt, clear interrput flag, disable power to the 
 *              Touchscreen Controller.
 * 
 */
void Touch::tsEnd()
{
    // Detach interrupt.
    detachInterrupt(TS_INT);

    // Clear interrupt flag.
    _tsFlag = false;

    // Disable the power to the touch.
    tsPower(false);
}

/**
 * @brief       Method does a HW reset by using RST pin on the Touchscreen/Touchscreen Controller.
 * 
 */
void Touch::tsReset()
{
    // Toggle RST line. Loggic low must be at least 1ms, re-init after reset not specified, 10 ms (from Linux kernel).
    digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, HIGH);
    delay(10);
    digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, LOW);
    delay(2);
    digitalWriteInternal(TOUCHSCREEN_IO_EXPANDER, TOUCHSCREEN_IO_REGS, TS_RST, HIGH);
    delay(10);
}

/**
 * @brief       Method executes a SW reset by using I2C command.
 * 
 */
void Touch::tsSwReset()
{
    // Issue a command for SW reset.
    tsSendCommand(CYPRESS_TOUCH_SOFT_RST_MODE);

    // Wait a little bit.
    delay(20);
}

/**
 * @brief       Function reads bootloader registers from the Touchscreen Controller.
 * 
 * @param       struct cyttspBootloaderData *_blDataPtr
 *              Defined in TouchCypressTypedefs.h, pointer to the struct cyttspBootloaderData to
 *              store bootloader registers data.
 * 
 * @return      bool
 *              true - Loading bootloader data register was successfull.
 *              false - Loading bootloader data from the registers has failed.
 */
bool Touch::tsLoadBootloaderRegs(struct cyttspBootloaderData *_blDataPtr)
{
    // Bootloader temp. registers array.
    uint8_t _bootloaderData[16];

    if (!tsReadI2CRegs(CYPRESS_TOUCH_BASE_ADDR, _bootloaderData, 16)) return false;

    // Parse Bootloader data into typedef struct.
    memcpy(_blDataPtr, _bootloaderData, 16);

    return true;
}

/**
 * @brief       Method forces Touchscreen Controller to exit bootloader mode and enters normal
 *              operating mode - to load preloaded firmware (possibly TTSP - TrueTouch Standard Product Firmware).
 * 
 * @return      bool
 *              true - Touchscreen Controller quit bootloader mode and loaded TTSP FW that is currently executing.
 *              false - Touchscreen Controller failed to exit bootloader mode.
 * 
 * @note        It exiting bootloader mode fails reading touch events will fail. Do not go further with the code for the
 *              Touchscreen.
 */
bool Touch::tsExitBootLoaderMode()
{
    // Bootloader command array.
    uint8_t _blCommandArry[] = 
    {
        0x00,   // File offset.
        0xFF,   // Command.
        0xA5,   // Exit bootloader command.
        0, 1, 2, 3, 4, 5, 6, 7  // Default keys.
    };

    // Write bootloader settings.
    tsWriteI2CRegs(CYPRESS_TOUCH_BASE_ADDR, _blCommandArry, sizeof(_blCommandArry));

    // Wait a little bit - Must be long delay, otherwise setSysInfoMode will fail!
    // Delay of 150ms will fail - tested!
    delay(500);

    // Get bootloader data.
    struct cyttspBootloaderData _bootloaderData;
    tsLoadBootloaderRegs(&_bootloaderData);

    // Check for validity.
    if (TS_GET_BOOTLOADERMODE(_bootloaderData.bl_status)) return false;

    // If everything went ok return true.
    return true;
}

/**
 * @brief       Set Touchscreen Controller into System Info mode.
 * 
 * @param       struct cyttspSysinfoData *_sysDataPtr
 *              Defined cypressTouchTypedefs.h, pointer to the struct for the system info registers.
 * 
 * @return      bool
 *              true - System Info mode usccessfully set.
 *              false - System Info mode failed.
 * 
 * @note        As soon as this fails, stop the Touchscreen from executing, touch data will be invalid. 
 */
bool Touch::tsSetSysInfoMode(struct cyttspSysinfoData *_sysDataPtr)
{
    // Change mode to system info.
    tsSendCommand(CYPRESS_TOUCH_SYSINFO_MODE);

    // Wait a bit.
    delay(20);

    // Buffer for the system info data.
    uint8_t _sysInfoArray[32];

    // Read the registers.
    if (!tsReadI2CRegs(CYPRESS_TOUCH_BASE_ADDR, _sysInfoArray, sizeof(_sysInfoArray)))
    {
        return false;
    }

    // Copy into struct typedef.
    memcpy(_sysDataPtr, _sysInfoArray, sizeof(_sysInfoArray));

    // Do a handshake!
    tsHandshake();

    // Check TTS version. If is zero, something went wrong.
    if (!_sysDataPtr->tts_verh && !_sysDataPtr->tts_verl)
    {
        return false;
    }

    // Everything went ok? Return true for success.
    return true;
}

/**
 * @brief       Set System info registers into their default state.
 * 
 * @param       struct cyttspSysinfoData *_sysDataPtr
 *              Defined in TouchCypressTypedefs.h, poinet to the struct for the system info registers.
 * 
 * @return      bool
 *              true - Registers are set successfully.
 *              false - Setting registers has failed.
 * 
 * @note        Stop the tuchscreen code from executing if this fails, touch data will be invalid.
 */
bool Touch::tsSetSysInfoRegs(struct cyttspSysinfoData *_sysDataPtr)
{
    // Modify registers to the default values.
    _sysDataPtr->act_intrvl = CYPRESS_TOUCH_ACT_INTRVL_DFLT;
    _sysDataPtr->tch_tmout = CYPRESS_TOUCH_TCH_TMOUT_DFLT;
    _sysDataPtr->lp_intrvl = CYPRESS_TOUCH_LP_INTRVL_DFLT;

    uint8_t _regs[] = {_sysDataPtr->act_intrvl, _sysDataPtr->tch_tmout, _sysDataPtr->lp_intrvl};

    // Send the registers to the I2C. Check if failed. If failed, return false.
    if (!tsWriteI2CRegs(0x1D, _regs, 3)) return false;

    // Wait a little bit.
    delay(20);

    // Everything went ok? Return true for success.
    return true;
}

/**
 * @brief       Method does handshake for the Touchscreen/Touchscreen Controller to confirm successfull read
 *              new touch report data.
 * 
 * @note        Handshake must be done on every new touch event from the Interrupt.
 */
void Touch::tsHandshake()
{
    // Read the hst_mode register (address 0x00).
    uint8_t _hstModeReg = 0;
    tsReadI2CRegs(CYPRESS_TOUCH_BASE_ADDR, &_hstModeReg, 1);
    _hstModeReg ^= 0x80;
    tsWriteI2CRegs(CYPRESS_TOUCH_BASE_ADDR, &_hstModeReg, 1);
}

bool Touch::tsPing(int _retries)
{
    // Sucess / return variable. Set it by default on fail.
    int _retValue = 1;

    // Try to ping multiple times in a row (just in case TSC is not in low power mode).
    // Delay between retires is 20ms (just a wildguess, don't have any documentation).
    for (int i = 0; i < _retries; i++)
    {
        // Ping the TSC (touchscreen controller) on I2C.
        Wire.beginTransmission(CPYRESS_TOUCH_I2C_ADDR);
        _retValue = Wire.endTransmission();

        // Return value is 0? That means ACK, TSC found!
        if (_retValue == 0)
        {
            return true;
        }

        // TSC not found? Try again, but before retry wait a little bit.
        delay(20);
    }

    // Got here? Not good, TSC not found, return error.
    return false;
}

// -----------------------------LOW level I2C functions-----------------------------

/**
 * @brief       Method sends I2C command to the Touchscreen Controller IC.
 * 
 * @param       uint8_t _cmd
 *              I2C command for the Touchscreen Controller IC. 
 * 
 * @return      true - Command is succesfully send and executed.
 *              false - I2C command send failed.
 */
bool Touch::tsSendCommand(uint8_t _cmd)
{
    // Init I2C communication.
    Wire.beginTransmission(CPYRESS_TOUCH_I2C_ADDR);
    
    // I'm not sure about this?
    // Write I2C sub-address (register address).
    Wire.write(CYPRESS_TOUCH_BASE_ADDR);

    // Write command.
    Wire.write(_cmd);

    // Wait a little bit.
    delay(20);

    // Send to I2C!
    return Wire.endTransmission() == 0?true:false;
}

/**
 * @brief       Method reads multiple I2C registers at once from the touchscreen controller and save them into buffer.
 * 
 * @param       uint8_t _cmd
 *              I2C command for the Touchscreen Controller.
 * @param       uint8_t *_buffer
 *              Buffer for the bytes read from the Touchscreen Controller. 
 * @param       int _len
 *              How many bytes to read from the I2C (Touchscreen Controller).
 * 
 * @return      bool
 *              true - I2C register read was successfull.
 *              false - I2C register read failed.
 * 
 * @note        More than 32 bytes can be read at the same time.
 */
bool Touch::tsReadI2CRegs(uint8_t _cmd, uint8_t *_buffer, int _len)
{
    // Init I2C communication!
    Wire.beginTransmission(CPYRESS_TOUCH_I2C_ADDR);

    // Send command byte.
    Wire.write(_cmd);

    // Write reg to the I2C! If I2C send has failed, return false.
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    // Read back data from the regs.
    // Watchout! Arduino Wire library can only read 32 bytes at the times
    int _index = 0;
    while (_len > 0)
    {
        // Check for the size of the remaining buffer.
        int _i2cLen = _len > 32?32:_len;

        // Read the bytes from the I2C.
        Wire.requestFrom(CPYRESS_TOUCH_I2C_ADDR, _i2cLen);
        Wire.readBytes(_buffer + _index, _i2cLen);
        
        // Update the buffer index position.
        _index += _i2cLen;

        // Update the lenght.
        _len -= _i2cLen;
    }

    // Everything went ok? Return true.
    return true;
}

/**
 * @brief       Method writes multiple I2C registers at once to the touchscreen controller from buffer provided.
 * 
 * @param       uint8_t _cmd
 *              I2C command for the Touchscreen Controller.
 * @param       uint8_t *_buffer
 *              Buffer for the bytes that needs to be sent to the Touchscreen Controller. 
 * @param       int _len
 *              How many bytes to write to the I2C (Touchscreen Controller).
 * 
 * @return      bool
 *              true - I2C register write was successfull.
 *              false - I2C register write failed.
 * 
 * @note        More than 32 bytes can be written at the same time.
 */
bool Touch::tsWriteI2CRegs(uint8_t _cmd, uint8_t *_buffer, int _len)
{
    // Init I2C communication!
    Wire.beginTransmission(CPYRESS_TOUCH_I2C_ADDR);

    // Send command byte.
    Wire.write(_cmd);

    // Write data.
    Wire.write(_buffer, _len);

    // Write reg to the I2C! If I2C send has failed, return false.
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    // Everything went ok? Return true.
    return true;
}

#endif