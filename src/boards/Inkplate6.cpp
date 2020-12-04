#include "../Inkplate.h"
#include "../include/Graphics.h"
#include "../include/defines.h"

#ifdef ARDUINO_ESP32_DEV
void Inkplate::begin(void)
{
    if (_beginDone == 1)
        return;

    Wire.begin();
    memset(mcpRegsInt, 0, 22);
    mcpBegin(MCP23017_ADDR, mcpRegsInt);
    pinModeMCP(VCOM, OUTPUT);
    pinModeMCP(PWRUP, OUTPUT);
    pinModeMCP(WAKEUP, OUTPUT);
    pinModeMCP(GPIO0_ENABLE, OUTPUT);
    digitalWriteMCP(GPIO0_ENABLE, HIGH);

    WAKEUP_SET;
    delay(1);
    Wire.beginTransmission(0x48);
    Wire.write(0x09);
    Wire.write(B00011011); // Power up seq.
    Wire.write(B00000000); // Power up delay (3mS per rail)
    Wire.write(B00011011); // Power down seq.
    Wire.write(B00000000); // Power down delay (6mS per rail)
    Wire.endTransmission();
    delay(1);
    WAKEUP_CLEAR;

    // CONTROL PINS
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeMCP(OE, OUTPUT);
    pinModeMCP(GMOD, OUTPUT);
    pinModeMCP(SPV, OUTPUT);

    // DATA PINS
    pinMode(4, OUTPUT); // D0
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT); // D7

    // TOUCHPAD PINS
    pinModeMCP(10, INPUT);
    pinModeMCP(11, INPUT);
    pinModeMCP(12, INPUT);

    // Battery voltage Switch MOSFET
    pinModeMCP(9, OUTPUT);

    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
    GLUT = (uint32_t *)malloc(256 * 8 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 8 * sizeof(uint32_t));

    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
    {
        do
        {
            delay(100);
        } while (true);
    }
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    for (int i = 0; i < 8; ++i)
    {
        for (uint32_t j = 0; j < 256; ++j)
        {
            uint8_t z = (waveform3Bit[j & 0x07][i] << 2) | (waveform3Bit[(j >> 4) & 0x07][i]);
            GLUT[i * 256 + j] = ((z & B00000011) << 4) | (((z & B00001100) >> 2) << 18) |
                                (((z & B00010000) >> 4) << 23) | (((z & B11100000) >> 5) << 25);
            z = ((waveform3Bit[j & 0x07][i] << 2) | (waveform3Bit[(j >> 4) & 0x07][i])) << 4;
            GLUT2[i * 256 + j] = ((z & B00000011) << 4) | (((z & B00001100) >> 2) << 18) |
                                 (((z & B00010000) >> 4) << 23) | (((z & B11100000) >> 5) << 25);
        }
    }

    _beginDone = 1;
}

void Graphics::writePixel(int16_t x0, int16_t y0, uint16_t color)
{
    if (x0 > width() - 1 || y0 > height() - 1 || x0 < 0 || y0 < 0)
        return;

    switch (rotation)
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = height() - x0 - 1;
        break;
    case 2:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    if (getDisplayMode() == 0)
    {
        int x = x0 >> 3;
        int x_sub = x0 & 7;
        uint8_t temp = *(_partial + 100 * y0 + x);
        *(_partial + 100 * y0 + x) = (~pixelMaskLUT[x_sub] & temp) | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        color &= 7;
        int x = x0 >> 1;
        int x_sub = x0 & 1;
        uint8_t temp;
        temp = *(DMemory4Bit + 400 * y0 + x);
        *(DMemory4Bit + 400 * y0 + x) = (pixelMaskGLUT[x_sub] & temp) | (x_sub ? color : color << 4);
    }
}


void Inkplate::display1b()
{
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    uint32_t _send;
    uint8_t data;
    uint8_t dram;
    einkOn();

    cleanFast(0, 1);
    cleanFast(1, 21);
    cleanFast(2, 1);
    cleanFast(0, 12);
    cleanFast(2, 1);
    cleanFast(1, 21);
    cleanFast(2, 1);
    cleanFast(0, 12);

    for (int k = 0; k < 4; ++k)
    {
        uint8_t *DMemoryNewPtr = DMemoryNew + (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            dram = *(DMemoryNewPtr--);
            data = LUTB[dram >> 4];
            _send = pinLUT[data];
            hscan_start(_send);
            data = LUTB[dram & 0x0F];
            _send = pinLUT[data];
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;

            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
            {
                dram = *(DMemoryNewPtr--);
                data = LUTB[dram >> 4];
                _send = pinLUT[data];
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
                data = LUTB[dram & 0x0F];
                _send = pinLUT[data];
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
            }
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }

    uint16_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    vscan_start();
    for (int i = 0; i < E_INK_HEIGHT; ++i)
    {
        dram = *(DMemoryNew + _pos);
        data = LUT2[dram >> 4];
        _send = pinLUT[data];
        hscan_start(_send);
        data = LUT2[dram & 0x0F];
        _send = pinLUT[data];
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        _pos--;
        for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
        {
            dram = *(DMemoryNew + _pos);
            data = LUT2[dram >> 4];
            _send = pinLUT[data];
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            data = LUT2[dram & 0x0F];
            _send = pinLUT[data];
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
        }
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }
    delayMicroseconds(230);

    vscan_start();
    for (int i = 0; i < E_INK_HEIGHT; ++i)
    {
        dram = *(DMemoryNew + _pos);
        data = 0;
        _send = pinLUT[data];
        hscan_start(_send);
        data = 0;
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
        {
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
        }
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }
    delayMicroseconds(230);

    vscan_start();
    einkOff();
    _blockPartial = 0;
}

void Inkplate::display3b()
{
    einkOn();
    cleanFast(0, 1);
    cleanFast(1, 21);
    cleanFast(2, 1);
    cleanFast(0, 12);
    cleanFast(2, 1);
    cleanFast(1, 21);
    cleanFast(2, 1);
    cleanFast(0, 12);
    for (int k = 0; k < 8; ++k)
    {
        uint8_t *dp = DMemory4Bit + E_INK_WIDTH * E_INK_HEIGHT / 2;

        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            hscan_start((GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]));
            GPIO.out_w1ts = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]) | CL;
            GPIO.out_w1tc = DATA | CL;

            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
            {
                GPIO.out_w1ts = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]) | CL;
                GPIO.out_w1tc = DATA | CL;
                GPIO.out_w1ts = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]) | CL;
                GPIO.out_w1tc = DATA | CL;
            }
            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    cleanFast(3, 1);
    vscan_start();
    einkOff();
}

void Inkplate::partialUpdate(bool _forced)
{
    if (getDisplayMode() == 1)
        return;
    if (_blockPartial == 1 && !_forced)
    {
        display1b();
        return;
    }

    uint16_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    uint32_t _send;
    uint8_t data = 0;
    uint8_t diffw, diffb;
    uint32_t n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;

    for (int i = 0; i < E_INK_HEIGHT; ++i)
    {
        for (int j = 0; j < E_INK_WIDTH / 8; ++j)
        {
            diffw = *(DMemoryNew + _pos) & ~*(_partial + _pos);
            diffb = ~*(DMemoryNew + _pos) & *(_partial + _pos);
            _pos--;
            *(_pBuffer + n) = LUTW[diffw >> 4] & (LUTB[diffb >> 4]);
            n--;
            *(_pBuffer + n) = LUTW[diffw & 0x0F] & (LUTB[diffb & 0x0F]);
            n--;
        }
    }

    einkOn();
    for (int k = 0; k < 5; ++k)
    {
        vscan_start();
        n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            data = *(_pBuffer + n);
            _send = pinLUT[data];
            hscan_start(_send);
            n--;
            for (int j = 0; j < ((E_INK_WIDTH / 4) - 1); ++j)
            {
                data = *(_pBuffer + n);
                _send = pinLUT[data];
                GPIO.out_w1ts = _send | CL;
                GPIO.out_w1tc = DATA | CL;
                n--;
            }
            GPIO.out_w1ts = _send | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    cleanFast(2, 2);
    cleanFast(3, 1);
    vscan_start();
    einkOff();

    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);
}

void Inkplate::clean()
{
    einkOn();
    int m = 0;
    cleanFast(0, 1);
    m++;
    cleanFast((waveform[m] >> 30) & 3, 8);
    m++;
    cleanFast((waveform[m] >> 24) & 3, 1);
    m++;
    cleanFast((waveform[m]) & 3, 8);
    m++;
    cleanFast((waveform[m] >> 6) & 3, 1);
    m++;
    cleanFast((waveform[m] >> 30) & 3, 10);
}

void Inkplate::cleanFast(uint8_t c, uint8_t rep)
{
    einkOn();
    uint8_t data = 0;
    if (c == 0)
        data = B10101010;
    else if (c == 1)
        data = B01010101;
    else if (c == 2)
        data = B00000000;
    else if (c == 3)
        data = B11111111;

    uint32_t _send = pinLUT[data];
    for (int k = 0; k < rep; ++k)
    {
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            hscan_start(_send);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = CL;
            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
            {
                GPIO.out_w1ts = CL;
                GPIO.out_w1tc = CL;
                GPIO.out_w1ts = CL;
                GPIO.out_w1tc = CL;
            }
            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
}

#endif