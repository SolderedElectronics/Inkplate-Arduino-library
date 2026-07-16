// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE31SPECTRA
#include "Inkplate31SPECTRADriver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

// SPI used for the MicroSD card (the panel itself does not use SPIClass - see qspiInit()).
SPIClass spi1(1);

// Instance of the SdFat object.
SdFat sd;

/**
 *
 * @brief       writePixelInternal sets pixel data for the (x, y) pixel position.
 *
 * @param       int16_t x, int16_t y
 *              Pixel position, pre-rotation.
 * @param       uint16_t color
 *              Palette index (0-5).
 *
 * @note        Landscape-native panel (unlike Inkplate13SPECTRA/Inkplate2) - rotation 2 is
 *              the identity transform, matching Inkplate6COLOR's convention.
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

    // Per-column lookup tables (built once in initializeFramebuffers()) fold in which of
    // the 8 chip strips this column belongs to and the byte/nibble offset within it.
    uint32_t byteIndex = _colChipByteBase[x0] + (uint32_t)y0 * (SPECTRA315_CHIP_RAM_WIDTH / 2) + _colByteInRow[x0];
    uint8_t *bytePtr = DMemory4Bit + byteIndex;
    uint8_t temp = *bytePtr;
    *bytePtr = _colNibble[x0] ? ((temp & 0x0F) | (color << 4)) : ((temp & 0xF0) | color);
}

/**
 * @brief       begin function initializes Inkplate object with predefined settings.
 *
 * @param       Inkplate *_inkplatePtr
 *
 * @return      True if initialization is successful, false if failed or already initialized.
 */
int EPDDriver::initDriver(Inkplate *_inkplatePtr)
{
    if (!_beginDone)
    {
        _inkplate = _inkplatePtr;

        pinMode(SPECTRA315_RST_PIN, OUTPUT);
        digitalWrite(SPECTRA315_RST_PIN, LOW);
        pinMode(SPECTRA315_BUSY_PIN, INPUT);

        // Explicit SDA/SCL pins - this is a new board with no established variant
        // default I2C pins yet (see CLAUDE.md).
        Wire.begin(SPECTRA315_I2C_SDA, SPECTRA315_I2C_SCL);

        expander1.begin(IO_INT_ADDR);
        expander2.begin(IO_INT_ADDR_EXP2); // Reserved/unused for now - see CLAUDE.md.

        // Default-deselect all 8 CS lines.
        for (uint8_t i = 0; i < SPECTRA315_CHIP_COUNT; i++)
        {
            expander1.pinMode(csExpanderBit[i], OUTPUT);
            expander1.digitalWrite(csExpanderBit[i], HIGH);
        }

        pmic.begin(&expander1, SPECTRA315_EXP_PMIC_PWR_EN, SPECTRA315_EXP_IST9201_EN, SPECTRA315_EXP_VDDP_EN,
                   SPECTRA315_EXP_VDDN_EN, SPECTRA315_EXP_VCNP_EN, SPECTRA315_TOGGLE_SEL0, SPECTRA315_TOGGLE_SEL1,
                   SPECTRA315_IST9201_PG, SPECTRA315_IST9201_PS);

        qspiInit();

        image.begin(_inkplatePtr);

        if (!initializeFramebuffers())
            return false;

        _beginDone = true;
    }

    return true;
}

/**
 * @brief       clearDisplay clears the memory buffer for the display.
 *
 * @note        This does not clear the actual display, only the memory buffer, you need
 *              to call display() function after this to clear the display.
 */
void EPDDriver::clearDisplay()
{
    memset(DMemory4Bit, INKPLATE_WHITE | (INKPLATE_WHITE << 4), (size_t)SPECTRA315_CHIP_COUNT * SPECTRA315_CHIP_BUFFER_BYTES);
}

/**
 * @brief       display sends the whole frame buffer to the panel and refreshes it.
 *
 * @param       bool _leaveOn
 *              If true, panel power is left on after the update.
 */
void EPDDriver::display(bool _leaveOn)
{
    displayStart();

    setPanelState(true);

    // Every chip's RAM is a fixed-size contiguous strip in DMemory4Bit, so each chip's
    // full image can be blasted as a single quad-SPI burst - no per-row interleaving
    // needed (unlike Inkplate13SPECTRA's 2-chip layout).
    for (uint8_t csx = 0; csx < SPECTRA315_CHIP_COUNT; csx++)
    {
        csSelect(csx);
        qspiWriteCommand(SPECTRA315_CMD_DTM);
        qspiWriteData(DMemory4Bit + (uint32_t)csx * SPECTRA315_CHIP_BUFFER_BYTES, SPECTRA315_CHIP_BUFFER_BYTES);
        csDeselect(csx);
    }

    sendPanelCommand(SPECTRA315_CMD_DRF, SPECTRA315_DRF_V, sizeof(SPECTRA315_DRF_V), 0xFF);
    waitForBusy();

    if (!_leaveOn)
        setPanelState(false);
    displayEnd();
}

/**
 * @brief       displayPartial refreshes only the specified rectangular region of the
 *              screen, using the PTLW (Partial Load Window) register of each of the 8
 *              driver ICs. Ported from the vendor's partialWindowUpdateWithImageData()/
 *              WithoutImageData(), generalized from 2 chips to 8 uneven-width chips.
 *
 * @param       int16_t x, int16_t y, int16_t w, int16_t h
 *              Update window in user (rotated) space.
 * @param       bool _leaveOn
 *              If true, panel power is left on after the update.
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

    // Map the user rectangle to panel-native (colStart..colEnd, rowStart..rowEnd),
    // mirroring the per-pixel transform in writePixelInternal.
    int16_t colStart, colEnd, rowStart, rowEnd;
    switch (_inkplate->getRotation())
    {
    case 0:
        colStart = (int16_t)E_INK_WIDTH - x - w;
        colEnd = (int16_t)E_INK_WIDTH - 1 - x;
        rowStart = (int16_t)E_INK_HEIGHT - y - h;
        rowEnd = (int16_t)E_INK_HEIGHT - 1 - y;
        break;
    case 2:
        colStart = x;
        colEnd = x + w - 1;
        rowStart = y;
        rowEnd = y + h - 1;
        break;
    case 3:
        colStart = (int16_t)E_INK_WIDTH - y - h;
        colEnd = (int16_t)E_INK_WIDTH - 1 - y;
        rowStart = x;
        rowEnd = x + w - 1;
        break;
    default:
    case 1:
        colStart = y;
        colEnd = y + h - 1;
        rowStart = (int16_t)E_INK_HEIGHT - x - w;
        rowEnd = (int16_t)E_INK_HEIGHT - 1 - x;
        break;
    }

    // Panel alignment requirements (per E Ink's partial-window app note):
    //   xStart and xPixel must both be multiples of 8 (host pixel domain).
    //   yStart + yLine must be even.
    // Chip boundaries (0,400,800,1200,1280,1680,2080,2480,2560) are all multiples of 8,
    // so clipping an 8-aligned full-panel window to a chip's range stays 8-aligned.
    colStart = (colStart / 8) * 8;
    colEnd = (((colEnd + 8) / 8) * 8) - 1;
    if (colEnd >= (int16_t)E_INK_WIDTH)
        colEnd = (int16_t)E_INK_WIDTH - 1;
    if (rowStart < 0)
        rowStart = 0;
    if ((rowEnd - rowStart + 1) % 2 != 0)
        rowEnd++;
    if (rowEnd >= (int16_t)E_INK_HEIGHT)
        rowEnd = (int16_t)E_INK_HEIGHT - 1;

    setPanelState(true);

    const uint16_t rowStride = SPECTRA315_CHIP_RAM_WIDTH / 2; // bytes per row per chip

    for (uint8_t csx = 0; csx < SPECTRA315_CHIP_COUNT; csx++)
    {
        int16_t chipXStart = (int16_t)chipXOffset[csx];
        int16_t chipXEnd = (int16_t)chipXOffset[csx + 1] - 1;
        bool intersects = (colStart <= chipXEnd) && (colEnd >= chipXStart);

        uint16_t localXStart, localXPixel, localYStart, localYLine;
        if (intersects)
        {
            int16_t lcs = (colStart > chipXStart) ? (colStart - chipXStart) : 0;
            int16_t lce = (colEnd < chipXEnd) ? (colEnd - chipXStart) : (chipXEnd - chipXStart);
            localXStart = lcs;
            localXPixel = lce - lcs + 1;
            localYStart = rowStart;
            localYLine = rowEnd - rowStart + 1;
        }
        else
        {
            // Every chip needs a PTLW+DTM cycle before DRF, or it falls back to a full
            // refresh on that chip (mirrors the vendor's per-chip minimal-window
            // requirement, e.g. Inkplate13SPECTRA's ptlwNull pattern).
            localXStart = 0;
            localXPixel = 8;
            localYStart = 0;
            localYLine = 2;
        }

        uint16_t HRST = localXStart * 2;
        uint16_t HRED = (localXStart + localXPixel) * 2 - 1;
        uint16_t VRST = localYStart / 2;
        uint16_t VRED = (localYStart + localYLine) / 2 - 1;

        uint8_t ptlwData[9];
        ptlwData[0] = HRST >> 8;
        ptlwData[1] = HRST & 0xFF;
        ptlwData[2] = HRED >> 8;
        ptlwData[3] = HRED & 0xFF;
        ptlwData[4] = VRST >> 8;
        ptlwData[5] = VRST & 0xFF;
        ptlwData[6] = VRED >> 8;
        ptlwData[7] = VRED & 0xFF;
        ptlwData[8] = SPECTRA315_PTLW_ENABLE;

        sendPanelCommand(SPECTRA315_CMD_PTLW, ptlwData, sizeof(ptlwData), (uint8_t)(1 << csx));

        csSelect(csx);
        qspiWriteCommand(SPECTRA315_CMD_DTM);
        {
            uint32_t chipBase = (uint32_t)csx * SPECTRA315_CHIP_BUFFER_BYTES;
            uint16_t bytesPerRow = localXPixel / 2;
            for (uint16_t row = 0; row < localYLine; row++)
            {
                uint32_t off = chipBase + (uint32_t)(localYStart + row) * rowStride + localXStart / 2;
                qspiWriteData(DMemory4Bit + off, bytesPerRow);
            }
        }
        csDeselect(csx);
    }

    sendPanelCommand(SPECTRA315_CMD_DRF, SPECTRA315_DRF_V, sizeof(SPECTRA315_DRF_V), 0xFF);
    waitForBusy();

    // Turn PTLW back off on all chips.
    uint8_t ptlwOff[9];
    memset(ptlwOff, 0, sizeof(ptlwOff));
    ptlwOff[8] = SPECTRA315_PTLW_DISABLE;
    sendPanelCommand(SPECTRA315_CMD_PTLW, ptlwOff, sizeof(ptlwOff), 0xFF);

    if (!_leaveOn)
        setPanelState(false);
    displayEnd();
}

/**
 * @brief       Returns the current panel power state, 0 for off, 1 for on.
 */
uint8_t EPDDriver::getPanelState()
{
    return _panelState;
}

/**
 * @brief       Sets the current panel power state.
 *
 * @param       uint8_t state
 *              1 to power up and initialize the panel, 0 to power it down.
 */
void EPDDriver::setPanelState(uint8_t state)
{
    if (state != _panelState)
    {
        if (state)
        {
            setPanelPinsToLow();
            delay(50);

            pinMode(SPECTRA315_RST_PIN, OUTPUT);
            pinMode(SPECTRA315_BUSY_PIN, INPUT);

            resetPanel();
            delay(100);

            // Vendor sample logs and proceeds even if a chip doesn't answer - it's up to
            // the caller to notice a bad image if a chip is truly dead.
            checkDriverICStatus();

            screenInit();
            pmic.powerUp();

            sendPanelCommand(SPECTRA315_CMD_PON, nullptr, 0, 0xFF);
            waitForBusy();
        }
        else
        {
            sendPanelCommand(SPECTRA315_CMD_POF, SPECTRA315_POF_V, sizeof(SPECTRA315_POF_V), 0xFF);
            waitForBusy(); // XON discharge, several seconds per the vendor's comment.
            pmic.powerDown();

            pinMode(SPECTRA315_RST_PIN, INPUT);
        }

        _panelState = state;
    }
}

/**
 * @brief       resetPanel performs a hardware reset of all 8 driver ICs.
 */
void EPDDriver::resetPanel()
{
    digitalWrite(SPECTRA315_RST_PIN, LOW);
    delay(20);
    digitalWrite(SPECTRA315_RST_PIN, HIGH);
    delay(20);
}

/**
 * @brief       Helps discharge capacitors before power-up - without this, the panel
 *              sometimes refuses to come up cleanly (same rationale as
 *              Inkplate13SPECTRA's setPanelPinsToLow()).
 */
void EPDDriver::setPanelPinsToLow()
{
    pinMode(SPECTRA315_RST_PIN, OUTPUT);
    pinMode(SPECTRA315_BUSY_PIN, OUTPUT);
    digitalWrite(SPECTRA315_RST_PIN, LOW);
    digitalWrite(SPECTRA315_BUSY_PIN, LOW);
}

/**
 * @brief       checkDriverICStatus reads the status register (0xF2) from every chip and
 *              checks that each one has bit0 set (ready).
 *
 * @return      1 if every chip responded, 0 if at least one didn't.
 */
uint8_t EPDDriver::checkDriverICStatus()
{
    uint8_t ok = 1;
    uint8_t buf[3];
    for (uint8_t csx = 0; csx < SPECTRA315_CHIP_COUNT; csx++)
    {
        memset(buf, 0, sizeof(buf));
        csSelect(csx);
        qspiReadData(SPECTRA315_CMD_STATUS, buf, sizeof(buf));
        csDeselect(csx);
        if ((buf[0] & 0x01) != 0x01)
            ok = 0;
    }
    return ok;
}

/**
 * @brief       screenInit sends the panel init sequence, ported register-for-register
 *              from the vendor's initEPD(). POFS is chip-role-specific: logical CS1 and
 *              CS5 get the "master" values, the other six get the "slave" values.
 */
void EPDDriver::screenInit()
{
    sendPanelCommand(SPECTRA315_CMD_VCOM_WOUT_EN, SPECTRA315_VCOM_WOUT_EN_V, sizeof(SPECTRA315_VCOM_WOUT_EN_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_TM_TCON, SPECTRA315_TM_TCON_V, sizeof(SPECTRA315_TM_TCON_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_CMD66, SPECTRA315_CMD66_V, sizeof(SPECTRA315_CMD66_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_PSR, SPECTRA315_PSR_V, sizeof(SPECTRA315_PSR_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_PWR, SPECTRA315_PWR_V, sizeof(SPECTRA315_PWR_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_PLL, SPECTRA315_PLL_V, sizeof(SPECTRA315_PLL_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_CDI, SPECTRA315_CDI_V, sizeof(SPECTRA315_CDI_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_TCON, SPECTRA315_TCON_V, sizeof(SPECTRA315_TCON_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_TRES, SPECTRA315_TRES_V, sizeof(SPECTRA315_TRES_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_EN_BUF, SPECTRA315_EN_BUF_V, sizeof(SPECTRA315_EN_BUF_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_PWS, SPECTRA315_PWS_V, sizeof(SPECTRA315_PWS_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_SPIM, SPECTRA315_SPIM_V, sizeof(SPECTRA315_SPIM_V), 0xFF);
    sendPanelCommand(SPECTRA315_CMD_CCSET, SPECTRA315_CCSET_V, sizeof(SPECTRA315_CCSET_V), 0xFF);

    const uint8_t masterMask = (1 << 1) | (1 << 5); // logical CS1 + CS5
    sendPanelCommand(SPECTRA315_CMD_POFS, SPECTRA315_POFS_MV, sizeof(SPECTRA315_POFS_MV), masterMask);
    sendPanelCommand(SPECTRA315_CMD_POFS, SPECTRA315_POFS_SV, sizeof(SPECTRA315_POFS_SV), (uint8_t)(~masterMask));

    sendPanelCommand(SPECTRA315_CMD_DCDC_TM, SPECTRA315_DCDC_TM_V, sizeof(SPECTRA315_DCDC_TM_V), 0xFF);
}

/**
 * @brief       Method waits until every selected chip is ready to accept new commands.
 */
void EPDDriver::waitForBusy()
{
    while (!digitalRead(SPECTRA315_BUSY_PIN))
    {
        delay(1);
    }
}

/**
 * @brief       Sends a command (+ optional parameter bytes) to the chips selected by
 *              csMask (bit i = logical CS i), asserting/deasserting their CS lines
 *              (on expander1) around a single-line SPI transaction.
 */
void EPDDriver::sendPanelCommand(uint8_t cmd, const uint8_t *params, uint32_t n, uint8_t csMask)
{
    csSelectMask(csMask);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.cmd = cmd;
    if (n != 0)
    {
        t.length = (size_t)n * 8;
        t.tx_buffer = params;
    }
    spi_device_transmit(_qspiDevice, &t);

    csDeselectMask(csMask);
}

/**
 * @brief       Asserts/deasserts a single chip's CS line (expander1, active low).
 */
void EPDDriver::csSelect(uint8_t csIndex)
{
    expander1.digitalWrite(csExpanderBit[csIndex], LOW);
}

void EPDDriver::csDeselect(uint8_t csIndex)
{
    expander1.digitalWrite(csExpanderBit[csIndex], HIGH);
}

/**
 * @brief       Asserts/deasserts every chip whose bit is set in csMask (bit i = logical
 *              CS i). Done as individual expander writes rather than a bulk port write,
 *              since the CS bits are only a fraction of expander1's bits and this stays
 *              simple/safe without risking clobbering the power-enable bits that share
 *              the same 16-bit port register.
 */
void EPDDriver::csSelectMask(uint8_t csMask)
{
    for (uint8_t i = 0; i < SPECTRA315_CHIP_COUNT; i++)
    {
        if (csMask & (1 << i))
            csSelect(i);
    }
}

void EPDDriver::csDeselectMask(uint8_t csMask)
{
    for (uint8_t i = 0; i < SPECTRA315_CHIP_COUNT; i++)
    {
        if (csMask & (1 << i))
            csDeselect(i);
    }
}

void EPDDriver::csDeselectAll()
{
    csDeselectMask(0xFF);
}

/**
 * @brief       Sets up the quad-SPI bus and device used to talk to the panel. Uses the
 *              ESP-IDF driver directly, since Arduino's SPIClass doesn't support quad
 *              transfers - see CLAUDE.md for the reasoning and the SPI3_HOST choice.
 */
void EPDDriver::qspiInit()
{
    spi_bus_config_t busCfg;
    memset(&busCfg, 0, sizeof(busCfg));
    busCfg.mosi_io_num = SPECTRA315_SPI_D0;
    busCfg.miso_io_num = SPECTRA315_SPI_D1;
    busCfg.sclk_io_num = SPECTRA315_SPI_SCK;
    busCfg.quadwp_io_num = SPECTRA315_SPI_D2;
    busCfg.quadhp_io_num = SPECTRA315_SPI_D3;
    busCfg.max_transfer_sz = SPECTRA315_CHIP_BUFFER_BYTES; // one full chip strip per burst

    spi_bus_initialize(SPI3_HOST, &busCfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devCfg;
    memset(&devCfg, 0, sizeof(devCfg));
    devCfg.command_bits = 8;
    devCfg.mode = 0;
    devCfg.clock_speed_hz = 10 * 1000 * 1000; // Conservative starting point - see CLAUDE.md.
    devCfg.spics_io_num = -1;                 // CS is manual, via expander1.
    devCfg.queue_size = 1;
    devCfg.flags = SPI_DEVICE_HALFDUPLEX;

    spi_bus_add_device(SPI3_HOST, &devCfg, &_qspiDevice);
}

/**
 * @brief       Sends a bare 8-bit command with no data phase, single-line.
 */
void EPDDriver::qspiWriteCommand(uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.cmd = cmd;
    spi_device_transmit(_qspiDevice, &t);
}

/**
 * @brief       Sends a quad-mode data burst with no command phase (the command must
 *              already have been sent via qspiWriteCommand(), with CS still asserted).
 *              This is only used for the large DTM image payload - register writes stay
 *              single-line via sendPanelCommand().
 */
void EPDDriver::qspiWriteData(const uint8_t *data, uint32_t len)
{
    spi_transaction_ext_t t;
    memset(&t, 0, sizeof(t));
    t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD;
    t.command_bits = 0; // command already sent separately
    t.base.length = (size_t)len * 8;
    t.base.tx_buffer = data;
    spi_device_transmit(_qspiDevice, (spi_transaction_t *)&t);
}

/**
 * @brief       Reads len bytes back after sending an 8-bit command, single-line (used
 *              for status/PWR/VCOM/TSC reads - the panel never returns data over quad
 *              mode in the vendor sample).
 */
void EPDDriver::qspiReadData(uint8_t cmd, uint8_t *data, uint32_t len)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.cmd = cmd;
    t.rxlength = (size_t)len * 8;
    t.rx_buffer = data;
    spi_device_transmit(_qspiDevice, &t);
}

/**
 * @brief       Allocates the frame buffer (8 contiguous 288000-byte chip strips in
 *              PSRAM) and builds the per-column lookup tables used by
 *              writePixelInternal().
 *
 * @return      0 if allocation failed, 1 if it succeeded.
 */
uint8_t EPDDriver::initializeFramebuffers()
{
    DMemory4Bit = (uint8_t *)ps_malloc((size_t)SPECTRA315_CHIP_COUNT * SPECTRA315_CHIP_BUFFER_BYTES);
    if (DMemory4Bit == NULL)
        return 0;

    for (uint16_t col = 0; col < E_INK_WIDTH; col++)
    {
        uint8_t chip = 0;
        while (chip < SPECTRA315_CHIP_COUNT - 1 && col >= chipXOffset[chip + 1])
            chip++;
        uint16_t localCol = col - chipXOffset[chip];
        _colChipByteBase[col] = (uint32_t)chip * SPECTRA315_CHIP_BUFFER_BYTES;
        _colByteInRow[col] = localCol / 2;
        _colNibble[col] = localCol & 1;
    }

    memset(DMemory4Bit, INKPLATE_WHITE | (INKPLATE_WHITE << 4), (size_t)SPECTRA315_CHIP_COUNT * SPECTRA315_CHIP_BUFFER_BYTES);

    return 1;
}

/**
 * @brief       sdCardInit initializes the SD card over SPI.
 *
 * @return      0 if failed to initialize, 1 if successful.
 */
int16_t EPDDriver::sdCardInit()
{
    expander1.pinMode(SPECTRA315_EXP_SD_ENABLE, OUTPUT);
    expander1.digitalWrite(SPECTRA315_EXP_SD_ENABLE, LOW);
    delay(200);
    spi1.begin(SPECTRA315_SD_SCK, SPECTRA315_SD_MISO, SPECTRA315_SD_MOSI, SPECTRA315_SD_CS);
    setSdCardOk(sd.begin(SdSpiConfig(SPECTRA315_SD_CS, SHARED_SPI, SD_SCK_MHZ(25), &spi1)));
    return getSdCardOk();
}

/**
 * @brief       sdCardSleep turns off the SD card supply to save energy in deep sleep.
 */
void EPDDriver::sdCardSleep()
{
    pinMode(SPECTRA315_SD_CS, INPUT);
    pinMode(SPECTRA315_SD_MOSI, INPUT);
    pinMode(SPECTRA315_SD_SCK, INPUT);
    pinMode(SPECTRA315_SD_MISO, INPUT);

    expander1.pinMode(SPECTRA315_EXP_SD_ENABLE, INPUT);
}

/**
 * @brief       getSdFat gets the SD card object.
 */
SdFat &EPDDriver::getSdFat()
{
    return sd;
}

/**
 * @brief       getSPIptr gets the SPI class object pointer used for the SD card.
 */
SPIClass *EPDDriver::getSPIptr()
{
    return &spi1;
}

/**
 * @brief       setSdCardOk sets the SD card OK status.
 */
void EPDDriver::setSdCardOk(int16_t s)
{
    _sdCardOk = s;
}

/**
 * @brief       getSdCardOk gets the SD card OK status.
 */
int16_t EPDDriver::getSdCardOk()
{
    return _sdCardOk;
}

/**
 * @brief       readBattery reads the battery voltage.
 *
 * @return      Battery voltage in volts.
 */
double EPDDriver::readBattery()
{
    // Read the pin on the battery MOSFET to figure out board revision (PMOS-only vs
    // PMOS+NMOS), same convention as Inkplate13SPECTRA.
    expander1.pinMode(SPECTRA315_EXP_VBAT_MOS, INPUT);
    int state = expander1.digitalRead(SPECTRA315_EXP_VBAT_MOS);
    expander1.pinMode(SPECTRA315_EXP_VBAT_MOS, OUTPUT);

    if (state)
        expander1.digitalWrite(SPECTRA315_EXP_VBAT_MOS, LOW);
    else
        expander1.digitalWrite(SPECTRA315_EXP_VBAT_MOS, HIGH);

    delay(5);

    analogReadResolution(12);
    int adc = analogReadMilliVolts(SPECTRA315_VBAT_ADC);

    if (state)
        expander1.digitalWrite(SPECTRA315_EXP_VBAT_MOS, HIGH);
    else
        expander1.digitalWrite(SPECTRA315_EXP_VBAT_MOS, LOW);

    return (double(adc) * 2.0 / 1000);
}

#endif
