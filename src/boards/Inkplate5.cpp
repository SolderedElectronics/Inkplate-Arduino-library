/**
 **************************************************
 *
 * @file        Inkplate6.cpp
 * @brief       Basic funtions for controling inkplate 6
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

/**
 * Includes
 */
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

    for (uint32_t i = 0; i < 256; ++i)
        pinLUT[i] = ((i & B00000011) << 4) | (((i & B00001100) >> 2) << 18) | (((i & B00010000) >> 4) << 23) |
                    (((i & B11100000) >> 5) << 25);


    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 9, LOW);

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

    // To reduce power consumption in deep sleep, unused pins of
    // first I/O expander have to be also set as inputs with pull down
    // resistors.
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 11, INPUT_PULLDOWN);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 12, INPUT_PULLDOWN);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 13, INPUT_PULLDOWN);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 14, INPUT_PULLDOWN);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, 15, INPUT_PULLDOWN);

    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);

    // And also disable uSD card supply
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, INPUT);

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

    // Allocate the memory for the 1 bit frame buffer
    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Allocate the memory for the partial update buffer
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);

    // Allocate the memory for the 3 bit buffer (each pixel is stored with 4 bits).
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);

    // Allocate the memory for the pixel to GPIO conversion
    GLUT = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));

    // Check if the each and every buffer is really allocated. If not, return error!
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
    {
        return 0;
    }

    // Init all the buffers
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    // Calcualte LUT for pixel to GPIO conversion.
    for (int i = 0; i < 9; ++i)
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

    // If everything went ok, set the flag to 1 (preventing memory leak if the begin is called multiple times).
    _beginDone = 1;

    // Return succes!
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

    // set x, y depending on selected rotation
    switch (rotation)
    {
    case 1: // 90 degree left
        _swap_int16_t(x0, y0);
        x0 = height() - x0 - 1;
        break;
    case 2: // 180 degree, or upside down
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 3: // 90 degree right
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    // If the 1 bit mode is used, pixels are packed 1 bit = 1 pixel in frame buffer
    if (getDisplayMode() == 0)
    {
        // Divide by 8 to find a byte.
        int x = x0 >> 3;

        // Get the remainder of the division to find a exact bit in the byte that needs to be modified.
        int x_sub = x0 & 7;

        // Save the currnet state of the byte in the frame buffer.
        uint8_t temp = *(_partial + (E_INK_WIDTH / 8) * y0 + x);

        // Modify the pixel. First clear the pixel by writing zero then write the 1 if the pixel is set.
        *(_partial + (E_INK_WIDTH / 8) * y0 + x) = (~pixelMaskLUT[x_sub] & temp) | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        // If 3 bit mode is used, constrain the color value (only 8 possible colors are available).
        color &= 7;

        // Divide by two to find a byte
        int x = x0 >> 1;

        //  Get the remainder of the division to find if the lower or upper 4 bits are needed.
        int x_sub = x0 & 1;

        // Store the current value of the byte.
        uint8_t temp;
        temp = *(DMemory4Bit + (E_INK_WIDTH / 2) * y0 + x);

        // Modify the specific pixel by writing all zeros into lower or upper 4 bits and set the needed color.
        *(DMemory4Bit + (E_INK_WIDTH / 2) * y0 + x) = (pixelMaskGLUT[x_sub] & temp) | (x_sub ? color : color << 4);
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
    // Copy everything from partial buffer to the main buffer.
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Variables for data handling.
    // Variable for GPIOs.
    uint32_t _send;

    // Variable for the EPD data LUT.
    uint8_t data;

    // Variable for current pixel data
    uint8_t dram;

    // Check if epaper power supply is successfully turned on.
    // If not, skip the update (if there is no power to the epaper, sending data to it can damage the epaper!).
    if (!einkOn())
        return;

    // Clear the display by flashing epaper display black, white, black white.
    clean(0, 1);
    clean(1, 14);
    clean(2, 1);
    clean(0, 14);
    clean(2, 1);
    clean(1, 14);
    clean(2, 1);
    clean(0, 14);
    clean(2, 1);

    // First send the only black pixels.
    for (int k = 0; k < 5; ++k)
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

            // Clock the last byte one more time.
            GPIO.out_w1ts = _send | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }

        // Wait 230 microseconds between the phases / frames (recorded from timing controller).
        delayMicroseconds(230);
    }

    // Send only one frame / phase of black and white pixels
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

        // Clock the last byte one more time.
        GPIO.out_w1ts = _send | CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }

    // Wait 230 microseconds between the phases / frames (recorded from timing controller).
    delayMicroseconds(230);

    // Send discharge to the every pixel of the epaper display (without that pixels would be blurry).
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

        // Clock the last byte one more time.
        GPIO.out_w1ts = _send | CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }

    // Wait 230 microseconds between the phases / frames (recorded from timing controller).
    delayMicroseconds(230);

    vscan_start();

    // If is needed to leave the epaper power supply on, do not turn it of.
    if (!leaveOn)
        einkOff();

    // Remove block on the partial updates (needed if the first thing after power up is partial update).
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
    // Check if epaper power supply is successfully turned on.
    // If not, skip the update (if there is no power to the epaper, sending data to it can damage the epaper!).
    if (!einkOn())
        return;

    // Clear the display by flashing epaper display black, white, black white.
    clean(0, 1);
    clean(1, 14);
    clean(2, 1);
    clean(0, 14);
    clean(2, 1);
    clean(1, 14);
    clean(2, 1);
    clean(0, 14);
    clean(2, 1);

    // Send everything to the display. There are 9 waveform phases to get the needed graycale.
    for (int k = 0; k < 9; ++k)
    {
        // Set the start of the pointer to the end of the framebuffer
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

            // Clock the last byte one more time.
            GPIO.out_w1ts = t | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }

        // Wait 230 microseconds between the phases / frames (recorded from timing controller).
        delayMicroseconds(230);
    }
    // Set the drivers inside epaper panel into dischare state.
    clean(3, 1);

    vscan_start();

    // If is needed to leave the epaper power supply on, do not turn it of.
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

    uint16_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    uint32_t _send;
    uint8_t data = 0;
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

    for (int k = 0; k < 6; ++k)
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

            // Clock the last byte one more time.
            GPIO.out_w1ts = _send | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }

        // Wait 230 microseconds between the phases / frames (recorded from timing controller).
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

            // Clock the last byte one more time.
            GPIO.out_w1ts = CL;
            GPIO.out_w1tc = CL;
            vscan_end();
        }

        // Wait 230 microseconds between the phases / frames (recorded from timing controller).
        delayMicroseconds(230);
    }
}

#endif