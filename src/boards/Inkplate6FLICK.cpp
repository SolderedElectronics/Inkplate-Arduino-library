/**
 **************************************************
 *
 * @file        Inkplate6plus.cpp
 * @brief       Basic funtions for controling inkplate 6PLUS
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

#ifdef ARDUINO_INKPLATE6FLICK

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

    // Initialize Wire (I2C) library. Needed for I/O expander, RTC and ePaper PMIC.
    Wire.begin();

    // Clear registers for both I/O expanders.
    memset(ioRegsInt, 0, 22);
    memset(ioRegsEx, 0, 22);

    // Initialize both I/O expanders.
    ioBegin(IO_INT_ADDR, ioRegsInt);
    ioBegin(IO_EXT_ADDR, ioRegsEx);

    // Set control pins for ePaper PMIC to outputs.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, VCOM, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, PWRUP, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, OUTPUT);

    // Enable ePaper PMIC to set proper power up sequence.
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

    // To reduce power consumption in deep sleep, unused pins of
    // first I/O expander have to be also set as output with pulled low.
    // VBAT measurement MOSFET switch.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 9, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, TOUCHSCREEN_EN, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, TS_RST, OUTPUT);

    // Frontlight.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 13, OUTPUT);

    // Unused pins on internal GPIO expander.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 14, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 15, OUTPUT);

    // Disable touchscreen.
    tsInit(false);

    // Disable frontlight.
    frontlight(false);

    // Unused pins on internal GPIO expander.
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 14, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 15, LOW);

    // Enable pull down resistors on SPI lines to reduce power consumption in deep sleep.
    pinMode(12, INPUT_PULLDOWN);
    pinMode(13, INPUT_PULLDOWN);
    pinMode(14, INPUT_PULLDOWN);
    pinMode(15, INPUT_PULLDOWN);

    // And also disable uSD card supply
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, INPUT);

    // Control pins for ePaper.
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, OE, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, GMOD, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SPV, OUTPUT);

    // Use only I2S1.
    myI2S = &I2S1;

    // Allocate memory for DMA descriptor and line buffer.
    _dmaLineBuffer = (uint8_t *)heap_caps_malloc((E_INK_WIDTH / 4) + 16, MALLOC_CAP_DMA);
    _dmaI2SDesc = (lldesc_s *)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);

    // Check for successfull DMA allocation.
    if (_dmaLineBuffer == NULL || _dmaI2SDesc == NULL)
    {
        return 0;
    }

    // Init the I2S driver. It will setup a I2S driver.
    I2SInit(myI2S, 4);

    // Battery voltage Switch MOSFET
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 9, OUTPUT);

    // Allocate framebuffer for the upcomming framebuffer.
    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Buffer for pixel difference in partial update mode.
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Buffer for the pixel to EPD conversion.
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);

    // 3 bit memory buffer.
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);

    // LUT for fast pixel and wavefrom to EPD conversion.
    GLUT = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));

    // Check memory allocations. If any failed, return error.
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
    {
        return 0;
    }

    // Clean-up allocated memory buffers.
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    // Fill up the pixel to EPD LUT for 3 bit mode.
    for (int j = 0; j < 9; ++j)
    {
        for (int i = 0; i < 256; ++i)
        {
            GLUT[j * 256 + i] = (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
            GLUT2[j * 256 + i] = ((waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j])) << 4;
        }
    }

    // If everything went ok, set the flag to 1 (preventing memory leak if the begin is called multiple times).
    _beginDone = 1;

    // Return succes!
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
    uint8_t data = 0;
    if (c == 0)
        data = B10101010;
    else if (c == 1)
        data = B01010101;
    else if (c == 2)
        data = B00000000;
    else if (c == 3)
        data = B11111111;

    // Fill up the buffer with the data.
    for (int i = 0; i < (E_INK_WIDTH / 4); i++)
    {
        _dmaLineBuffer[i] = data;
    }

    _dmaI2SDesc->size = (E_INK_WIDTH / 4) + 16;
    _dmaI2SDesc->length = (E_INK_WIDTH / 4) + 16;
    _dmaI2SDesc->sosf = 1;
    _dmaI2SDesc->owner = 1;
    _dmaI2SDesc->qe.stqe_next = 0;
    _dmaI2SDesc->eof = 1;
    _dmaI2SDesc->buf = _dmaLineBuffer;
    _dmaI2SDesc->offset = 0;

    for (int k = 0; k < rep; ++k)
    {
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
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
    // Copy everything from partial buffer into main buffer.
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Helper variables.
    uint32_t _send;
    uint8_t data;
    uint8_t dram;

    // Try to power up ePaper power supply (TPS65186).
    // Cancel the screen refresh if power up failed.
    if (!einkOn())
        return;

    // Clear the screen (clear sequence).
    clean(0, 5);
    clean(1, 22);
    clean(2, 1);
    clean(0, 22);
    clean(2, 1);
    clean(1, 22);
    clean(2, 1);
    clean(0, 22);
    clean(2, 1);

    // Write only black pixels.
    for (int k = 0; k < 4; k++)
    {
        uint8_t *DMemoryNewPtr = DMemoryNew + (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            for (int n = 0; n < (E_INK_WIDTH / 4); n += 4)
            {
                uint8_t dram1 = *(DMemoryNewPtr);
                uint8_t dram2 = *(DMemoryNewPtr - 1);
                _dmaLineBuffer[n] = LUTB[(dram2 >> 4) & 0x0F];     // i + 2;
                _dmaLineBuffer[n + 1] = LUTB[dram2 & 0x0F];        // i + 3;
                _dmaLineBuffer[n + 2] = LUTB[(dram1 >> 4) & 0x0F]; // i;
                _dmaLineBuffer[n + 3] = LUTB[dram1 & 0x0F];        // i + 1;
                DMemoryNewPtr -= 2;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
    }

    // Now write both black and white pixels.
    for (int k = 0; k < 1; k++)
    {
        uint8_t *DMemoryNewPtr = DMemoryNew + (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            for (int n = 0; n < (E_INK_WIDTH / 4); n += 4)
            {
                uint8_t dram1 = *(DMemoryNewPtr);
                uint8_t dram2 = *(DMemoryNewPtr - 1);
                _dmaLineBuffer[n] = LUT2[(dram2 >> 4) & 0x0F];     // i + 2;
                _dmaLineBuffer[n + 1] = LUT2[dram2 & 0x0F];        // i + 3;
                _dmaLineBuffer[n + 2] = LUT2[(dram1 >> 4) & 0x0F]; // i;
                _dmaLineBuffer[n + 3] = LUT2[dram1 & 0x0F];        // i + 1;
                DMemoryNewPtr -= 2;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
        delayMicroseconds(230);
    }

    // Discharge sequence.
    for (int k = 0; k < 1; k++)
    {
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            for (int n = 0; n < (E_INK_WIDTH / 4); n += 4)
            {
                _dmaLineBuffer[n] = 0;
                _dmaLineBuffer[n + 1] = 0;
                _dmaLineBuffer[n + 2] = 0;
                _dmaLineBuffer[n + 3] = 0;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
    }

    // Leave the ePaper power supply on if needed.
    if (!leaveOn)
        einkOff();

    // Allow partial updates.
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
    // Try to power up ePaper power managment IC (TPS65186).
    // Cancel the screen update if failed.
    if (!einkOn())
        return;

    // Clear the screen (clear sequence).
    clean(0, 5);
    clean(1, 22);
    clean(2, 1);
    clean(0, 22);
    clean(2, 1);
    clean(1, 22);
    clean(2, 1);
    clean(0, 22);
    clean(2, 1);

    // Update the screen with new image by using custom waveform for the grayscale (can be found in Inkplate6FLICK.h file).
    for (int k = 0; k < 9; k++)
    {
        uint8_t *dp = DMemory4Bit + E_INK_WIDTH * E_INK_HEIGHT / 2;

        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            for (int j = 0; j < (E_INK_WIDTH / 4); j += 4)
            {
                _dmaLineBuffer[j + 2] = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]);
                _dmaLineBuffer[j + 3] = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]);
                _dmaLineBuffer[j] = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]);
                _dmaLineBuffer[j + 1] = (GLUT2[k * 256 + (*(--dp))] | GLUT[k * 256 + (*(--dp))]);
            }
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
    }

    // Set ePapaer drivers into HiZ state. 
    clean(3, 1);

    // Keep the ePaper supply enabled if needed.
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

    uint32_t changeCount = 0;

    _dmaI2SDesc->size = (E_INK_WIDTH / 4) + 16;
    _dmaI2SDesc->length = (E_INK_WIDTH / 4) + 16;
    _dmaI2SDesc->sosf = 1;
    _dmaI2SDesc->owner = 1;
    _dmaI2SDesc->qe.stqe_next = 0;
    _dmaI2SDesc->eof = 1;
    _dmaI2SDesc->buf = _dmaLineBuffer;
    _dmaI2SDesc->offset = 0;

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

    for (int k = 0; k < 4; k++)
    {
        vscan_start();
        n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            for (int j = 0; j < (E_INK_WIDTH / 4); j += 4)
            {
                _dmaLineBuffer[j + 2] = *(_pBuffer + n);
                _dmaLineBuffer[j + 3] = *(_pBuffer + n - 1);
                _dmaLineBuffer[j] = *(_pBuffer + n - 2);
                _dmaLineBuffer[j + 1] = *(_pBuffer + n - 3);
                n -= 4;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
    }
    clean(2, 2);
    clean(3, 1);
    vscan_start();

    if (!leaveOn)
        einkOff();

    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    return changeCount;
}

#endif
