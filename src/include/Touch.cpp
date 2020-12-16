#include "Touch.h"

#ifdef ARDUINO_INKPLATE6PLUS

uint8_t Touch::tsWriteRegs(uint8_t _addr, const uint8_t *_buff, uint8_t _size)
{
    Wire.beginTransmission(_addr);
    Wire.write(_buff, _size);
    return Wire.endTransmission();
}

void Touch::tsReadRegs(uint8_t _addr, uint8_t *_buff, uint8_t _size)
{
    Wire.requestFrom(_addr, _size);
    Wire.readBytes(_buff, _size);
}

void Touch::tsHardwareReset()
{
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, TS_RTS, LOW);
    delay(15);
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, TS_RTS, HIGH);
    delay(15);
}

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

bool Touch::tsInit(uint8_t _pwrState)
{
    // Enable power to TS
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, TOUCHSCREEN_EN, LOW);

    pinMode(TS_INT, INPUT_PULLUP);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, TS_RTS, OUTPUT);
    attachInterrupt(TS_INT, tsInt, FALLING);
    tsHardwareReset();
    if (!tsSoftwareReset())
    {
        detachInterrupt(TS_INT);
        return false;
    }
    tsGetResolution(&_tsXResolution, &_tsYResolution);
    tsSetPowerState(_pwrState);
    return true;
}

void Touch::tsShutdown()
{
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, TOUCHSCREEN_EN, HIGH);
}

void Touch::tsGetRawData(uint8_t *b)
{
    Wire.requestFrom(TS_ADDR, 8);
    Wire.readBytes(b, 8);
}

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
    }
    return fingers;
}

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

void Touch::tsSetPowerState(uint8_t _s)
{
    _s &= 1;
    uint8_t powerStateReg[] = {0x54, 0x50, 0x00, 0x01};
    powerStateReg[1] |= (_s << 3);
    tsWriteRegs(TS_ADDR, powerStateReg, 4);
}

uint8_t Touch::tsGetPowerState()
{
    const uint8_t powerStateReg[] = {0x53, 0x50, 0x00, 0x01};
    uint8_t buf[4];
    tsWriteRegs(TS_ADDR, powerStateReg, 4);
    _tsFlag = false;
    tsReadRegs(TS_ADDR, buf, 4);
    return (buf[1] >> 3) & 1;
}

bool Touch::tsAvailable()
{
    return _tsFlag;
}

#endif