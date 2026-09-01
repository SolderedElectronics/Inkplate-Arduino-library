// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE5V2
#include "Inkplate5V2Driver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

SPIClass spi2(2);
SdFat sd;

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

    // set x, y depending on selected rotation
    switch (_inkplate->getRotation())
    {
    case 1: // 90 degree left
        _swap_int16_t(x0, y0);
        x0 = _inkplate->height() - x0 - 1;
        break;
    case 2: // 180 degree, or upside down
        x0 = _inkplate->width() - x0 - 1;
        y0 = _inkplate->height() - y0 - 1;
        break;
    case 3: // 90 degree right
        _swap_int16_t(x0, y0);
        y0 = _inkplate->width() - y0 - 1;
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

    // Calculate color LUTs to optimize drawing to the screen
    calculateLUTs();

    // Use only myI2S
    myI2S = &I2S1;

    // Allocate memory for DMA descriptor and line buffer.
    _dmaLineBuffer = (uint8_t *)heap_caps_malloc((E_INK_WIDTH / 4) + 16, MALLOC_CAP_DMA);
    _dmaI2SDesc = (lldesc_s *)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);


    if (_dmaLineBuffer == NULL || _dmaI2SDesc == NULL)
    {
        return 0;
    }

    // Init the I2S driver. It will setup a I2S driver.
    I2SInit(myI2S);

    _beginDone = 1;
    return 1;
}


/**
 * @brief       Calculates the values of the lookup table to
 *              speed up rendering
 */
void EPDDriver::calculateLUTs()
{
    // Fill up the pixel to EPD LUT for 3 bit mode.
    for (int j = 0; j < 9; ++j)
    {
        for (int i = 0; i < 256; ++i)
        {
            GLUT[j * 256 + i] = (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
            GLUT2[j * 256 + i] = ((waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j])) << 4;
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
    LE_SET;
    LE_CLEAR;
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
    // Check if epaper power supply is successfully turned on.
    // If not, skip the update (if there is no power to the epaper, sending data to it can damage the epaper!).
    if (!einkOn())
        return;

    // Clear the display by flashing epaper display black, white, black white.
    clean(0, 1);
    clean(1, 11);
    clean(2, 1);
    clean(0, 11);
    clean(2, 1);
    clean(1, 11);
    clean(2, 1);
    clean(0, 11);

    // Send everything to the display. There are 9 waveform phases to get the needed graycale.
    for (int k = 0; k < 9; ++k)
    {
        uint8_t *dp = DMemory4Bit;

        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            uint8_t *_DMemoryNewPtrFlipped = (dp + E_INK_WIDTH / 2) - 1;
            for (int j = 0; j < (E_INK_WIDTH / 4); j += 4)
            {
                _dmaLineBuffer[j + 2] =
                    (GLUT2[k * 256 + (*(_DMemoryNewPtrFlipped--))] | GLUT[k * 256 + (*(_DMemoryNewPtrFlipped--))]);
                _dmaLineBuffer[j + 3] =
                    (GLUT2[k * 256 + (*(_DMemoryNewPtrFlipped--))] | GLUT[k * 256 + (*(_DMemoryNewPtrFlipped--))]);
                _dmaLineBuffer[j] =
                    (GLUT2[k * 256 + (*(_DMemoryNewPtrFlipped--))] | GLUT[k * 256 + (*(_DMemoryNewPtrFlipped--))]);
                _dmaLineBuffer[j + 1] =
                    (GLUT2[k * 256 + (*(_DMemoryNewPtrFlipped--))] | GLUT[k * 256 + (*(_DMemoryNewPtrFlipped--))]);
                dp += 8;
            }
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
        delayMicroseconds(230);
    }

    // Set the drivers inside epaper panel into dischare state.
    clean(3, 1);

    // vscan_start();

    // If is needed to leave the epaper power supply on, do not turn it of.
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

    uint32_t _send;
    uint8_t data;
    uint8_t dram;

    if (!einkOn())
        return;

    clean(0, 1);
    clean(1, 11);
    clean(2, 1);
    clean(0, 11);
    clean(2, 1);
    clean(1, 11);
    clean(2, 1);
    clean(0, 11);

    for (int k = 0; k < 3; ++k)
    {
        uint8_t *DMemoryNewPtr = DMemoryNew;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            uint8_t *_DMemoryNewPtrFlipped = (DMemoryNewPtr + E_INK_WIDTH / 8) - 1;
            for (int n = 0; n < (E_INK_WIDTH / 4); n += 4)
            {
                uint8_t dram1 = *(_DMemoryNewPtrFlipped--);
                uint8_t dram2 = *(_DMemoryNewPtrFlipped--);
                _dmaLineBuffer[n] = LUTB[(dram2 >> 4) & 0x0F];     // i + 2;
                _dmaLineBuffer[n + 1] = LUTB[dram2 & 0x0F];        // i + 3;
                _dmaLineBuffer[n + 2] = LUTB[(dram1 >> 4) & 0x0F]; // i;
                _dmaLineBuffer[n + 3] = LUTB[dram1 & 0x0F];        // i + 1;
                DMemoryNewPtr += 2;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
        delayMicroseconds(230);
    }

    for (int k = 0; k < 1; ++k)
    {
        uint8_t *DMemoryNewPtr = DMemoryNew;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            uint8_t *_DMemoryNewPtrFlipped = (DMemoryNewPtr + E_INK_WIDTH / 8) - 1;
            for (int n = 0; n < (E_INK_WIDTH / 4); n += 4)
            {
                uint8_t dram1 = *(_DMemoryNewPtrFlipped--);
                uint8_t dram2 = *(_DMemoryNewPtrFlipped--);
                _dmaLineBuffer[n] = LUT2[(dram2 >> 4) & 0x0F];     // i + 2;
                _dmaLineBuffer[n + 1] = LUT2[dram2 & 0x0F];        // i + 3;
                _dmaLineBuffer[n + 2] = LUT2[(dram1 >> 4) & 0x0F]; // i;
                _dmaLineBuffer[n + 3] = LUT2[dram1 & 0x0F];        // i + 1;
                DMemoryNewPtr += 2;
            }
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
        delayMicroseconds(230);
    }

    for (int k = 0; k < 1; ++k)
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
        delayMicroseconds(230);
    }

    // vscan_start();
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

    for (int k = 0; k < 4; ++k)
    {
        uint8_t *dp = _pBuffer;
        vscan_start();
        for (int i = 0; i < E_INK_HEIGHT; ++i)
        {
            uint8_t *_dpPtrFlipped = (dp + E_INK_WIDTH / 4) - 1;
            for (int j = 0; j < (E_INK_WIDTH / 4); j += 4)
            {
                _dmaLineBuffer[j + 2] = *(_dpPtrFlipped--);
                _dmaLineBuffer[j + 3] = *(_dpPtrFlipped--);
                _dmaLineBuffer[j] = *(_dpPtrFlipped--);
                _dmaLineBuffer[j + 1] = *(_dpPtrFlipped--);
            }
            dp += (E_INK_WIDTH / 4);
            // Send the data using I2S DMA driver.
            sendDataI2S(myI2S, _dmaI2SDesc);
            vscan_end();
        }
        delayMicroseconds(230);
    }
    clean(2, 2);
    // vscan_start();

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

    if (!pmic.powerUp(500))
    {
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
    GPIO.out &= ~(DATA | LE | CL);
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
 * @brief       pinsAsOutputs sets all tps pins as outputs
 */
void EPDDriver::pinsAsOutputs()
{
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    expander1.pinMode(OE, OUTPUT, true);
    expander1.pinMode(GMOD, OUTPUT, true);
    expander1.pinMode(SPV, OUTPUT, true);
    // Set up the EPD Data and CL pins for I2S.
    setI2S1pin(0, I2S1O_BCK_OUT_IDX, 0);
    setI2S1pin(4, I2S1O_DATA_OUT0_IDX, 0);
    setI2S1pin(5, I2S1O_DATA_OUT1_IDX, 0);
    setI2S1pin(18, I2S1O_DATA_OUT2_IDX, 0);
    setI2S1pin(19, I2S1O_DATA_OUT3_IDX, 0);
    setI2S1pin(23, I2S1O_DATA_OUT4_IDX, 0);
    setI2S1pin(25, I2S1O_DATA_OUT5_IDX, 0);
    setI2S1pin(26, I2S1O_DATA_OUT6_IDX, 0);
    setI2S1pin(27, I2S1O_DATA_OUT7_IDX, 0);

    // Start sending clock to the EPD.
    myI2S->conf1.tx_stop_en = 0;
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
 */
bool EPDDriver::isPowerGood()
{
    return pmic.isPowerGood();
}

/**
 * @brief       pinsZstate sets all tps pins at high z state
 *
 * @note        this is used only when turning off epaper
 */
void EPDDriver::pinsZstate()
{
    pinMode(2, INPUT);
    pinMode(32, INPUT);
    pinMode(33, INPUT);
    expander1.pinMode(OE, INPUT, true);
    expander1.pinMode(GMOD, INPUT, true);
    expander1.pinMode(SPV, INPUT, true);

    // Set up the EPD Data and CL pins for I2S .
    pinMode(0, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(18, INPUT);
    pinMode(19, INPUT);
    pinMode(23, INPUT);
    pinMode(25, INPUT);
    pinMode(26, INPUT);
    pinMode(27, INPUT);

    myI2S->conf1.tx_stop_en = 0;
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
        delayMicroseconds(230);
    }
}

/**
 * @brief       hscan_start starts writing data into current row
 *
 * @param       uint32_t _d
 *              data to be written into current row
 */
void EPDDriver::hscan_start(uint32_t _d)
{
    SPH_CLEAR;
    GPIO.out_w1ts = (_d) | CL;
    GPIO.out_w1tc = DATA | CL;
    SPH_SET;
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
 * @brief       Initializes the internal and external IO expanders,
 *              Configures all of the data and control pins for the
 *              EPD Driver.
 */
void EPDDriver::gpioInit()
{
    expander1.begin(IO_INT_ADDR);

    expander1.digitalWrite(9, LOW);

    expander1.pinMode(VCOM, OUTPUT, true);
    expander1.pinMode(PWRUP, OUTPUT, true);
    expander1.pinMode(WAKEUP, OUTPUT, true);
    expander1.pinMode(GPIO0_ENABLE, OUTPUT);
    expander1.digitalWrite(GPIO0_ENABLE, 1);

    pmicBegin();

    // For same reason, unused pins of first I/O expander have to be also set as
    // outputs, low.
    expander1.pinMode(11, OUTPUT);
    expander1.pinMode(12, OUTPUT);
    expander1.pinMode(13, OUTPUT);
    expander1.pinMode(14, OUTPUT);
    expander1.pinMode(15, OUTPUT);
    expander1.digitalWrite(11, LOW);
    expander1.digitalWrite(12, LOW);
    expander1.digitalWrite(13, LOW);
    expander1.digitalWrite(14, LOW);
    expander1.digitalWrite(15, LOW);

    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(12, INPUT_PULLDOWN);
    pinMode(13, INPUT_PULLDOWN);
    pinMode(14, INPUT_PULLDOWN);
    pinMode(15, INPUT_PULLDOWN);

    // And also disable uSD card supply
    expander1.pinMode(SD_PMOS_PIN, INPUT);

    // CONTROL PINS
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    expander1.pinMode(OE, OUTPUT, true);
    expander1.pinMode(GMOD, OUTPUT, true);
    expander1.pinMode(SPV, OUTPUT, true);

    // Battery voltage Switch MOSFET
    expander1.pinMode(9, OUTPUT);
    expander1.digitalWrite(9, LOW);

    for (uint32_t i = 0; i < 256; ++i)
        pinLUT[i] = ((i & B00000011) << 4) | (((i & B00001100) >> 2) << 18) | (((i & B00010000) >> 4) << 23) |
                    (((i & B11100000) >> 5) << 25);
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
    if (DMemoryNew == NULL || _partial == NULL || _pBuffer == NULL || DMemory4Bit == NULL || GLUT == NULL ||
        GLUT2 == NULL)
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
 * @brief       sdCardInit initializes sd card trough SPI
 *
 * @return      0 if failed to initialise, 1 if successful
 */
int16_t EPDDriver::sdCardInit()
{
    expander1.pinMode(SD_PMOS_PIN, OUTPUT);
    expander1.digitalWrite(SD_PMOS_PIN, LOW);
    delay(50);
    spi2.begin(14, 12, 13, 15);
    setSdCardOk(sd.begin(SdSpiConfig(15, SHARED_SPI, SD_SCK_MHZ(25), &spi2)));
    return getSdCardOk();
}

/**
 * @brief       sdCardSleep turns off the P-MOS which powers the sd card to save energy in deep sleep
 */
void EPDDriver::sdCardSleep()
{
    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);

    // And also disable uSD card supply
    expander1.pinMode(SD_PMOS_PIN, INPUT);
}

/**
 * @brief       getSdFat gets sd card object
 *
 * @return      sd card class object
 */
SdFat &EPDDriver::getSdFat()
{
    return sd;
}

/**
 * @brief       getSPIptr gets SPI class object pointer
 *
 * @return      SPI class object
 */
SPIClass *EPDDriver::getSPIptr()
{
    return &spi2;
}

/**
 * @brief       setSdCardOk sets sd card OK status
 *
 * @param       int16_t s
 *              sd card OK status, can be 1 or 0
 */
void EPDDriver::setSdCardOk(int16_t s)
{
    _sdCardOk = s;
}


/**
 * @brief       setSdCardOk gets sd card OK status
 *
 * @return      sd card OK status, can be 1 or 0
 */
int16_t EPDDriver::getSdCardOk()
{
    return _sdCardOk;
}


/**
 * @brief       readBattery reads voltage of the battery
 *
 * @return      returns battery voltage value
 */
double EPDDriver::readBattery()
{
    // Read the pin on the battery MOSFET. If is high, that means is older version of the board
    // that uses PMOS only. If it's low, newer board with both PMOS and NMOS.
    expander1.pinMode(9, INPUT);
    int state = expander1.digitalRead(9);
    expander1.pinMode(9, OUTPUT);

    // If the input is pulled high, it's PMOS only.
    // If it's pulled low, it's PMOS and NMOS.
    if (state)
    {
        expander1.digitalWrite(9, LOW);
    }
    else
    {
        expander1.digitalWrite(9, HIGH);
    }

    // Wait a little bit after a MOSFET enable.
    delay(5);

    // Set to the highest resolution and read the voltage.
    analogReadResolution(12);
    int adc = analogReadMilliVolts(35);

    // Turn off the MOSFET (and voltage divider).
    if (state)
    {
        expander1.digitalWrite(9, HIGH);
    }
    else
    {
        expander1.digitalWrite(9, LOW);
    }

    // Calculate the voltage at the battery terminal (voltage is divided in half by voltage divider).
    return (double(adc) * 2.0 / 1000);
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
        clean(1, 21);
        clean(2, 1);
        clean(0, 12);
        clean(2, 1);
        clean(1, 21);
        clean(2, 1);
        clean(0, 12);
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
    expander1.pinMode(6, INPUT_PULLUP);
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

    // Wait until EEPROM has been programmed (INT goes LOW)
    // Make sure INT pin is configured correctly elsewhere (usually input pullup).
    while (expander1.digitalRead(6))
    {
        delay(1);
    }

    // Clear interrupt flag by reading INT1 register
    (void)readReg(0x07);


    // Read back registers for verification
    uint8_t rdL = readReg(0x03);
    // uint8_t rdH_bit0 = readReg(0x04) & 0x01;
    uint8_t reg04full = readReg(0x04);
    uint8_t rdH_bit0 = reg04full & 0x01;

    int check = ((int)rdH_bit0 << 8) | rdL;

    // DEBUG PRINTS
    Serial.printf("\nraw=%d (0x%03X), vcomL=0x%02X, vcomMSB=%d\n", raw, raw, vcomL, vcomMSB);
    Serial.printf("readback: rdL=0x%02X, rdHbit0=%d => check=%d (0x%03X)\n", rdL, rdH_bit0, check, check);
    Serial.printf("reg04 full=0x%02X\n", reg04full);
    // Turn off TPS/EPD power (your function)
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
    expander1.blockPinUsage(OE);
    expander1.blockPinUsage(GMOD);
    expander1.blockPinUsage(SPV);
}

#endif