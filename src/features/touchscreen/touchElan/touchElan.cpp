/**
 **************************************************
 * @file        TouchElan.h
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
 * @authors     @ Soldered
 ***************************************************/
#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2) || defined(ARDUINO_INKPLATE4TEMPERA)
#include "touchElan.h"
#include "Inkplate.h"
#include "../../../system/inkplateSemaphore.h"

uint16_t _tsXResolution;
uint16_t _tsYResolution;

// Interrupt function callback for Touch Interruput event.
static volatile bool _tsFlag = false;
static void IRAM_ATTR tsInt()
{
    // On interrupt event set flag to true.
    _tsFlag = true;
}

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
    if (available())
    {
        uint8_t n;
        uint16_t x[2], y[2];
        n = getData(x, y);

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

void Touch::begin(Inkplate *inkplatePtr)
{
    // Save the given inkplate pointer for internal use
    _inkplate = inkplatePtr;
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
    i2cStart();
    Wire.beginTransmission(_addr);
    Wire.write(_buff, _size);
    uint8_t result = Wire.endTransmission();
    i2cEnd();
    return result;
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
    i2cStart();
    Wire.requestFrom(_addr, _size);
    Wire.readBytes(_buff, _size);
    i2cEnd();
}

/**
 * @brief       tsHardwareReset resets ts hardware
 */
void Touch::tsHardwareReset()
{
#ifdef ARDUINO_INKPLATE4TEMPERA
    _inkplate->expander2.digitalWrite(TOUCHSCREEN_RST, LOW);
    delay(15);
    _inkplate->expander2.digitalWrite(TOUCHSCREEN_RST, HIGH);
    delay(15);
#else
    _inkplate->expander1.digitalWrite(TOUCHSCREEN_RST, LOW);
    delay(15);
    _inkplate->expander1.digitalWrite(TOUCHSCREEN_RST, HIGH);
    delay(15);
#endif
}

/**
 * @brief       tsSoftwareReset resets toucscreen software
 *
 * @return      true if successful, false if failed
 */
bool Touch::tsSoftwareReset()
{
    const uint8_t soft_rst_cmd[] = {0x77, 0x77, 0x77, 0x77};
    if (tsWriteRegs(TOUCHSCREEN_ADDR, soft_rst_cmd, 4) == 0)
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
        i2cStart();
        Wire.requestFrom(0x15, 4);
        Wire.readBytes(rb, 4);
        i2cEnd();
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
bool Touch::init(uint8_t _pwrState)
{
    if (!_inkplate)
        return false;
#ifdef ARDUINO_INKPLATE4TEMPERA
    _inkplate->expander2.digitalWrite(TOUCHSCREEN_EN, LOW);
#else
    _inkplate->expander1.digitalWrite(TOUCHSCREEN_EN, LOW);
#endif
    // Enable power to TS

    delay(100);

    pinMode(TOUCHSCREEN_INT, INPUT);
    attachInterrupt(TOUCHSCREEN_INT, tsInt, FALLING);
    tsHardwareReset();
    if (!tsSoftwareReset())
    {
        detachInterrupt(TOUCHSCREEN_INT);
        return false;
    }
    tsGetResolution(&_tsXResolution, &_tsYResolution);
    setPowerState(_pwrState);

    tsInt();
    return true;
}

/**
 * @brief       shutdown turns off touchscreen power
 */
void Touch::shutdown()
{
    end();
}

/**
 * @brief       getRawData gets touchscreen register content
 *
 * @param       uint8_t *b
 *              pointer to store register content
 */
void Touch::getRawData(uint8_t *b)
{
    i2cStart();
    Wire.requestFrom(TOUCHSCREEN_ADDR, 8);
    Wire.readBytes(b, 8);
    i2cEnd();
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
uint8_t Touch::getData(uint16_t *xPos, uint16_t *yPos)
{
    uint8_t _raw[8];
    uint16_t xRaw[2], yRaw[2];
    uint8_t fingers = 0;
    _tsFlag = false;
    getRawData(_raw);
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

        switch (_inkplate->getRotation())
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

// For Inkplate 4TEMPERA, both X and Y are mirrored for the touchscreen
#elif defined(ARDUINO_INKPLATE4TEMPERA)

        switch (_inkplate->getRotation())
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
    tsWriteRegs(TOUCHSCREEN_ADDR, cmd_x, 4);
    tsReadRegs(TOUCHSCREEN_ADDR, rec, 4);
    *xRes = ((rec[2])) | ((rec[3] & 0xf0) << 4);
    tsWriteRegs(TOUCHSCREEN_ADDR, cmd_y, 4);
    tsReadRegs(TOUCHSCREEN_ADDR, rec, 4);
    *yRes = ((rec[2])) | ((rec[3] & 0xf0) << 4);
    _tsFlag = false;
}

/**
 * @brief       setPowerState sets power state of touchscreen
 *
 * @param       uint8_t _s
 *              touchscreen power state to be set (0 or 1)
 */
void Touch::setPowerState(uint8_t _s)
{
    _s &= 1;
    uint8_t powerStateReg[] = {0x54, 0x50, 0x00, 0x01};
    powerStateReg[1] |= (_s << 3);
    tsWriteRegs(TOUCHSCREEN_ADDR, powerStateReg, 4);
}

/**
 * @brief       getPowerState checks if touchscreen is powered up
 *
 * @return      touchscreen power state, 1 if powered, 0 if not
 */
uint8_t Touch::getPowerState()
{
    const uint8_t powerStateReg[] = {0x53, 0x50, 0x00, 0x01};
    uint8_t buf[4];
    tsWriteRegs(TOUCHSCREEN_ADDR, powerStateReg, 4);
    _tsFlag = false;
    tsReadRegs(TOUCHSCREEN_ADDR, buf, 4);
    return (buf[1] >> 3) & 1;
}

/**
 * @brief       available checks for touch screen functionality
 *
 * @return      tsflag, 1 for available touchscreen, 0 if not
 */
bool Touch::available()
{
    return _tsFlag;
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
void Touch::power(bool _pwr)
{
    if (_pwr)
    {
        _inkplate->expander1.digitalWrite(TOUCHSCREEN_EN, LOW);
        delay(50);
        _inkplate->expander1.digitalWrite(TOUCHSCREEN_RST, HIGH);
        delay(50);
    }
    else
    {
        _inkplate->expander1.digitalWrite(TOUCHSCREEN_EN, HIGH);
        delay(50);
        _inkplate->expander1.digitalWrite(TOUCHSCREEN_RST, LOW);
    }
}

/**
 * @brief       Disable touchscreen. Detach interrupt, clear interrput flag, disable power to the
 *              Touchscreen Controller.
 *
 */
void Touch::end()
{
    if (_tsInitDone)
        detachInterrupt(TOUCHSCREEN_INT);

    _tsFlag = false;
    power(false);
    _tsInitDone = false;
}


#endif