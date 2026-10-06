/**
 **************************************************
 *
 * @file        Inkplate10Driver.cpp
 * @brief       Low level driver for the Inkplate 10 e-paper panel, ESP32-S3 breakout version
 *
 *              The panel is a 9.7" 1200x825 monochrome e-paper driven over a parallel
 *              interface. The data lines are driven directly through the ESP32-S3 GPIO
 *              output registers, the slow control lines through native GPIOs as well, and
 *              the PMIC control lines (WAKEUP, PWRUP, VCOM, PWR_GOOD) through a single
 *              PCAL6416 expander. The TPS65186 PMIC generates the e-paper driving rails
 *              and also provides the panel temperature reading.
 *
 *              This breakout build has no RTC, no microSD card, no touchpad, no battery
 *              divider and no second I/O expander.
 *
 *              The driver supports both display modes: 1 bit (black and white, with partial
 *              updates) and 3 bit (8 levels of grey, full updates only). The waveforms used
 *              for the greyscale refresh are in waveforms.h and are picked based on the
 *              waveform revision stored in the board EEPROM.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

// Header guard for the Arduino include
#if defined(ARDUINO_ESP32S3_DEV)
#include "Inkplate10Driver.h"
#include "../../system/inkplateSemaphore.h"
#include "Inkplate.h"

// Every native GPIO the driver drives, control lines first, then the data bus.
static const uint8_t epdPins[] = {CL_PIN, SPV_PIN, GMOD_PIN, OE_PIN, CKV_PIN, LE_PIN, SPH_PIN, EPD_D0,
                                  EPD_D1, EPD_D2,  EPD_D3,   EPD_D4, EPD_D5,  EPD_D6, EPD_D7};

/**
 *
 * @brief       writePixelInternal funtion sets pixel data for (x, y) pixel position
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
void EPDDriver::writePixelInternal(int16_t x, int16_t y, uint16_t color)
{
    int16_t x0 = x;
    int16_t y0 = y;
    if (x0 > _inkplate->width() - 1 || y0 > _inkplate->height() - 1 || x0 < 0 || y0 < 0)
        return;
    switch (_inkplate->getRotation())
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = _inkplate->height() - x0 - 1;
        break;
    case 2:
        x0 = _inkplate->width() - x0 - 1;
        y0 = _inkplate->height() - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = _inkplate->width() - y0 - 1;
        break;
    }

    if (_inkplate->getDisplayMode() == 0)
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
 * @brief       begin function initialize Inkplate object with predefined
 * settings
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
int EPDDriver::initDriver(Inkplate *_inkplatePtr)
{
    // If the driver was already initialized, skip current initialization
    if (_beginDone == 1)
        return 0;


    // Save the given inkplate pointer for internal use
    _inkplate = _inkplatePtr;

    // Initialize the image processing functionalities
    image.begin(_inkplatePtr);

    // Initialize the GIF playback functionalities
    gif.begin(_inkplatePtr);

    // Initialize the all GPIOs
    gpioInit();

    // Block using pins connected to the panel by the user
    blockGpioPins();


    if (!initializeFramebuffers())
    {
        return 0;
    }

    // Load the 3-bit waveform stored in EEPROM during factory programming before
    // calculating LUTs, so the LUTs are built from the correct waveform.
    // Falls back to the compiled-in default if EEPROM data is absent or corrupt.
    EEPROM.begin(512);
    struct waveformData waveformEEPROM;
    if (getWaveformFromEEPROM(&waveformEEPROM) && waveformEEPROM.waveformId >= INKPLATE10_WAVEFORM1 &&
        waveformEEPROM.waveformId <= INKPLATE10_WAVEFORM5)
    {
        memcpy(waveform3Bit, waveformEEPROM.waveform, sizeof(waveform3Bit));
    }

    // Calculate color LUTs to optimize drawing to the screen
    calculateLUTs();

    _beginDone = 1;
    return 1;
}


/**
 * @brief       Calculates the values of the lookup table to
 *              speed up rendering
 *
 * @note        Two tables per LUT, one for each GPIO output register, because the
 *              data bus is split between GPIO21 and GPIO38-48.
 */
void EPDDriver::calculateLUTs()
{
    for (int j = 0; j < 9; ++j)
    {
        for (uint32_t i = 0; i < 256; ++i)
        {
            uint8_t z = (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
            GLUT[j * 256 + i] = DATA_TO_LOW(z);
            GLUTH[j * 256 + i] = DATA_TO_HIGH(z);
            z = ((waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j])) << 4;
            GLUT2[j * 256 + i] = DATA_TO_LOW(z);
            GLUT2H[j * 256 + i] = DATA_TO_HIGH(z);
        }
    }
}


/**
 * @brief       vscan_start starts writing new frame and skips first two lines
 * that are invisible on screen
 */
void EPDDriver::vscan_start()
{
    CKV_SET;
    delayMicroseconds(7);
    SPV_CLEAR;
    delayMicroseconds(10);
    CKV_CLEAR;
    delayMicroseconds(0);
    CKV_SET;
    delayMicroseconds(8);
    SPV_SET;
    delayMicroseconds(10);
    CKV_CLEAR;
    delayMicroseconds(0);
    CKV_SET;
    delayMicroseconds(18);
    CKV_CLEAR;
    delayMicroseconds(0);
    CKV_SET;
    delayMicroseconds(18);
    CKV_CLEAR;
    delayMicroseconds(0);
    CKV_SET;
}

/**
 * @brief       vscan_end ends current row and prints data to screen
 */
void EPDDriver::vscan_end()
{
    CKV_CLEAR;
    EPD_LE_DELAY();
    LE_SET;
    EPD_LE_DELAY();
    LE_CLEAR;
    EPD_LE_DELAY();
    delayMicroseconds(0);
}

/**
 * @brief       sets the current display mode of the e-ink display
 *
 * @param       uint8_t
 *              if set to 1, it will be set to grayscale mode
 *              if set to 0, set to BW mode
 */
void EPDDriver::selectDisplayMode(uint8_t displayMode)
{
    _displayMode = displayMode;
}

/**
 * @brief       clearDisplay function clears memory buffer for display
 *
 * @note        This does not clear the actual display, only the memory buffer, you need to call
 * display() function after this to clear the display
 */
void EPDDriver::clearDisplay()
{
    // Clear 1 bit per pixel display buffer
    if (_displayMode == 0)
        memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Clear 3 bit per pixel display buffer
    else if (_displayMode == 1)
        memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);
}

/**
 * @brief       display function update display with new data from buffer
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 */
void EPDDriver::display(bool _leaveOn)
{
    displayStart();
    if (_displayMode == 0)
    {
        display1b(_leaveOn);
    }
    else if (_displayMode == 1)
    {
        display3b(_leaveOn);
    }
    displayEnd();
}

/**
 * @brief       display3b function writes grayscale data to display
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 */
void IRAM_ATTR EPDDriver::display3b(bool leaveOn)
{
    if (!einkOn())
        return;
    clean(1, 1);
    clean(0, 10);
    clean(2, 1);
    clean(1, 10);
    clean(2, 1);
    clean(0, 10);
    clean(2, 1);
    clean(1, 10);

    for (int k = 0; k < 9; k++)
    {
        uint8_t *dp = DMemory4Bit + (E_INK_HEIGHT * E_INK_WIDTH / 2);

        vscan_start();

        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            uint8_t b1 = *(--dp);
            uint8_t b0 = *(--dp);
            uint32_t tl = GLUT2[k * 256 + b1] | GLUT[k * 256 + b0];
            uint32_t th = GLUT2H[k * 256 + b1] | GLUTH[k * 256 + b0];
            hscan_start(tl, th);

            b1 = *(--dp);
            b0 = *(--dp);
            tl = GLUT2[k * 256 + b1] | GLUT[k * 256 + b0];
            th = GLUT2H[k * 256 + b1] | GLUTH[k * 256 + b0];
            EPD_SEND(tl, th);

            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
            {
                b1 = *(--dp);
                b0 = *(--dp);
                tl = GLUT2[k * 256 + b1] | GLUT[k * 256 + b0];
                th = GLUT2H[k * 256 + b1] | GLUTH[k * 256 + b0];
                EPD_SEND(tl, th);

                b1 = *(--dp);
                b0 = *(--dp);
                tl = GLUT2[k * 256 + b1] | GLUT[k * 256 + b0];
                th = GLUT2H[k * 256 + b1] | GLUTH[k * 256 + b0];
                EPD_SEND(tl, th);
            }

            EPD_SEND(0, 0);
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
 *
 * @brief       display1b function writes black and white data to display
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
 */
void EPDDriver::display1b(bool _leaveOn)
{
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);

    uint32_t _pos;
    uint8_t data;
    uint8_t dram;
    uint8_t _repeat;

    if (!einkOn())
        return;

    clean(0, 1);
    clean(1, 10);
    clean(2, 1);
    clean(0, 10);
    clean(2, 1);
    clean(1, 10);
    clean(2, 1);
    clean(0, 10);
    _repeat = 5;

    for (int k = 0; k < _repeat; k++)
    {
        _pos = (E_INK_HEIGHT * E_INK_WIDTH / 8) - 1;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; i++)
        {
            dram = (*(DMemoryNew + _pos));
            data = LUTB[(dram >> 4) & 0x0F];
            hscan_start(pinLUT[data], pinLUTH[data]);
            data = LUTB[dram & 0x0F];
            EPD_SEND(pinLUT[data], pinLUTH[data]);
            _pos--;
            for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++)
            {
                dram = (*(DMemoryNew + _pos));
                data = LUTB[(dram >> 4) & 0x0F];
                EPD_SEND(pinLUT[data], pinLUTH[data]);
                data = LUTB[dram & 0x0F];
                EPD_SEND(pinLUT[data], pinLUTH[data]);
                _pos--;
            }
            EPD_SEND(0, 0);
            vscan_end();
        }
        delayMicroseconds(230);
    }

    clean(2, 2);
    clean(3, 1);

    vscan_start();
    if (!_leaveOn)
        einkOff();
    _blockPartial = 0;
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
uint32_t EPDDriver::partialUpdate(bool _forced, bool leaveOn)
{
    if (getDisplayMode() == 1)
        return 0;
    if (_blockPartial == 1 && !_forced)
    {
        display1b(leaveOn);
        return 0;
    }

    if (_partialUpdateCounter >= _partialUpdateLimiter && _partialUpdateLimiter != 0)
    {
        // Force full update.
        display1b(leaveOn);

        // Reset the counter!
        _partialUpdateCounter = 0;

        // Go back!
        return 0;
    }

    uint32_t _pos = (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
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


    _repeat = 5;

    for (int k = 0; k < _repeat; ++k)
    {
        vscan_start();
        n = (E_INK_WIDTH * E_INK_HEIGHT / 4) - 1;
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            data = *(_pBuffer + n);
            hscan_start(pinLUT[data], pinLUTH[data]);
            n--;
            for (int j = 0; j < ((E_INK_WIDTH / 4) - 1); ++j)
            {
                data = *(_pBuffer + n);
                EPD_SEND(pinLUT[data], pinLUTH[data]);
                n--;
            }
            EPD_SEND(pinLUT[data], pinLUTH[data]);
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

    if (_partialUpdateLimiter != 0)
        _partialUpdateCounter++;

    return changeCount;
}

/**
 * @brief   Set the number of partial updates afterwhich full screen update is performed.
 *
 * @param   uint16_t _numberOfPartialUpdates
 *          Number of allowed partial updates afterwhich full update is performed.
 *          0 = disabled, no automatic full update will be performed.
 *
 * @note    By default, this is disabled, but to keep best image quality perform a full update
 *          every 60-80 partial updates.
 */
void EPDDriver::setFullUpdateThreshold(uint16_t _numberOfPartialUpdates)
{
    // Copy the value into the local variable.
    _partialUpdateLimiter = _numberOfPartialUpdates;

    // If the limiter is enabled, force full update.
    if (_numberOfPartialUpdates != 0)
        _blockPartial = 1;
}

/**
 * @brief       einkOn turns on supply for epaper display (TPS65186) [+15 VDC,
 * -15VDC, +22VDC, -20VDC, +3.3VDC, VCOM]
 *
 * @note        its important to use this order when turning epaper on.
 *              using wrong order can irreparably damage epaper
 *
 * @return      1 if succesfully turned on, 0 if failed
 */
int EPDDriver::einkOn()
{
    if (getPanelState() == 1)
        return 1;

    pinsAsOutputs();
    LE_CLEAR;
    CL_CLEAR;
    SPH_SET;
    GMOD_SET;
    SPV_SET;
    CKV_CLEAR;
    OE_CLEAR;
    setPanelState(1);

    if (!pmic.powerUp())
    {
        // log_e goes to the same place the core I2C errors do, so this shows up even
        // in sketches that never call Serial.begin().
        log_e("PMIC power-up failed, PWR_GOOD reg = 0x%02X (want 0x%02X), PWR_GOOD pin = %d", pmic.readPowerGood(),
              PWR_GOOD_OK, expander1.digitalRead(PWR_GOOD, true));
        einkOff();
        return 0;
    }

    OE_SET;
    return 1;
}

/**
 * @brief       einkOff turns off epaper power supply and put all digital IO
 * pins in high Z state
 */
void EPDDriver::einkOff()
{
    if (getPanelState() == 0)
        return;
    OE_CLEAR;
    GMOD_CLEAR;
    GPIO.out_w1tc = DATA_LOW | LE | CL;
    GPIO.out1_w1tc.val = DATA_HIGH;
    CKV_CLEAR;
    SPH_CLEAR;
    SPV_CLEAR;
    pmic.powerDown();
    pinsZstate();
    setPanelState(0);
}

/**
 * @brief       Initializes the PMIC with the board-specific IO expander and pin assignments.
 */
void EPDDriver::pmicBegin()
{
    pmic.begin(&expander1, WAKEUP, PWRUP, VCOM);
}


/**
 * @brief       pinsAsOutputs sets all panel control and data pins as outputs
 */
void EPDDriver::pinsAsOutputs()
{
    for (uint8_t i = 0; i < sizeof(epdPins); i++)
        pinMode(epdPins[i], OUTPUT);
}

/**
 * @brief       Returns the current power state of the EPD panel.
 *
 * @return      1 if the panel is powered on, 0 if powered off.
 */
uint8_t EPDDriver::getPanelState()
{
    return _panelState;
}
/**
 * @brief       Sets the internal panel power state flag.
 *
 * @param       uint8_t state
 *              1 to mark the panel as powered on, 0 as powered off.
 */
void EPDDriver::setPanelState(uint8_t state)
{
    _panelState = state;
}

/**
 * @brief       readPowerGood reads ok status for each rail
 *
 * @return      power good status register
 */
uint8_t EPDDriver::readPowerGood()
{
    return pmic.readPowerGood();
}

/**
 * @brief       isPowerGood checks if power good status is ok for all rails
 *
 * @return      true if power good status is ok for all rails, false otherwise
 *
 * @note        The hardware PWR_GOOD line of the TPS65186 is wired to pin 4 of the
 *              expander, so the rails are checked there first. The status register is
 *              still read afterwards because it tells which rail is failing.
 */
bool EPDDriver::isPowerGood()
{
    if (!expander1.digitalRead(PWR_GOOD, true))
        return false;

    return pmic.isPowerGood();
}

/**
 * @brief       pinsZstate sets all panel control and data pins to high Z state
 *
 * @note        this is used only when turning off epaper
 */
void EPDDriver::pinsZstate()
{
    for (uint8_t i = 0; i < sizeof(epdPins); i++)
        pinMode(epdPins[i], INPUT);
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
void EPDDriver::clean(uint8_t c, uint8_t rep)
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

    uint32_t _sendLow = pinLUT[data];
    uint32_t _sendHigh = pinLUTH[data];
    for (int k = 0; k < rep; ++k)
    {
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            hscan_start(_sendLow, _sendHigh);
            GPIO.out1_w1ts.val = _sendHigh;
            GPIO.out_w1ts = _sendLow | CL;
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
 * @brief       hscan_start starts writing data into current row
 *
 * @param       uint32_t _dLow
 *              data bits belonging to the low GPIO output register
 * @param       uint32_t _dHigh
 *              data bits belonging to the high GPIO output register
 */
void EPDDriver::hscan_start(uint32_t _dLow, uint32_t _dHigh)
{
    SPH_CLEAR;
    EPD_LE_DELAY();
    EPD_SEND(_dLow, _dHigh);
    SPH_SET;
    EPD_LE_DELAY();
    CKV_SET;
}

/**
 * @brief       Returns the current display mode.
 *
 * @return      0 for 1-bit (black and white), 1 for 3-bit (grayscale).
 */
uint8_t EPDDriver::getDisplayMode()
{
    return _displayMode;
}

/**
 * @brief       Initializes the IO expander, the PMIC and all of the data and
 *              control pins of the EPD driver.
 */
void EPDDriver::gpioInit()
{
    if (!expander1.begin(IO_INT_ADDR))
        log_e("I/O expander not found at 0x%02X", IO_INT_ADDR);

    expander1.pinMode(VCOM, OUTPUT, true);
    expander1.pinMode(PWRUP, OUTPUT, true);
    expander1.pinMode(WAKEUP, OUTPUT, true);
    expander1.digitalWrite(VCOM, LOW, true);
    expander1.digitalWrite(PWRUP, LOW, true);
    expander1.digitalWrite(WAKEUP, LOW, true);

    // PWR_GOOD and INT are open-drain outputs of the TPS65186, so they need the
    // expander pull-up or they read low forever.
    expander1.pinMode(PWR_GOOD, INPUT_PULLUP, true);
#if TPS_INT >= 0
    expander1.pinMode(TPS_INT, INPUT_PULLUP, true);
#endif

    pmicBegin();

    // Panel control and data lines.
    pinsAsOutputs();

    // Data byte to GPIO register masks, one table per output register.
    for (uint32_t i = 0; i < 256; ++i)
    {
        pinLUT[i] = DATA_TO_LOW(i);
        pinLUTH[i] = DATA_TO_HIGH(i);
    }
}

/**
 * @brief       initializeFramebuffers allocates memory to be used
 *              by specific display modes of the display
 *
 * @return      returns 0 if allocation failed, 1 if it succeeded
 */
uint8_t EPDDriver::initializeFramebuffers()
{
    // Initialize all the framebuffers
    DMemoryNew = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _partial = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8);
    _pBuffer = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);
    DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
    GLUT = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUT2 = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUTH = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    GLUT2H = (uint32_t *)malloc(256 * 9 * sizeof(uint32_t));
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL || GLUTH == NULL || GLUT2H == NULL)
    {
        return 0;
    }
    // Set all the framebuffers to White at start
    memset(DMemoryNew, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);
    memset(_pBuffer, 0, E_INK_WIDTH * E_INK_HEIGHT / 4);
    memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);

    return 1;
}

/**
 * @brief       getSdCardOk reports the microSD card status
 *
 * @return      always 0, this board has no microSD card slot
 */
int16_t EPDDriver::getSdCardOk()
{
    return 0;
}

/**
 * @brief       burnInClean function cleans the screen of any potential burn in by
 *              by writing a clear sequence to the panel
 *
 *
 * @param       uint8_t clear_cycles
 *              number of clear cycles
 *
 * @param       uint16_t cycles delay
 *              delay between clear cycles (in milliseconds)
 *
 *
 * @note        Cycles delay should not be smaller than 5 seconds
 */
void EPDDriver::burnInClean(uint8_t clear_cycles, uint16_t cycles_delay)
{
    einkOn();

    while (clear_cycles)
    {
        clean(1, 12);
        clean(2, 1);
        clean(0, 9);
        clean(2, 1);
        clean(1, 12);
        clean(2, 1);
        clean(0, 9);
        clean(2, 1);

        delay(cycles_delay);
        clear_cycles--;
    }
}

/**
 * @brief       Sets the VCOM voltage of the panel and saves it to EEPROM.
 *
 * @param       double vcom
 *              VCOM voltage to set; must be in the range [-5.0, 0.0].
 *
 * @return      true if the voltage was successfully set and saved, false otherwise.
 */
bool EPDDriver::setVCOM(double vcom)
{
    EEPROM.begin(512);
    // Check for out of bounds
    if (vcom < -5.0 || vcom > 0.0)
    {
        return false;
    }

    if (!writeVCOMToPanelEEPROM(vcom))
    {
        return false;
    }

    EEPROM.put(0, vcom);
    EEPROM.commit();
    return true;
}


/**
 * @brief       Programs the VCOM voltage into the TPS65186 internal EEPROM.
 *
 * @param       double v
 *              VCOM voltage value to program; must be in the range [-5.0, 0.0].
 *
 * @return      true if the readback value matches what was written, false otherwise.
 */
bool EPDDriver::writeVCOMToPanelEEPROM(double v)
{
    int raw = abs((int)(v * 100.0)) & 0x1FF;

    uint8_t vcomL = (uint8_t)(raw & 0xFF);
    uint8_t vcomMSB = (uint8_t)((raw >> 8) & 0x01); // goes into bit0 of reg 0x04

    // Power up TPS65186
    einkOn();
    delay(10);

    // Write low 8 bits
    writeReg(0x03, vcomL);

    // Read current reg 0x04 and preserve everything except bit0/bit6
    uint8_t r4 = readReg(0x04);
    r4 &= (uint8_t) ~((1 << 0) | (1 << 6)); // clear bit0 (MSB) and bit6 (program)
    r4 |= vcomMSB;                          // set bit0 as needed

    // Write updated reg 0x04 (bit6 still 0)
    writeReg(0x04, r4);
    delay(1);

    // Strobe "program to EEPROM" (bit6 = 1)
    writeReg(0x04, (uint8_t)(r4 | (1 << 6)));

#if TPS_INT >= 0
    // Wait until EEPROM has been programmed (INT goes LOW), with a timeout so a
    // missing INT connection cannot hang the call forever.
    unsigned long _timer = millis();
    while (expander1.digitalRead(TPS_INT, true) && (millis() - _timer) < 1000)
    {
        delay(1);
    }
#else
    // INT is not wired to the expander on this board, programming takes well under 100 ms.
    delay(100);
#endif

    // Clear interrupt flag by reading INT1 register
    (void)readReg(0x07);


    // Read back registers for verification
    uint8_t rdL = readReg(0x03);
    uint8_t reg04full = readReg(0x04);
    uint8_t rdH_bit0 = reg04full & 0x01;

    int check = ((int)rdH_bit0 << 8) | rdL;

    einkOff();
    delay(100);

    return (check == raw);
}

/**
 * @brief       Reads the VCOM voltage stored in the ESP32 EEPROM.
 *
 * @return      VCOM voltage as a double (negative value, e.g. -1.5).
 */
double EPDDriver::getVCOMValue()
{
    EEPROM.begin(512);
    double vcom;
    EEPROM.get(0, vcom);
    return vcom;
}
/**
 * @brief Write to a register of the TPS e-Paper power supply chip
 *
 * @param _reg The selected register
 * @param _data The data to write
 */
void EPDDriver::writeReg(uint8_t _reg, float _data)
{
    pmic.writeReg(_reg, (uint8_t)_data);
}

/**
 * @brief Read a register of the TPS e-Paper power supply chip
 *
 * @param _reg The selected register to read
 * @return uint8_t The data stored in the register
 */
uint8_t EPDDriver::readReg(uint8_t _reg)
{
    return pmic.readReg(_reg);
}

/**
 * @brief       getVcomVoltage reads VCOM voltage from registers
 *
 * @return      VCOM voltage in volts
 */
double EPDDriver::getStoredVCOM()
{
    delay(10);                            // Wake up TPS65186 so registers respond
    uint8_t vcomL = readReg(0x03);        // REad low 8 bits from register 0x03
    uint8_t vcomH = readReg(0x04) & 0x01; // Read full byte, mask off all but bit 0 (MSB)
    delay(10);                            // Power down driver
    int raw = (vcomH << 8) | vcomL;       // Value between 0 - 511
    return -(raw / 100.0);
}

/**
 * @brief       readTemperature reads panel temperature
 *
 * @return      returns  temperature in range from -10 to 85 degree C with
 * accuracy of +-1 in range from 0 to 50
 */
int8_t EPDDriver::readTemperature()
{
    return pmic.readTemperature();
}

/**
 * @brief       Blocks pins on the IO Expander which are used to control the panel, done to avoid damage to the display
 * by the user
 *
 * @return      None
 */
void EPDDriver::blockGpioPins()
{
    expander1.blockPinUsage(WAKEUP);
    expander1.blockPinUsage(PWRUP);
    expander1.blockPinUsage(VCOM);
    expander1.blockPinUsage(PWR_GOOD);
}

/**
 * @brief       Function calculates checksum of wavefrom data read from EEPROM
 *
 * @param       struct waveformData _w
 *              Structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 *
 * @return      Value of checksum from data read from EEPROM
 */
uint8_t EPDDriver::calculateChecksum(struct waveformData _w)
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
 * @brief       Function writes waveform data to EEPROM
 *
 * @param       struct waveformData *_w
 *              Structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 */
void EPDDriver::burnWaveformToEEPROM(struct waveformData _w)
{
    uint8_t *_ptr = (uint8_t *)&_w;
    for (int i = 0; i < sizeof(struct waveformData); i++)
    {
        EEPROM.write(i, _ptr[i]);
    }
    EEPROM.commit();
}

/**
 * @brief       Function allows grayscale waveform to be changed
 *
 * @param       uint8_t *_wf
 *              Waveform array with 8 rows where every row represents one color and 9 columns where every column
 * represents one phase or frame of each color.
 */
void EPDDriver::changeWaveform(uint8_t *_wf)
{
    memcpy(waveform3Bit, _wf, sizeof(waveform3Bit));
    calculateLUTs();
}

/**
 * @brief       Function reads waveform data from EEPROM and checks it's validity.
 *
 * @param       struct waveformData *_w
 *              Pointer to structure for waveform data read from EEPROM. Struct can be found in Inkplate.h file
 *
 * @return      True if data is vaild, false if not
 */
bool EPDDriver::getWaveformFromEEPROM(struct waveformData *_w)
{
    uint8_t *_ptr = (uint8_t *)_w;
    for (int i = 0; i < sizeof(struct waveformData); i++)
    {
        _ptr[i] = EEPROM.read(i);
    }

    return (calculateChecksum(*_w) != _w->checksum) ? false : true;
}


const uint8_t EPDDriver::waveform1[8][9] = {
    {0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0}, {0, 1, 2, 2, 1, 2, 2, 1, 0},
    {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};

const uint8_t EPDDriver::waveform2[8][9] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 2, 1, 2, 1, 1, 0}, {0, 0, 0, 2, 2, 1, 2, 1, 0}, {0, 0, 2, 2, 1, 2, 2, 1, 0},
    {0, 0, 0, 2, 1, 1, 1, 2, 0}, {0, 0, 2, 2, 2, 1, 1, 2, 0}, {0, 0, 0, 0, 0, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};

const uint8_t EPDDriver::waveform3[8][9] = {
    {0, 3, 3, 3, 3, 3, 3, 3, 0}, {0, 1, 2, 1, 1, 2, 2, 1, 0}, {0, 2, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 2, 2, 2, 2, 1, 0},
    {0, 3, 3, 2, 1, 1, 1, 2, 0}, {0, 3, 3, 2, 2, 1, 1, 2, 0}, {0, 2, 1, 2, 1, 2, 1, 2, 0}, {0, 3, 3, 3, 2, 2, 2, 2, 0}};

const uint8_t EPDDriver::waveform4[8][9] = {
    {0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0}, {1, 1, 2, 2, 1, 2, 2, 1, 0},
    {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 1, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};

const uint8_t EPDDriver::waveform5[8][9] = {
    {0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {2, 2, 2, 1, 0, 2, 1, 0, 0}, {2, 1, 1, 2, 1, 1, 1, 2, 0},
    {2, 2, 2, 1, 1, 1, 0, 2, 0}, {2, 2, 2, 1, 1, 2, 1, 2, 0}, {0, 0, 0, 0, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};

const uint8_t *const EPDDriver::waveformList[5] = {&waveform1[0][0], &waveform2[0][0], &waveform3[0][0],
                                                   &waveform4[0][0], &waveform5[0][0]};

/**
 * @brief       Selects and applies an EPD waveform preset by number.
 *
 * @param       uint8_t waveformNumber
 *              Waveform index (1–5). Values outside this range return false.
 * @param       bool burnToEEPROM
 *              If true, permanently stores the waveform selection in EEPROM so
 *              it survives power cycles.
 *
 * @return      true if the waveform was applied successfully, false if out of range.
 */
bool EPDDriver::setWaveform(uint8_t waveformNumber, bool burnToEEPROM)
{
    if (waveformNumber < 1 || waveformNumber > 5)
        return false;

    uint8_t index = waveformNumber - 1;

    // Apply waveform immediately
    changeWaveform((uint8_t *)waveformList[index]);

    if (!burnToEEPROM)
        return true;

    EEPROM.begin(512);

    waveformData waveformEEPROM;

    waveformEEPROM.waveformId = INKPLATE10_WAVEFORM1 + index;

    memcpy(&waveformEEPROM.waveform, waveformList[index], sizeof(waveformEEPROM.waveform));

    waveformEEPROM.checksum = calculateChecksum(waveformEEPROM);

    burnWaveformToEEPROM(waveformEEPROM);

    return true;
}

#endif
