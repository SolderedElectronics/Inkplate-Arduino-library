/**
 **************************************************
 *
 * @file        Inkplate13SPECTRADriver.cpp
 * @brief       Low level driver for the Inkplate 13 Spectra 6 e-paper panel
 *
 *              The Inkplate 13 uses a 13.3" 1200x1600 Spectra 6 colour e-paper panel. The
 *              panel is driven by two cascaded controller chips (master and slave), each one
 *              covering one half of the screen, so every command is addressed to the master,
 *              the slave, or to both of them at once (see eSpectraChipID).
 *
 *              Communication is 4-wire SPI. The frame buffer holds one 4 bit colour index per
 *              pixel, two pixels per byte, and supports the six Spectra colours: black, white,
 *              yellow, red, blue and green.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE13SPECTRA
#include "Inkplate13SPECTRADriver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

// SPI used for the MicroSd card
SPIClass spi1(1);

// Instance of the SdFat object
SdFat sd;

// SPI settings for communicating with the display, 10MHz
SPISettings epdSpiSettings(10000000, MSBFIRST, SPI_MODE0);

/**
 *
 * @brief       writePixelInternal funtion sets pixel data for (x, y) pixel position
 *
 * @param       int16_t x0
 *              default position for x, will be changed depending on rotation
 * @param       int16_t y0
 *              default position for y, will be changed depending on rotation
 * @param       uint16_t color
 *              pixel color
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
    if (color > 5)
        return;
    color = colorPalette[color];
    switch (_inkplate->getRotation())
    {
    case 3:
        _swap_int16_t(x0, y0);
        x0 = _inkplate->height() - x0 - 1;
        break;
    case 0:
        x0 = _inkplate->width() - x0 - 1;
        y0 = _inkplate->height() - y0 - 1;
        break;
    case 1:
        _swap_int16_t(x0, y0);
        y0 = _inkplate->width() - y0 - 1;
        break;
    }

    int _x = x0 / 2;
    int _x_sub = x0 % 2;
    uint8_t temp;
    temp = *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x);
    *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x) = (pixelMaskGLUT[_x_sub] & temp) | (_x_sub ? color : color << 4);
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

    // Check if begin is called already. If it is, that means that panel needs to
    // be reseted in order to be woken up from deep sleep. If is this first time
    // begin has been called, init SPI, init pins, allocate memory for frame
    // buffer and clear frame buffer
    if (!_beginDone)
    {

        setPanelPinsToLow();

        Wire.begin();

        // Save the given inkplate pointer for internal use
        _inkplate = _inkplatePtr;

        expander1.begin(IO_INT_ADDR);

        image.begin(_inkplatePtr);

        _inkplate->setRotation(1);

        // Allocate memory for internal frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Color whole frame buffer in white color
        memset(DMemory4Bit, INKPLATE_WHITE | (INKPLATE_WHITE << 4), E_INK_WIDTH * E_INK_HEIGHT / 2);

        _beginDone = true;
    }

    return true;
}


/**
 * @brief       clearDisplay function clears memory buffer for display
 *
 * @note        This does not clear the actual display, only the memory buffer, you need to call
 * display() function after this to clear the display
 */
void EPDDriver::clearDisplay()
{
    memset(DMemory4Bit, WHITE << 4 | WHITE, E_INK_WIDTH * E_INK_HEIGHT / 2);
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

    // Power up the screen (if is not already powered on).
    setPanelState(true);

    // Framebuffer must be send to two seperate display driver.
    // The screen is splitted in half - left and right side.
    // Send data in rows - first on master (left side of the screen).
    digitalWrite(SPECTRA133_CS_M_PIN, LOW);
    digitalWrite(SPECTRA133_CS_S_PIN, HIGH);

    // Start SPI transaction and send the command to fill the EPD framebuffer with data.
    spiStart();
    SPI.beginTransaction(epdSpiSettings);
    SPI.write(SPECTRA133_REGISTER_DTM);

    // Send the data to the first driver (left side)
    for (int i = 0; i < E_INK_HEIGHT; i++)
    {
        SPI.writeBytes(DMemory4Bit + (i * E_INK_WIDTH / 2), (E_INK_WIDTH / 4));
    }
    SPI.endTransaction();
    spiEnd();

    // Send data in rows - now on slave (right side of the screen).
    digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
    digitalWrite(SPECTRA133_CS_S_PIN, LOW);
    waitForBusy();

    // Start SPI transaction and send the command to fill the EPD framebuffer with data.
    spiStart();
    SPI.beginTransaction(epdSpiSettings);
    SPI.write(SPECTRA133_REGISTER_DTM);

    // Send the data to the second driver (right side).
    for (int i = 0; i < E_INK_HEIGHT; i++)
    {
        SPI.writeBytes(DMemory4Bit + (i * E_INK_WIDTH / 2) + (E_INK_WIDTH / 4), (E_INK_WIDTH / 4));
    }
    SPI.endTransaction();
    spiEnd();

    // Disable both drivers.
    digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
    digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
    waitForBusy();

    // Force display command.
    sendCommand(SPECTRA133_REGISTER_DRF, SPECTRA133_REGISTER_DRF_V, sizeof(SPECTRA133_REGISTER_DRF_V), eChipIdBoth);
    waitForBusy();

    // Disable power to the display (if needed).
    if (!_leaveOn)
        setPanelState(false);
    displayEnd();
}

/**
 * @brief       displayPartial refreshes only the specified rectangular region of the screen
 *              using the PTLW (Partial Load Window) register of the GDEP133C02 controller.
 *              Only the pixels inside the window are transferred to the panel; the rest of the
 *              display is unaffected.
 *
 * @param       int16_t x        Left edge of the update window (user space)
 * @param       int16_t y        Top edge of the update window (user space)
 * @param       int16_t w        Width of the update window in pixels
 * @param       int16_t h        Height of the update window in pixels
 * @param       bool _leaveOn    If true, panel power is left on after the update
 */
void EPDDriver::displayPartial(int16_t x, int16_t y, int16_t w, int16_t h, bool _leaveOn)
{
    displayStart();

    // Clip to the screen bounds for the current rotation.
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > _inkplate->width())
        w = _inkplate->width() - x;
    if (y + h > _inkplate->height())
        h = _inkplate->height() - y;
    if (w <= 0 || h <= 0)
    {
        displayEnd();
        return;
    }

    // Map user rectangle to panel-native rectangle.
    // Panel native: col = 0..E_INK_WIDTH-1 (1199), row = 0..E_INK_HEIGHT-1 (1599).
    // Each case mirrors the per-pixel transform in writePixelInternal.
    int16_t colStart, colEnd, rowStart, rowEnd;
    switch (_inkplate->getRotation())
    {
    case 0:
        // User space: E_INK_WIDTH × E_INK_HEIGHT.
        // panel_col = (E_INK_WIDTH-1) - x,  panel_row = (E_INK_HEIGHT-1) - y.
        colStart = (int16_t)E_INK_WIDTH - x - w;
        colEnd = (int16_t)E_INK_WIDTH - 1 - x;
        rowStart = (int16_t)E_INK_HEIGHT - y - h;
        rowEnd = (int16_t)E_INK_HEIGHT - 1 - y;
        break;
    case 2:
        // User space: E_INK_WIDTH × E_INK_HEIGHT.
        // panel_col = x,  panel_row = y  (identity — no transform applied in writePixelInternal).
        colStart = x;
        colEnd = x + w - 1;
        rowStart = y;
        rowEnd = y + h - 1;
        break;
    case 3:
        // User space: E_INK_HEIGHT × E_INK_WIDTH.
        // panel_col = (E_INK_WIDTH-1) - y,  panel_row = x.
        colStart = (int16_t)E_INK_WIDTH - y - h;
        colEnd = (int16_t)E_INK_WIDTH - 1 - y;
        rowStart = x;
        rowEnd = x + w - 1;
        break;
    default:
    case 1:
        // User space: E_INK_HEIGHT × E_INK_WIDTH.
        // panel_col = y,  panel_row = (E_INK_HEIGHT-1) - x.
        colStart = y;
        colEnd = y + h - 1;
        rowStart = (int16_t)E_INK_HEIGHT - x - w;
        rowEnd = (int16_t)E_INK_HEIGHT - 1 - x;
        break;
    }

    // PTLW alignment requirements (GDEP133C02):
    //   H: colStart and (colEnd+1) must both be multiples of 4.
    //   V: rowStart must be even; (rowEnd+1) must be even.
    colStart = (colStart / 4) * 4;
    colEnd = (((colEnd + 4) / 4) * 4) - 1;
    if (colEnd >= (int16_t)E_INK_WIDTH)
        colEnd = (int16_t)E_INK_WIDTH - 1;
    if (rowStart % 2 != 0)
        rowStart--;
    if (rowStart < 0)
        rowStart = 0;
    if ((rowEnd + 1) % 2 != 0)
        rowEnd++;
    if (rowEnd >= (int16_t)E_INK_HEIGHT)
        rowEnd = (int16_t)E_INK_HEIGHT - 1;

    setPanelState(true);

    const int16_t HALF_WIDTH = (int16_t)(E_INK_WIDTH / 2); // 600 pixels per chip
    const int16_t HALF_BYTES = HALF_WIDTH / 2;             // 300 bytes per row per chip

    bool masterNeeded = (colStart < HALF_WIDTH);
    bool slaveNeeded = (colEnd >= HALF_WIDTH);

    // Both chips must receive a full PTLW+DTM cycle before DRF, otherwise the
    // uninvolved chip falls back to a full-panel refresh when DRF fires.
    // For the uninvolved chip, a minimal 4×4 null window is used: it reads the
    // existing framebuffer data (same as what is already on screen) so the
    // refresh produces no visible change on that side.
    static const uint8_t ptlwNull[9] = {
        0x00, 0x00, // HRST = 0
        0x00, 0x07, // HRED = 7
        0x00, 0x00, // VRST = 0
        0x00, 0x01, // VRED = 1
        0x01        // PT   = 1 (enable)
    };

    // Master chip
    {
        uint8_t ptlwData[9];
        int16_t bytesPerRow, memColOff, rStart, rEnd;

        if (masterNeeded)
        {
            int16_t lcs = colStart;
            int16_t lce = (colEnd < HALF_WIDTH) ? colEnd : (HALF_WIDTH - 1);
            uint16_t HRST = (uint16_t)lcs * 2;
            uint16_t HRED = (uint16_t)(lce + 1) * 2 - 1;
            uint16_t VRST = (uint16_t)rowStart / 2;
            uint16_t VRED = (uint16_t)(rowEnd + 1) / 2 - 1;
            ptlwData[0] = HRST >> 8;
            ptlwData[1] = HRST & 0xFF;
            ptlwData[2] = HRED >> 8;
            ptlwData[3] = HRED & 0xFF;
            ptlwData[4] = VRST >> 8;
            ptlwData[5] = VRST & 0xFF;
            ptlwData[6] = VRED >> 8;
            ptlwData[7] = VRED & 0xFF;
            ptlwData[8] = 0x01;
            bytesPerRow = (lce - lcs + 1) / 2;
            memColOff = lcs / 2;
            rStart = rowStart;
            rEnd = rowEnd;
        }
        else
        {
            memcpy(ptlwData, ptlwNull, 9);
            bytesPerRow = 2; // 4 px / 2 px-per-byte
            memColOff = 0;   // top-left corner of master's region
            rStart = 0;
            rEnd = 3;
        }

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_M_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_CMD66);
        SPI.writeBytes(SPECTRA133_REGISTER_CMD66_V, sizeof(SPECTRA133_REGISTER_CMD66_V));
        digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_M_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_PTLW);
        SPI.writeBytes(ptlwData, 9);
        digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_M_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_DTM);
        for (int16_t row = rStart; row <= rEnd; row++)
            SPI.writeBytes(DMemory4Bit + row * (E_INK_WIDTH / 2) + memColOff, bytesPerRow);
        digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();
    }

    // Slave chip
    waitForBusy();
    {
        uint8_t ptlwData[9];
        int16_t bytesPerRow, memColOff, rStart, rEnd;

        if (slaveNeeded)
        {
            int16_t lcs = (colStart >= HALF_WIDTH) ? (colStart - HALF_WIDTH) : 0;
            int16_t lce = colEnd - HALF_WIDTH;
            uint16_t HRST = (uint16_t)lcs * 2;
            uint16_t HRED = (uint16_t)(lce + 1) * 2 - 1;
            uint16_t VRST = (uint16_t)rowStart / 2;
            uint16_t VRED = (uint16_t)(rowEnd + 1) / 2 - 1;
            ptlwData[0] = HRST >> 8;
            ptlwData[1] = HRST & 0xFF;
            ptlwData[2] = HRED >> 8;
            ptlwData[3] = HRED & 0xFF;
            ptlwData[4] = VRST >> 8;
            ptlwData[5] = VRST & 0xFF;
            ptlwData[6] = VRED >> 8;
            ptlwData[7] = VRED & 0xFF;
            ptlwData[8] = 0x01;
            bytesPerRow = (lce - lcs + 1) / 2;
            memColOff = HALF_BYTES + lcs / 2;
            rStart = rowStart;
            rEnd = rowEnd;
        }
        else
        {
            memcpy(ptlwData, ptlwNull, 9);
            bytesPerRow = 2;        // 4 px / 2 px-per-byte
            memColOff = HALF_BYTES; // top-left corner of slave's region
            rStart = 0;
            rEnd = 3;
        }

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_S_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_CMD66);
        SPI.writeBytes(SPECTRA133_REGISTER_CMD66_V, sizeof(SPECTRA133_REGISTER_CMD66_V));
        digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_S_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_PTLW);
        SPI.writeBytes(ptlwData, 9);
        digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();

        spiStart();
        SPI.beginTransaction(epdSpiSettings);
        digitalWrite(SPECTRA133_CS_S_PIN, LOW);
        SPI.write(SPECTRA133_REGISTER_DTM);
        for (int16_t row = rStart; row <= rEnd; row++)
            SPI.writeBytes(DMemory4Bit + row * (E_INK_WIDTH / 2) + memColOff, bytesPerRow);
        digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
        SPI.endTransaction();
        spiEnd();
    }

    // Both chips have received PTLW+DTM; trigger a coordinated refresh.
    waitForBusy();
    sendCommand(SPECTRA133_REGISTER_DRF, SPECTRA133_REGISTER_DRF_V, sizeof(SPECTRA133_REGISTER_DRF_V), eChipIdBoth);
    waitForBusy();

    if (!_leaveOn)
        setPanelState(false);
    displayEnd();
}

/**
 * @brief       returns the current panel state, 0 for off, 1 for on
 *
 */
uint8_t EPDDriver::getPanelState()
{
    return _panelState;
}

/**
 * @brief       sets the current panel state
 *
 * @param       uint8_t state
 *              if set to 1, the panel will be turned on and initialized.
 *              is et to 0, the panel will be turned off.
 */
void EPDDriver::setPanelState(uint8_t state)
{
    // Check if the current display power state is different than the provided.
    // If there is a difference - update it/change it.
    if (state != _panelState)
    {
        // Check if the screen must be powered down or powered up.
        if (state)
        {
            setPanelPinsToLow();
            delay(50);
            // Configure GPIOs.
            setIO();

            // Enable power to the screen.
            digitalWrite(SPECTRA133_PWR_EN, HIGH);

            // Wait a little bit.
            delay(100ULL);

            // First, do a hardware reset!
            resetPanel();

            // Wait for the reset to finish.
            delay(100ULL);

            // Initialze the screen by sending the magic values to the registers provided by the manufacturer.
            screenInit();

            // Power up a screen.
            sendCommand(SPECTRA133_REGISTER_PON, nullptr, 0, eChipIdBoth);
            waitForBusy();
        }
        else
        {
            // Power off the screen.
            sendCommand(SPECTRA133_REGISTER_POF, SPECTRA133_REGISTER_POF_V, sizeof(SPECTRA133_REGISTER_POF_V),
                        eChipIdBoth);
            waitForBusy();

            // Disable GPIOs.
            pinMode(SPECTRA133_DC_PIN, INPUT);
            pinMode(SPECTRA133_CS_M_PIN, INPUT);
            pinMode(SPECTRA133_CS_S_PIN, INPUT);
            pinMode(SPECTRA133_RST_PIN, INPUT);
            pinMode(SPECTRA133_BUSYN_PIN, INPUT);
            pinMode(SPECTRA133_PWR_EN, INPUT);

            // Disable power to the screen.
            digitalWrite(SPECTRA133_PWR_EN, LOW);
        }

        // Update the status variable.
        _panelState = state;
    }
}

/**
 * @brief       initializes the communication pins as well as SPI communication with the Inkplate 13 panel
 *
 */
void EPDDriver::setIO()
{
    // Config the GPIOs.
    pinMode(SPECTRA133_DC_PIN, OUTPUT);
    pinMode(SPECTRA133_CS_M_PIN, OUTPUT);
    pinMode(SPECTRA133_CS_S_PIN, OUTPUT);
    pinMode(SPECTRA133_RST_PIN, OUTPUT);
    pinMode(SPECTRA133_BUSYN_PIN, INPUT_PULLUP);
    pinMode(SPECTRA133_PWR_EN, OUTPUT);
    pinMode(SPECTRA133_BS0, OUTPUT);
    pinMode(SPECTRA133_BS1, OUTPUT);

    // Set their default states.
    digitalWrite(SPECTRA133_DC_PIN, HIGH);
    digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
    digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
    digitalWrite(SPECTRA133_RST_PIN, LOW);
    digitalWrite(SPECTRA133_PWR_EN, LOW);
    digitalWrite(SPECTRA133_BS0, LOW);
    digitalWrite(SPECTRA133_BS1, HIGH);

    // Config SPI.
    if (!SPI.begin(SPECTRA133_SPI_SCK, SPECTRA133_SPI_MISO, SPECTRA133_SPI_MOSI))
    {
        Serial.println("Failed to init SPI");
    }
}

/**
 * @brief       resetPanel resets Inkplate 13 screen
 */
void EPDDriver::resetPanel()
{
    // Toggle the reset pin to initialte HW reset.
    digitalWrite(SPECTRA133_RST_PIN, LOW);
    delay(100ULL);
    digitalWrite(SPECTRA133_RST_PIN, HIGH);
    delay(100ULL);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 13
 *
 * @param       uint8_t _command
 *              predefined command for epaper control
 */
void EPDDriver::sendCommand(uint8_t _cmd, const uint8_t *_parameters, uint32_t _n, enum eSpectraChipID _chipId)
{
    // Config the SPI.
    spiStart();
    SPI.beginTransaction(epdSpiSettings);

    // Set the chip select pin to low as well.
    if (_chipId & eChipIdSlave)
        digitalWrite(SPECTRA133_CS_S_PIN, LOW);
    if (_chipId & eChipIdMaster)
        digitalWrite(SPECTRA133_CS_M_PIN, LOW);

    // Send the command.
    SPI.write(_cmd);

    if (_n != 0)
    {
        SPI.writeBytes(_parameters, _n);
    }

    SPI.endTransaction();
    spiEnd();

    // Release the chip select.
    if (_chipId & eChipIdSlave)
        digitalWrite(SPECTRA133_CS_S_PIN, HIGH);
    if (_chipId & eChipIdMaster)
        digitalWrite(SPECTRA133_CS_M_PIN, HIGH);
}


/**
 * @brief       screenInit sends init commands to the panel.
 */
void EPDDriver::screenInit()
{
    // Send magic values to the registers. These values are provided from the manufacturer.
    sendCommand(SPECTRA133_REGISTER_AN_TM, SPECTRA133_REGISTER_AN_TM_V, sizeof(SPECTRA133_REGISTER_AN_TM_V),
                eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_CMD66, SPECTRA133_REGISTER_CMD66_V, sizeof(SPECTRA133_REGISTER_CMD66_V),
                eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_PSR, SPECTRA133_REGISTER_PSR_V, sizeof(SPECTRA133_REGISTER_PSR_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_PLL, SPECTRA133_REGISTER_PLL_V, sizeof(SPECTRA133_REGISTER_PLL_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_CDI, SPECTRA133_REGISTER_CDI_V, sizeof(SPECTRA133_REGISTER_CDI_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_TCON, SPECTRA133_REGISTER_TCON_V, sizeof(SPECTRA133_REGISTER_TCON_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_AGID, SPECTRA133_REGISTER_AGID_V, sizeof(SPECTRA133_REGISTER_AGID_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_PWS, SPECTRA133_REGISTER_PWS_V, sizeof(SPECTRA133_REGISTER_PWS_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_CCSET, SPECTRA133_REGISTER_CCSET_V, sizeof(SPECTRA133_REGISTER_CCSET_V),
                eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_TRES, SPECTRA133_REGISTER_TRES_V, sizeof(SPECTRA133_REGISTER_TRES_V), eChipIdBoth);
    sendCommand(SPECTRA133_REGISTER_PWR, SPECTRA133_REGISTER_PWR_V, sizeof(SPECTRA133_REGISTER_PWR_V), eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_EN_BUF, SPECTRA133_REGISTER_EN_BUF_V, sizeof(SPECTRA133_REGISTER_EN_BUF_V),
                eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_BTST_P, SPECTRA133_REGISTER_BTST_P_V, sizeof(SPECTRA133_REGISTER_BTST_P_V),
                eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_BOOST_VDDP_EN, SPECTRA133_REGISTER_BOOST_VDDP_EN_V,
                sizeof(SPECTRA133_REGISTER_BOOST_VDDP_EN_V), eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_BTST_N, SPECTRA133_REGISTER_BTST_N_V, sizeof(SPECTRA133_REGISTER_BTST_N_V),
                eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_BUCK_BOOST_VDDN, SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V,
                sizeof(SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V), eChipIdMaster);
    sendCommand(SPECTRA133_REGISTER_TFT_VCOM_POWER, SPECTRA133_REGISTER_TFT_VCOM_POWER_V,
                sizeof(SPECTRA133_REGISTER_TFT_VCOM_POWER_V), eChipIdMaster);
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
    delay(200);
    spi1.begin(12, 13, 11, 10);
    setSdCardOk(sd.begin(SdSpiConfig(10, SHARED_SPI, SD_SCK_MHZ(25), &spi1)));
    return getSdCardOk();
}

/**
 * @brief       sdCardSleep turns off the P-MOS which powers the sd card to save energy in deep sleep
 */
void EPDDriver::sdCardSleep()
{
    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(10, INPUT);
    pinMode(11, INPUT);
    pinMode(12, INPUT);
    pinMode(13, INPUT);

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
    return &spi1;
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
    int adc = analogReadMilliVolts(1);

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
 * @brief       Method waits until the screen is ready to accept new commands.
 */
void EPDDriver::waitForBusy()
{
    // Wait until the screen is ready to accept new commads.
    // This will be indicated by pulling the BUSYN pin to high.
    while (!digitalRead(SPECTRA133_BUSYN_PIN))
    {
        // Let the RTOS breathe.
        delay(1);
    }
}

/**
 * @brief       Function helps empty capacitors, without this sometimes the panel refuses to refresh...
 */
void EPDDriver::setPanelPinsToLow()
{
    pinMode(SPECTRA133_DC_PIN, OUTPUT);
    pinMode(SPECTRA133_CS_M_PIN, OUTPUT);
    pinMode(SPECTRA133_RST_PIN, OUTPUT);
    pinMode(SPECTRA133_BUSYN_PIN, OUTPUT);
    pinMode(SPECTRA133_CS_S_PIN, OUTPUT);
    pinMode(SPECTRA133_PWR_EN, OUTPUT);
    pinMode(SPECTRA133_BS0, OUTPUT);
    pinMode(SPECTRA133_BS1, OUTPUT);

    digitalWrite(SPECTRA133_DC_PIN, LOW);
    digitalWrite(SPECTRA133_CS_M_PIN, LOW);
    digitalWrite(SPECTRA133_RST_PIN, LOW);
    digitalWrite(SPECTRA133_BUSYN_PIN, LOW);
    digitalWrite(SPECTRA133_CS_S_PIN, LOW);
    digitalWrite(SPECTRA133_PWR_EN, LOW);
    digitalWrite(SPECTRA133_BS0, LOW);
    digitalWrite(SPECTRA133_BS1, LOW);
}


#endif