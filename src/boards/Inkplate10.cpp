/**
 **************************************************
 *
 * @file        Inkplate10.cpp
 * @brief       Basic funtions for controling inkplate 10
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

#include "../Inkplate.h"
#include "../include/Graphics.h"
#include "../include/defines.h"
#include "EEPROM.h"

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)

/**
 * @brief       begin function initialize Inkplate object with predefined
 * settings
 *
 * @param       uint8_t lightWaveform
 *              if inkplate doesn't work well or if it is fading after turning off
 *              lightWaveform should be set to 1 in order to fix that, but older boards
 *              may not support it
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
bool Inkplate::begin()
{
    if (_beginDone == 1)
        return 0;
    Wire.begin();
    EEPROM.begin(512);

    if (!getWaveformFromEEPROM(&waveformEEPROM) || waveformEEPROM.waveformId < INKPLATE10_WAVEFORM1 ||
        waveformEEPROM.waveformId > INKPLATE10_WAVEFORM3)
    {
        Serial.println("Wavefrom load failed! Upload new waveform in EEPROM. Using default waveform.");
        uint8_t defaultWaveform[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0},
                                         {0, 0, 2, 1, 1, 2, 2, 1, 0}, {0, 1, 2, 2, 1, 2, 2, 1, 0},
                                         {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0},
                                         {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};
        memcpy(waveform3Bit, defaultWaveform, sizeof(waveform3Bit));
    }
    else
    {
        memcpy(waveform3Bit, waveformEEPROM.waveform, sizeof(waveform3Bit));
    }

#if !defined(ARDUINO_INKPLATECOLOR) || !defined(ARDUINO_INKPLATE2)
    for (uint32_t i = 0; i < 256; ++i)
        pinLUT[i] = ((i & B00000011) << 4) | (((i & B00001100) >> 2) << 18) | (((i & B00010000) >> 4) << 23) |
                    (((i & B11100000) >> 5) << 25);
#endif

#ifdef ARDUINO_ESP32_DEV
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, HIGH);
#else
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, LOW);
#endif

    memset(ioRegsInt, 0, 22);
    memset(ioRegsEx, 0, 22);

    ioBegin(IO_INT_ADDR, ioRegsInt);
    ioBegin(IO_EXT_ADDR, ioRegsEx);

    pinModeInternal(IO_INT_ADDR, ioRegsInt, VCOM, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, PWRUP, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, GPIO0_ENABLE, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, GPIO0_ENABLE, HIGH);

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
        pinModeInternal(IO_EXT_ADDR, ioRegsEx, i, OUTPUT);
        digitalWriteInternal(IO_EXT_ADDR, ioRegsEx, i, LOW);
    }

    // For same reason, unused pins of first I/O expander have to be also set as
    // outputs, low.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 14, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 15, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 14, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 15, LOW);

#ifdef ARDUINO_INKPLATE10V2
    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);

    // And also disable uSD card supply
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, INPUT);
#else
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, LOW);
#endif

    // CONTROL PINS
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, OE, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, GMOD, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SPV, OUTPUT);

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
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 10, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 11, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 12, INPUT);

    // Battery voltage Switch MOSFET
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 9, OUTPUT);

    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
    GLUT = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
    {
        return 0;
    }
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    calculateLUTs();

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
        int x = x0 >> 3;
        int x_sub = x0 & 7;
        uint8_t temp = *(_partial + ((E_INK_WIDTH >> 3) * y0) + x);
        *(_partial + (E_INK_WIDTH / 8 * y0) + x) = (~pixelMaskLUT[x_sub] & temp) | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        color &= 7;
        int x = x0 >> 1;
        int x_sub = x0 & 1;
        uint8_t temp;
        temp = *(DMemory4Bit + (E_INK_WIDTH >> 1) * y0 + x);
        *(DMemory4Bit + (E_INK_WIDTH >> 1) * y0 + x) = (pixelMaskGLUT[x_sub] & temp) | (x_sub ? color : color << 4);
    }
}

/**
 *
 * @brief       display1b function writes black and white data to display
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 */
void Inkplate::display1b(bool leaveOn)
{
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    uint32_t _pos;
    uint8_t data;
    uint8_t dram;
    uint8_t _repeat;

    if (!einkOn())
        return;

    if (waveformEEPROM.waveformId != INKPLATE10_WAVEFORM1)
    {
        clean(0, 1);
        clean(1, 12);
        clean(2, 1);
        clean(0, 9);
        clean(2, 1);
        clean(1, 12);
        clean(2, 1);
        clean(0, 9);
        _repeat = 3;
    }
    else
    {
        clean(0, 1);
        clean(1, 10);
        clean(2, 1);
        clean(0, 10);
        clean(2, 1);
        clean(1, 10);
        clean(2, 1);
        clean(0, 10);
        _repeat = 5;
    }

    for (int k = 0; k < _repeat; k++)
    {
        _pos = (E_INK_HEIGHT * E_INK_WIDTH / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            dram = (*(DMemoryNew + _pos));
            data = LUTB[(dram >> 4) & 0x0F];
            hscan_start(pinLUT[data]);
            data = LUTB[dram & 0x0F];
            GPIO.out_w1ts = pinLUT[data] | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
            {
                dram = (*(DMemoryNew + _pos));
                data = LUTB[(dram >> 4) & 0x0F];
                GPIO.out_w1ts = pinLUT[data] | CL;
                GPIO.out_w1tc = DATA | CL;
                data = LUTB[dram & 0x0F];
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

    clean(2, 2);
    clean(3, 1);

    vscan_start();
    if (!leaveOn)
        einkOff();
    _blockPartial = 0;
}

/**
 * @brief       display3b function writes grayscale data to display
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 */
void IRAM_ATTR Inkplate::display3b(bool leaveOn)
{
    if (!einkOn())
        return;

    if (waveformEEPROM.waveformId != INKPLATE10_WAVEFORM1)
    {
        clean(1, 1);
        clean(0, 7);
        clean(2, 1);
        clean(1, 12);
        clean(2, 1);
        clean(0, 7);
        clean(2, 1);
        clean(1, 12);
    }
    else
    {
        clean(1, 1);
        clean(0, 10);
        clean(2, 1);
        clean(1, 10);
        clean(2, 1);
        clean(0, 10);
        clean(2, 1);
        clean(1, 10);
    }

    for (int k = 0; k < 9; k++)
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
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 *
 * @note        Partial update only works in black and white mode
 *
 * @return      Number of pixels changed from black to white, leaving blur
 */
uint32_t Inkplate::partialUpdate(bool _forced, bool leaveOn)
{
    if (getDisplayMode() == 1)
        return 0;
    if (_blockPartial == 1 && !_forced)
    {
        display1b(leaveOn);
        return 0;
    }

    uint32_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    uint32_t _send;
    uint8_t data = 0;
    uint8_t diffw, diffb;
    uint32_t n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;
    uint8_t _repeat;

    uint32_t changeCount = 0;

    for (int i = 0; i < E_INK_HEIGHT; ++i)
    {
        for (int j = 0; j < E_INK_WIDTH / 8; ++j)
        {
            diffw = *(DMemoryNew + _pos) & ~*(_partial + _pos);
            diffb = ~*(DMemoryNew + _pos) & *(_partial + _pos);
            if (diffw) // count pixels turning from black to white as these are visible blur
            {
                for (int bv = 1; bv < 256; bv <<= 1)
                {
                    if (diffw & bv)
                        ++changeCount;
                }
            }
            _pos--;
            *(_pBuffer + n) = LUTW[diffw >> 4] & (LUTB[diffb >> 4]);
            n--;
            *(_pBuffer + n) = LUTW[diffw & 0x0F] & (LUTB[diffb & 0x0F]);
            n--;
        }
    }

    if (!einkOn())
        return 0;

    if (waveformEEPROM.waveformId != INKPLATE10_WAVEFORM1)
    {
        _repeat = 4;
    }
    else
    {
        _repeat = 5;
    }

    for (int k = 0; k < _repeat; ++k)
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
    clean(2, 2);
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();

    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    return changeCount;
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

/**
 * @brief       Function allows grayscale waveform to be changed
 *
 * @param       uint8_t *_wf
 *              Waveform array with 8 rows where every row represents one color and 9 columns where every column
 * represents one phase or frame of each color.
 */
void Inkplate::changeWaveform(uint8_t *_wf)
{
    memcpy(waveform3Bit, _wf, sizeof(waveform3Bit));
    calculateLUTs();
}

/**
 * @brief       Calculation of LUTs for fast conversion pixels to waveform
 */
void Inkplate::calculateLUTs()
{
    for (int j = 0; j < 9; ++j)
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
}

/**
 * @brief       Function calculates checksum of wavefrom data read from EEPROM
 *
 * @param       struct waveformData _w
 *              Structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 *
 * @return      Value of checksum from data read from EEPROM
 */
uint8_t Inkplate::calculateChecksum(struct waveformData _w)
{
    uint8_t *_d = (uint8_t *)&_w;
    uint16_t _sum = 0;
    int _n = sizeof(struct waveformData) - 1;

    for (int i = 0; i < _n; i++)
    {
        _sum += _d[i];
    }
    return _sum % 256;
}

/**
 * @brief       Function reads waveform data from EEPROM and checks it's validity.
 *
 * @param       struct waveformData *_w
 *              Pointer to structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 *
 * @return      True if data is vaild, false if not
 */
bool Inkplate::getWaveformFromEEPROM(struct waveformData *_w)
{
    uint8_t *_ptr = (uint8_t *)_w;
    for (int i = 0; i < sizeof(struct waveformData); i++)
    {
        _ptr[i] = EEPROM.read(i);
    }

    return (calculateChecksum(*_w) != _w->checksum) ? false : true;
}

/**
 * @brief       Function writes waveform data to EEPROM
 *
 * @param       struct waveformData *_w
 *              Structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 */
void Inkplate::burnWaveformToEEPROM(struct waveformData _w)
{
    uint8_t *_ptr = (uint8_t *)&_w;
    for (int i = 0; i < sizeof(struct waveformData); i++)
    {
        EEPROM.write(i, _ptr[i]);
    }
    EEPROM.commit();
}

#endif