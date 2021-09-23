/**
 **************************************************
 *
 * @file        Inkplate5.cpp
 * @brief       Basic funtions for controling inkplate 5
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
 * @authors     @ e-radionica.com
 ***************************************************/

#include "../Inkplate.h"
#include "../include/Graphics.h"
#include "../include/defines.h"

#ifdef ARDUINO_INKPLATE5

/**
 * @brief       begin function initialize Inkplate object with predefined
 * settings
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
bool Inkplate::begin(void)
{
    if (_beginDone == 1)
        return 0;

    Wire.begin();
    memset(mcpRegsInt, 0, 22);
    memset(mcpRegsEx, 0, 22);
    mcpBegin(MCP23017_INT_ADDR, mcpRegsInt);
    mcpBegin(MCP23017_EXT_ADDR, mcpRegsEx);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, VCOM, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, PWRUP, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, WAKEUP, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, GPIO0_ENABLE, OUTPUT);
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, GPIO0_ENABLE, HIGH);

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

    // Set all pins of seconds I/O expander to outputs, low.
    // For some reason, it draw more current in deep sleep when pins are set as
    // inputs...
    for (int i = 0; i < 15; i++)
    {
        pinModeInternal(MCP23017_EXT_ADDR, mcpRegsEx, i, OUTPUT);
        digitalWriteInternal(MCP23017_EXT_ADDR, mcpRegsEx, i, LOW);
    }

    // For same reason, unused pins of first I/O expander have to be also set as
    // outputs, low.
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 13, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 14, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 15, OUTPUT);
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, 13, LOW);
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, 14, LOW);
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, 15, LOW);

    // CONTROL PINS
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, OUTPUT);

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
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 10, INPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 11, INPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 12, INPUT);

    // Battery voltage Switch MOSFET
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, 9, OUTPUT);

    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
    GLUT = (uint32_t *)malloc(256 * 8 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 8 * sizeof(uint32_t));
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
    {
        return 0;
    }
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    for (int j = 0; j < 8; ++j)
    {
        for (uint32_t i = 0; i < 256; ++i)
        {
            uint8_t z = (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
            GLUT[j * 256 + i] = ((z & B00000011) << 4) | (((z & B00001100) >> 2) << 18) |
                                (((z & B00010000) >> 4) << 23) | (((z & B11100000) >> 5) << 25);
            z = ((waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j])) << 4;
            GLUT2[j * 256 + i] = ((z & B00000011) << 4) | (((z & B00001100) >> 2) << 18) |
                                 (((z & B00010000) >> 4) << 23) | (((z & B11100000) >> 5) << 25);
        }
    }

    _beginDone = 1;
    return 1;
}

/**
 *
 * @brief       writePixel funtion sets pixel data for (x, y) pixel position
 *
 * @param       int16_t x0
 *              default position for x, will be changed depending on rotation
 * @param       int16_t y0
 *              default position for y, will be changed depending on rotation
 * @param       uint16_t color
 *              pixel color, in 3bit mode have values in range 0-7
 *
 * @note        If x0 or y0 are out of inkplate screen borders, function will
 * exit.
 */
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
        int x = x0 / 8;
        int x_sub = x0 % 8;
        uint8_t temp = *(_partial + (E_INK_WIDTH / 8 * y0) + x);
        *(_partial + (E_INK_WIDTH / 8 * y0) + x) = (~pixelMaskLUT[x_sub] & temp) | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        color &= 7;
        int x = x0 / 2;
        int x_sub = x0 % 2;
        uint8_t temp;
        temp = *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + x);
        *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + x) = (pixelMaskGLUT[x_sub] & temp) | (x_sub ? color : color << 4);
    }
}

/**
 *
 * @brief       display1b function writes black and white data to display
 */
void Inkplate::display1b(bool leaveOn)
{
    for (int i = 0; i < (E_INK_HEIGHT * E_INK_WIDTH) / 8; i++)
    {
        *(DMemoryNew + i) &= *(_partial + i);
        *(DMemoryNew + i) |= (*(_partial + i));
    }
    uint32_t _pos;
    uint8_t data;
    uint8_t dram;


    if (!leaveOn && !einkOn())
    {
        return;
    }

    clean(0, 17);
    clean(1, 17);
    clean(0, 17);
    clean(1, 17);
    for (int k = 0; k < 4; k++)
    {
        _pos = (E_INK_HEIGHT * E_INK_WIDTH / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            dram = ~(*(DMemoryNew + _pos));
            data = LUTW[(dram >> 4) & 0x0F];
            hscan_start(pinLUT[data]);
            data = LUTW[dram & 0x0F];
            GPIO.out_w1ts = pinLUT[data] | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
            {
                dram = ~(*(DMemoryNew + _pos));
                data = LUTW[(dram >> 4) & 0x0F];
                GPIO.out_w1ts = pinLUT[data] | CL;
                GPIO.out_w1tc = DATA | CL;
                data = LUTW[dram & 0x0F];
                GPIO.out_w1ts = pinLUT[data] | CL;
                GPIO.out_w1tc = DATA | CL;
                _pos--;
            }
            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }

    _pos = (E_INK_HEIGHT * E_INK_WIDTH / 8) - 1;
    vscan_start();
    for (int i = 0; i < E_INK_HEIGHT; i++)
    {
        dram = *(DMemoryNew + _pos);
        data = LUT2[(dram >> 4) & 0x0F];
        hscan_start(pinLUT[data]);
        data = LUT2[dram & 0x0F];
        GPIO.out_w1ts = (pinLUT[data]) | CL;
        GPIO.out_w1tc = DATA | CL;
        _pos--;
        for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
        {
            dram = *(DMemoryNew + _pos);
            data = LUT2[(dram >> 4) & 0x0F];
            GPIO.out_w1ts = (pinLUT[data]) | CL;
            GPIO.out_w1tc = DATA | CL;
            data = LUT2[dram & 0x0F];
            GPIO.out_w1ts = (pinLUT[data]) | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
        }
        GPIO.out_w1ts = CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }
    delayMicroseconds(230);

    clean(2, 2);
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();
    _blockPartial = 0;
}

/**
 * @brief       display3b function writes grayscale data to display
 */
void IRAM_ATTR Inkplate::display3b(bool leaveOn)
{
    if (!leaveOn && !einkOn())
    {
        return;
    }
    clean(0, 17);
    clean(1, 17);
    clean(0, 17);
    clean(1, 17);

    for (int k = 0; k < 8; k++)
    {
        uint8_t *dp = DMemory4Bit + (E_INK_HEIGHT * E_INK_WIDTH / 2);

        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            uint32_t t = GLUT2[k * 256 + (*(--dp))];
            t |= GLUT[k * 256 + (*(--dp))];
            hscan_start(t);
            t = GLUT2[k * 256 + (*(--dp))];
            t |= GLUT[k * 256 + (*(--dp))];
            GPIO.out_w1ts = t | CL;
            GPIO.out_w1tc = DATA | CL;

            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
            {
                t = GLUT2[k * 256 + (*(--dp))];
                t |= GLUT[k * 256 + (*(--dp))];
                GPIO.out_w1ts = t | CL;
                GPIO.out_w1tc = DATA | CL;
                t = GLUT2[k * 256 + (*(--dp))];
                t |= GLUT[k * 256 + (*(--dp))];
                GPIO.out_w1ts = t | CL;
                GPIO.out_w1tc = DATA | CL;
            }

            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();
}

/**
 * @brief       partialUpdate function updates changed parts of the screen
 * without need to refresh whole display
 *
 * @param       bool _forced
 *              For advanced use with deep sleep. Can force partial update in
 * deep sleep
 *
 * @note        Partial update only works in black and white mode
 */
void Inkplate::partialUpdate(bool _forced, bool leaveOn)
{
    if (getDisplayMode() == 1)
        return;

    if (_blockPartial == 1 && !_forced)
    {
        display1b(leaveOn);
        return;
    }
    uint32_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    uint8_t data;
    uint8_t diffw, diffb;
    uint32_t n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;

    for (int i = 0; i < E_INK_HEIGHT; i++)
    {
        for (int j = 0; j < E_INK_WIDTH / 8; j++)
        {
            diffw = ((*(DMemoryNew + _pos)) ^ (*(_partial + _pos))) & (~(*(_partial + _pos)));
            diffb = ((*(DMemoryNew + _pos)) ^ (*(_partial + _pos))) & ((*(_partial + _pos)));
            _pos--;
            *(_pBuffer + n) = LUTW[diffw >> 4] & (LUTB[diffb >> 4]);
            n--;
            *(_pBuffer + n) = LUTW[diffw & 0x0F] & (LUTB[diffb & 0x0F]);
            n--;
        }
    }

    if (!leaveOn)
    {
        if (!einkOn())
        {
            return;
        }
    }

    for (int k = 0; k < 5; k++)
    {
        vscan_start();
        n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            data = *(_pBuffer + n);
            hscan_start(pinLUT[data]);
            n--;
            for (int j = 0; j < ((E_INK_WIDTH / 4) - 1); j++)
            {
                data = *(_pBuffer + n);
                GPIO.out_w1ts = (pinLUT[data]) | CL;
                GPIO.out_w1tc = DATA | CL;
                n--;
            }
            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    clean(2, 2);
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();
    for (int i = 0; i < (E_INK_WIDTH * E_INK_HEIGHT / 8); i++)
    {
        *(DMemoryNew + i) &= *(_partial + i);
        *(DMemoryNew + i) |= (*(_partial + i));
    }
}

/**
 * @brief       clean function cleans screen of any potential burn in
 *
 *              Based on c param it will: if c=0 light screen, c=1 darken the
 * screen, c=2 discharge the screen or 3 skip all pixels
 *
 * @param       uint8_t c
 *              one of four posible pixel states
 *
 * @param       uint8_t rep
 *              Number of repetitions
 *
 *
 * @note        Should not be used in intervals smaller than 5 seconds
 */
void Inkplate::clean(uint8_t c, uint8_t rep)
{
    einkOn();
    uint8_t data = 0;
    if (c == 0)
    {
        data = B10101010; // White
    }
    else if (c == 1)
    {
        data = B01010101; // Black
    }
    else if (c == 2)
    {
        data = B00000000; // Discharge
    }
    else if (c == 3)
    {
        data = B11111111; // Skip
    }

    uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) |
                     (((data & B11100000) >> 5) << 25);
    ;
    for (int k = 0; k < rep; k++)
    {
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            hscan_start(_send);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = CL;
            for (int j = 0; j < (E_INK_WIDTH / 8) - 1; j++)
            {
                GPIO.out_w1ts = CL;
                GPIO.out_w1tc = CL;
                GPIO.out_w1ts = CL;
                GPIO.out_w1tc = CL;
            }
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
}
#endif