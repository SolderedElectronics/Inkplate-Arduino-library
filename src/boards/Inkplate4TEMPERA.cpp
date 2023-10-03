/**
 **************************************************
 *
 * @file        Inkplate4TEMPERA.cpp
 * @brief       Basic funtions for controlling Inkplate 4TEMPERA
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

#ifdef ARDUINO_INKPLATE4TEMPERA

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
    case 0:
        _swap_int16_t(x0, y0);
        x0 = height() - x0 - 1;
        break;
    case 1:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 2:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    if (getDisplayMode() == 0)
    {
        int x = x0 / 8;
        int x_sub = x0 % 8;
        uint8_t temp = *(_partial + (E_INK_WIDTH / 8 * y0) + x); // DMemoryNew[99 * y0 + x];
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

    // Turn off the buzzer so it doesn't beep
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, HIGH);

#ifndef ARDUINO_INKPLATECOLOR
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

    // Set the rest of the internal GPIO expander pins
    pinModeInternal(IO_INT_ADDR, ioRegsInt, INT_APDS, INPUT_PULLUP); // Gesture interrupt pin
    pinModeInternal(IO_INT_ADDR, ioRegsInt, INT2_LSM, INPUT_PULLUP); // LSM interrupt pins
    pinModeInternal(IO_INT_ADDR, ioRegsInt, INT1_LSM, INPUT_PULLUP);
    
    pinModeInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, OUTPUT); // Buzzer enable
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, BUZZ_EN, HIGH);

    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, LOW);

    pinModeInternal(IO_INT_ADDR, ioRegsInt, FRONTLIGHT_EN, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, FRONTLIGHT_EN, LOW);
    
    for (int i = 2; i < 15; i++)
    {
        pinModeInternal(IO_EXT_ADDR, ioRegsEx, i, OUTPUT);
        digitalWriteInternal(IO_EXT_ADDR, ioRegsEx, i, LOW);
    }

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

    // Put all the sensors to sleep
    sleepPeripheral(INKPLATE_BUZZER | INKPLATE_APDS9960 | INKPLATE_BME688 | INKPLATE_ACCELEROMETER |
                    INKPLATE_FUEL_GAUGE);

    _beginDone = 1;
    return 1;
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
    uint8_t data;
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
    else
    {
        data = 0;
    }

    uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) |
                     (((data & B11100000) >> 5) << 25);

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

    uint32_t _send;
    uint8_t data;
    uint8_t dram;

    if (!einkOn())
        return;

    clean(0, 5);
    clean(1, 15);
    clean(0, 15);
    clean(1, 15);
    clean(0, 15);

    for (int k = 0; k < 10; ++k)
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

    clean(2, 1);
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
void Inkplate::display3b(bool leaveOn)
{
    if (!einkOn())
        return;

    clean(0, 5);
    clean(1, 15);
    clean(0, 15);
    clean(1, 15);
    clean(0, 15);
    for (int k = 0; k < 8; ++k)
    {
        uint8_t *dp = DMemory4Bit + E_INK_WIDTH * E_INK_HEIGHT / 2;

        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            uint32_t t = GLUT2[k * 256 + (*(--dp))];
            t |= GLUT[k * 256 + (*(--dp))];
            hscan_start(t);
            t = GLUT2[k * 256 + (*(--dp))];
            t |= GLUT[k * 256 + (*(--dp))];
            GPIO.out_w1ts = t | CL;
            GPIO.out_w1tc = DATA | CL;

            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j)
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
    // uint32_t _send;
    uint8_t data;
    uint8_t diffw, diffb;
    uint32_t n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;

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

    for (int k = 0; k < 9; k++)
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

    // for (int k = 0; k < 60; ++k)
    // {
    //     uint8_t _send = B11111111;
    //     vscan_start();

    //     writeRow(_send);
    //     for (int i = 0; i < E_INK_HEIGHT / 2; i++)
    //     {
    //         hscan_start(pinLUT[_send]);
    //         delayMicroseconds(1);
    //         vscan_end();
    //     }

    //     _send = B01010101;

    //     writeRow(_send);
    //     for (int i = 0; i < E_INK_HEIGHT / 2; i++)
    //     {
    //         hscan_start(pinLUT[_send]);
    //         delayMicroseconds(1);
    //         vscan_end();
    //     }
    // }

    clean(2, 2);
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    return changeCount;
}

/**
 * @brief       Wake a single or multiple peripherals for Inkplate 4TEMPERA.
 *              They are in deep sleep by default so this function MUST be called
 *
 * @param       uint8_t _peripheral
 *              The flag which peripheral to wake from deep sleep, you may use:
 *              INKPLATE_ACCELEROMETER, INKPLATE_BME688, INKPLATE_APDS9960 and
 *              INKPLATE_FUEL_GAUGE
 *
 * @return      None
 */
void Inkplate::wakePeripheral(uint8_t _peripheral)
{
    if (_peripheral & INKPLATE_ACCELEROMETER)
    {
        // Wake accelerometer
        uint8_t accControlReg;
        lsm6ds3.readRegister(&accControlReg, 0x13);
        lsm6ds3.writeRegister(0x13, accControlReg & 0xBF);
        delay(2);
        lsm6ds3.beginCore();
    }

    if (_peripheral & INKPLATE_BME688)
    {
        // Wake BME
        uint8_t bmeControlReg = bme688.readByte(BME_CONTROL_ADDR);
        bme688.putData(BME_CONTROL_ADDR, bmeControlReg | 0x01);
        bme688.begin();
    }

    if (_peripheral & INKPLATE_APDS9960)
    {
        // Wake APDS
        //apds9960.enablePower();
        //
        // TODO apds9960.begin();
    }

    if (_peripheral & INKPLATE_FUEL_GAUGE)
    {
        uint8_t fuelGaugeControlReg1 = battery.readBlockData(0x3A);
        uint8_t fuelGaugeControlReg2 = battery.readBlockData(0x3B);
        fuelGaugeControlReg2 &= 0xCF;
        uint16_t fuelGaugeControlRegToWrite = (fuelGaugeControlReg1 << 8) | fuelGaugeControlReg2;
        battery.writeOpConfig(fuelGaugeControlRegToWrite);
    }
}

/**
 * @brief       Put a peripheral back to sleep, they are in sleep by default
 *
 * @param       uint8_t _peripheral
 *              The flag which peripheral to put to deep sleep, you may use:
 *              INKPLATE_ACCELEROMETER, INKPLATE_BME688, INKPLATE_APDS9960 and
 *              INKPLATE_FUEL_GAUGE
 *
 * @return      None
 */
void Inkplate::sleepPeripheral(uint8_t _peripheral)
{
    if (_peripheral & INKPLATE_ACCELEROMETER)
    {
        // Put accelerometer in sleep mode
        uint8_t accControlReg;
        lsm6ds3.readRegister(&accControlReg, 0x13);
        lsm6ds3.writeRegister(0x13, accControlReg | 0x04);
    }

    if (_peripheral & INKPLATE_BME688)
    {
        // Put BME in sleep mode
        uint8_t bmeControlReg = bme688.readByte(BME_CONTROL_ADDR);
        bme688.putData(BME_CONTROL_ADDR, bmeControlReg & 0xFC);
    }

    if (_peripheral & INKPLATE_APDS9960)
    {
        // Put APDS in sleep mode
        // TODO
    }

    if (_peripheral & INKPLATE_FUEL_GAUGE)
    {
        uint8_t fuelGaugeControlReg1 = battery.readBlockData(0x3A);
        uint8_t fuelGaugeControlReg2 = battery.readBlockData(0x3B);
        fuelGaugeControlReg2 |= 0x20;
        uint16_t fuelGaugeControlRegToWrite = (fuelGaugeControlReg1 << 8) | fuelGaugeControlReg2;
        battery.writeOpConfig(fuelGaugeControlRegToWrite);
    }
}

#endif
