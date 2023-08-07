/**
 **************************************************
 * @file        Touch.h
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
 * @authors     @ Soldered
 ***************************************************/

#include "Touch.h"

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATEPLUS2)

uint16_t _tsXResolution;
uint16_t _tsYResolution;

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
    int16_t x2 = x1 + w, y2 = y1 + h;
    if (tsAvailable())
    {
        uint8_t n;
        uint16_t x[2], y[2];
        n = tsGetData(x, y);

        if (n)
        {
            touchT = millis();
            touchN = n;
            memcpy(touchX, x, 2);
            memcpy(touchY, y, 2);
        }
    }

    if (millis() - touchT < 100)
    {
        // Serial.printf("%d: %d, %d - %d, %d\n", touchN, touchX[0], touchY[0],
        // touchX[1], touchY[1]);
        if (touchN == 1 && BOUND(x1, touchX[0], x2) && BOUND(y1, touchY[0], y2))
            return true;
        if (touchN == 2 && ((BOUND(x1, touchX[0], x2) && BOUND(y1, touchY[0], y2)) ||
                            (BOUND(x1, touchX[1], x2) && BOUND(y1, touchY[1], y2))))
            return true;
    }
    return false;
}

/**
 * @brief       tsWriteRegs writes data to touchscreen registers
 *
 * @param       uint8_t _addr
 *              touchscreen register address
 * @param       uint8_t *_buff
 *              buffer to write into touchscreen registers
 * @param       uint8_t _size
 *              number of bytes to write
 *
 * @return      returns 1 on successful write, 0 on fail
 */
uint8_t Touch::tsWriteRegs(uint8_t _addr, const uint8_t *_buff, uint8_t _size)
{
    Wire.beginTransmission(_addr);
    Wire.write(_buff, _size);
    return Wire.endTransmission();
}

/**
 * @brief       tsReadRegs returns touchscreen registers content
 *
 * @param       uint8_t _addr
 *              touchscreen register address
 * @param       uint8_t *_buff
 *              buffer to write touchscreen register content into
 * @param       uint8_t _size
 *              number of bytes to read
 */
void Touch::tsReadRegs(uint8_t _addr, uint8_t *_buff, uint8_t _size)
{
    Wire.requestFrom(_addr, _size);
    Wire.readBytes(_buff, _size);
}

/**
 * @brief       tsHardwareReset resets ts hardware
 */
void Touch::tsHardwareReset()
{
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, TS_RTS, LOW);
    delay(15);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, TS_RTS, HIGH);
    delay(15);
}

/**
 * @brief       tsSoftwareReset resets toucscreen software
 *
 * @return      true if successful, false if failed
 */
bool Touch::tsSoftwareReset()
{
    const uint8_t soft_rst_cmd[] = {0x77, 0x77, 0x77, 0x77};
    if (tsWriteRegs(TS_ADDR, soft_rst_cmd, 4) == 0)
    {
        uint8_t rb[4];
        uint16_t timeout = 1000;
        while (!_tsFlag && timeout > 0)
        {
            delay(1);
            timeout--;
        }
        if (timeout > 0)
            _tsFlag = true;
        Wire.requestFrom(0x15, 4);
        Wire.readBytes(rb, 4);
        _tsFlag = false;
        if (!memcmp(rb, hello_packet, 4))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/**
 * @brief       tsInit starts touchscreen and sets ts registers
 *
 * @param       uint8_t _pwrState
 *              power state for touchScreen
 */
bool Touch::tsInit(uint8_t _pwrState)
{
    // Enable power to TS
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, TOUCHSCREEN_EN, LOW);

    pinMode(TS_INT, INPUT_PULLUP);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, TS_RTS, OUTPUT);
    attachInterrupt(TS_INT, tsInt, FALLING);
    tsHardwareReset();
    if (!tsSoftwareReset())
    {
        detachInterrupt(TS_INT);
        return false;
    }
    tsGetResolution(&_tsXResolution, &_tsYResolution);
    tsSetPowerState(_pwrState);

    tsInt();
    return true;
}

/**
 * @brief       tsShutdown turns off touchscreen power
 */
void Touch::tsShutdown()
{
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, TOUCHSCREEN_EN, HIGH);
}

/**
 * @brief       tsGetRawData gets touchscreen register content
 *
 * @param       uint8_t *b
 *              pointer to store register content
 */
void Touch::tsGetRawData(uint8_t *b)
{
    Wire.requestFrom(TS_ADDR, 8);
    Wire.readBytes(b, 8);
}

/**
 * @brief       tsGetXY gets x and y plane values
 *
 * @param       uint8_t *_d
 *              pointer to register content of touchscreen register (data must
 * be adapted, cant use raw data)
 * @param       uint16_t *x
 *              pointer to store x plane data
 * @param       uint16_t *y
 *              pointer to store y plane data
 */
void Touch::tsGetXY(uint8_t *_d, uint16_t *x, uint16_t *y)
{
    *x = *y = 0;
    *x = (_d[0] & 0xf0);
    *x <<= 4;
    *x |= _d[1];
    *y = (_d[0] & 0x0f);
    *y <<= 8;
    *y |= _d[2];
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
    uint8_t _raw[8];
    uint16_t xRaw[2], yRaw[2];
    uint8_t fingers = 0;
    _tsFlag = false;
    tsGetRawData(_raw);
    for (int i = 0; i < 8; i++)
    {
        if (_raw[7] & (1 << i))
            fingers++;
    }

    for (int i = 0; i < 2; i++)
    {
        tsGetXY((_raw + 1) + (i * 3), &xRaw[i], &yRaw[i]);

// Default touchscreen rotation
#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)

        switch (getRotation())
        {
        case 0:
            yPos[i] = ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            xPos[i] = E_INK_WIDTH - 1 - ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 1:
            xPos[i] = ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            yPos[i] = ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 2:
            yPos[i] = E_INK_HEIGHT - 1 - ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            xPos[i] = ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 3:
            xPos[i] = E_INK_HEIGHT - 1 - ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            yPos[i] = E_INK_WIDTH - 1 - ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        }

// For Inkplate PLUS2, both X and Y are mirrored for the touchscreen
#elif defined(ARDUINO_INKPLATEPLUS2)

        switch (getRotation())
        {
        case 0:
            yPos[i] = E_INK_HEIGHT - 1 - ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            xPos[i] = ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 1:
            xPos[i] = E_INK_HEIGHT - 1 - ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            yPos[i] = E_INK_WIDTH - 1 - ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 2:
            yPos[i] = ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            xPos[i] = E_INK_WIDTH - 1 - ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        case 3:
            xPos[i] = ((xRaw[i] * E_INK_HEIGHT - 1) / _tsXResolution);
            yPos[i] = ((yRaw[i] * E_INK_WIDTH - 1) / _tsYResolution);
            break;
        }

#endif
    }
    return fingers;
}

/**
 * @brief       tsGetResolution gets touchscreen resolution for x and y
 *
 * @param       uint16_t *xRes
 *              pointer to store x resolution
 * @param       uint16_t *yRes
 *              pointer to store y resolution
 */
void Touch::tsGetResolution(uint16_t *xRes, uint16_t *yRes)
{
    const uint8_t cmd_x[] = {0x53, 0x60, 0x00, 0x00}; // Get x resolution
    const uint8_t cmd_y[] = {0x53, 0x63, 0x00, 0x00}; // Get y resolution
    uint8_t rec[4];
    tsWriteRegs(TS_ADDR, cmd_x, 4);
    tsReadRegs(TS_ADDR, rec, 4);
    *xRes = ((rec[2])) | ((rec[3] & 0xf0) << 4);
    tsWriteRegs(TS_ADDR, cmd_y, 4);
    tsReadRegs(TS_ADDR, rec, 4);
    *yRes = ((rec[2])) | ((rec[3] & 0xf0) << 4);
    _tsFlag = false;
}

/**
 * @brief       tsSetPowerState sets power state of touchscreen
 *
 * @param       uint8_t _s
 *              touchscreen power state to be set (0 or 1)
 */
void Touch::tsSetPowerState(uint8_t _s)
{
    _s &= 1;
    uint8_t powerStateReg[] = {0x54, 0x50, 0x00, 0x01};
    powerStateReg[1] |= (_s << 3);
    tsWriteRegs(TS_ADDR, powerStateReg, 4);
}

/**
 * @brief       tsGetPowerState checks if touchscreen is powered up
 *
 * @return      touchscreen power state, 1 if powered, 0 if not
 */
uint8_t Touch::tsGetPowerState()
{
    const uint8_t powerStateReg[] = {0x53, 0x50, 0x00, 0x01};
    uint8_t buf[4];
    tsWriteRegs(TS_ADDR, powerStateReg, 4);
    _tsFlag = false;
    tsReadRegs(TS_ADDR, buf, 4);
    return (buf[1] >> 3) & 1;
}

/**
 * @brief       tsAvailable checks for touch screen functionality
 *
 * @return      tsflag, 1 for available touchscreen, 0 if not
 */
bool Touch::tsAvailable()
{
    return _tsFlag;
}

#endif