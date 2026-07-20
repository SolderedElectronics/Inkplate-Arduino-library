// Header guard for the Arduino include
#ifdef ARDUINO_ESP32S3_DEV
#include "Inkplate7SPECTRADriver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

// SPI used for the MicroSd card
SPIClass spi1(1);

// Instance of the SdFat object
SdFat sd;

// SPI settings for communicating with the display, 8MHz
SPISettings epdSpiSettings(8000000, MSBFIRST, SPI_MODE0);

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

    // The panel is mounted rotated by 180 degrees inside the enclosure, so
    // rotation 0 (the default) maps user coordinates to flipped panel coordinates.
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

        // Default rotation - the panel is mounted rotated by 180 degrees,
        // rotation 0 compensates for that (see writePixelInternal).
        _inkplate->setRotation(0);

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

    // Send the whole framebuffer to the panel over SPI.
    // The command byte is sent with the DC pin low, pixel data with the DC pin high
    // (4-wire SPI interface mode).
    spiStart();
    SPI.beginTransaction(epdSpiSettings);

    // Send the command to fill the EPD framebuffer with data.
    digitalWrite(SPECTRA73_DC_PIN, LOW);
    digitalWrite(SPECTRA73_CS_PIN, LOW);
    SPI.write(SPECTRA73_REGISTER_DTM);
    digitalWrite(SPECTRA73_CS_PIN, HIGH);
    digitalWrite(SPECTRA73_DC_PIN, HIGH);

    // Send the data row by row.
    digitalWrite(SPECTRA73_CS_PIN, LOW);
    for (int i = 0; i < E_INK_HEIGHT; i++)
    {
        SPI.writeBytes(DMemory4Bit + (i * E_INK_WIDTH / 2), (E_INK_WIDTH / 2));
    }
    digitalWrite(SPECTRA73_CS_PIN, HIGH);

    SPI.endTransaction();
    spiEnd();
    waitForBusy();

    // Force display command.
    sendCommand(SPECTRA73_REGISTER_DRF, SPECTRA73_REGISTER_DRF_V, sizeof(SPECTRA73_REGISTER_DRF_V));
    waitForBusy();

    // Disable power to the display (if needed).
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
            digitalWrite(SPECTRA73_PWR_EN, HIGH);

            // Wait a little bit.
            delay(100ULL);

            // First, do a hardware reset!
            resetPanel();

            // Wait for the panel to be ready after the reset.
            waitForBusy();

            // Initialze the screen by sending the magic values to the registers provided by the manufacturer.
            screenInit();

            // Power up a screen.
            sendCommand(SPECTRA73_REGISTER_PON, nullptr, 0);
            waitForBusy();
        }
        else
        {
            // Power off the screen.
            sendCommand(SPECTRA73_REGISTER_POF, SPECTRA73_REGISTER_POF_V, sizeof(SPECTRA73_REGISTER_POF_V));
            waitForBusy();

            // Disable power to the screen.
            digitalWrite(SPECTRA73_PWR_EN, LOW);

            // Disable GPIOs.
            pinMode(SPECTRA73_DC_PIN, INPUT);
            pinMode(SPECTRA73_CS_PIN, INPUT);
            pinMode(SPECTRA73_RST_PIN, INPUT);
            pinMode(SPECTRA73_BUSYN_PIN, INPUT);
            pinMode(SPECTRA73_PWR_EN, INPUT);
        }

        // Update the status variable.
        _panelState = state;
    }
}

/**
 * @brief       initializes the communication pins as well as SPI communication with the Inkplate 7 panel
 *
 */
void EPDDriver::setIO()
{
    // Config the GPIOs.
    pinMode(SPECTRA73_DC_PIN, OUTPUT);
    pinMode(SPECTRA73_CS_PIN, OUTPUT);
    pinMode(SPECTRA73_RST_PIN, OUTPUT);
    pinMode(SPECTRA73_BUSYN_PIN, INPUT_PULLUP);
    pinMode(SPECTRA73_PWR_EN, OUTPUT);
    pinMode(SPECTRA73_BS0, OUTPUT);
    pinMode(SPECTRA73_BS1, OUTPUT);

    // Set their default states.
    // BS0 and BS1 low select the 4-wire SPI interface mode on the panel.
    digitalWrite(SPECTRA73_DC_PIN, HIGH);
    digitalWrite(SPECTRA73_CS_PIN, HIGH);
    digitalWrite(SPECTRA73_RST_PIN, LOW);
    digitalWrite(SPECTRA73_PWR_EN, LOW);
    digitalWrite(SPECTRA73_BS0, LOW);
    digitalWrite(SPECTRA73_BS1, LOW);

    // Config SPI. MISO is not used by the panel.
    if (!SPI.begin(SPECTRA73_SPI_SCK, -1, SPECTRA73_SPI_MOSI))
    {
        Serial.println("Failed to init SPI");
    }
}

/**
 * @brief       resetPanel resets Inkplate 7 screen
 */
void EPDDriver::resetPanel()
{
    // Toggle the reset pin to initialte HW reset.
    digitalWrite(SPECTRA73_RST_PIN, LOW);
    delay(10ULL);
    digitalWrite(SPECTRA73_RST_PIN, HIGH);
    delay(20ULL);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 7
 *
 * @param       uint8_t _cmd
 *              predefined command for epaper control
 * @param       const uint8_t *_parameters
 *              command parameters (data bytes), can be nullptr
 * @param       uint32_t _n
 *              number of parameter bytes
 */
void EPDDriver::sendCommand(uint8_t _cmd, const uint8_t *_parameters, uint32_t _n)
{
    // Config the SPI.
    spiStart();
    SPI.beginTransaction(epdSpiSettings);

    // Send the command byte with the DC pin set to low.
    digitalWrite(SPECTRA73_DC_PIN, LOW);
    digitalWrite(SPECTRA73_CS_PIN, LOW);
    SPI.write(_cmd);
    digitalWrite(SPECTRA73_CS_PIN, HIGH);

    // Send the command parameters with the DC pin set to high.
    digitalWrite(SPECTRA73_DC_PIN, HIGH);
    if (_n != 0)
    {
        digitalWrite(SPECTRA73_CS_PIN, LOW);
        SPI.writeBytes(_parameters, _n);
        digitalWrite(SPECTRA73_CS_PIN, HIGH);
    }

    SPI.endTransaction();
    spiEnd();
}


/**
 * @brief       screenInit sends init commands to the panel.
 */
void EPDDriver::screenInit()
{
    // Send magic values to the registers. These values are provided from the manufacturer.
    sendCommand(SPECTRA73_REGISTER_CMDH, SPECTRA73_REGISTER_CMDH_V, sizeof(SPECTRA73_REGISTER_CMDH_V));
    sendCommand(SPECTRA73_REGISTER_PWR, SPECTRA73_REGISTER_PWR_V, sizeof(SPECTRA73_REGISTER_PWR_V));
    sendCommand(SPECTRA73_REGISTER_PSR, SPECTRA73_REGISTER_PSR_V, sizeof(SPECTRA73_REGISTER_PSR_V));
    sendCommand(SPECTRA73_REGISTER_PFS, SPECTRA73_REGISTER_PFS_V, sizeof(SPECTRA73_REGISTER_PFS_V));
    sendCommand(SPECTRA73_REGISTER_BTST1, SPECTRA73_REGISTER_BTST1_V, sizeof(SPECTRA73_REGISTER_BTST1_V));
    sendCommand(SPECTRA73_REGISTER_BTST2, SPECTRA73_REGISTER_BTST2_V, sizeof(SPECTRA73_REGISTER_BTST2_V));
    sendCommand(SPECTRA73_REGISTER_BTST3, SPECTRA73_REGISTER_BTST3_V, sizeof(SPECTRA73_REGISTER_BTST3_V));
    sendCommand(SPECTRA73_REGISTER_IPC, SPECTRA73_REGISTER_IPC_V, sizeof(SPECTRA73_REGISTER_IPC_V));
    sendCommand(SPECTRA73_REGISTER_PLL, SPECTRA73_REGISTER_PLL_V, sizeof(SPECTRA73_REGISTER_PLL_V));
    sendCommand(SPECTRA73_REGISTER_TSE, SPECTRA73_REGISTER_TSE_V, sizeof(SPECTRA73_REGISTER_TSE_V));
    sendCommand(SPECTRA73_REGISTER_CDI, SPECTRA73_REGISTER_CDI_V, sizeof(SPECTRA73_REGISTER_CDI_V));
    sendCommand(SPECTRA73_REGISTER_TCON, SPECTRA73_REGISTER_TCON_V, sizeof(SPECTRA73_REGISTER_TCON_V));
    sendCommand(SPECTRA73_REGISTER_TRES, SPECTRA73_REGISTER_TRES_V, sizeof(SPECTRA73_REGISTER_TRES_V));
    sendCommand(SPECTRA73_REGISTER_VDCS, SPECTRA73_REGISTER_VDCS_V, sizeof(SPECTRA73_REGISTER_VDCS_V));
    sendCommand(SPECTRA73_REGISTER_T_VDCS, SPECTRA73_REGISTER_T_VDCS_V, sizeof(SPECTRA73_REGISTER_T_VDCS_V));
    sendCommand(SPECTRA73_REGISTER_AGID, SPECTRA73_REGISTER_AGID_V, sizeof(SPECTRA73_REGISTER_AGID_V));
    sendCommand(SPECTRA73_REGISTER_PWS, SPECTRA73_REGISTER_PWS_V, sizeof(SPECTRA73_REGISTER_PWS_V));
    sendCommand(SPECTRA73_REGISTER_CCSET, SPECTRA73_REGISTER_CCSET_V, sizeof(SPECTRA73_REGISTER_CCSET_V));
    sendCommand(SPECTRA73_REGISTER_TSSET, SPECTRA73_REGISTER_TSSET_V, sizeof(SPECTRA73_REGISTER_TSSET_V));
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
    spi1.begin(SPECTRA73_SD_SPI_CLK, SPECTRA73_SD_SPI_MISO, SPECTRA73_SD_SPI_MOSI, SPECTRA73_SD_SPI_CS);
    setSdCardOk(sd.begin(SdSpiConfig(SPECTRA73_SD_SPI_CS, SHARED_SPI, SD_SCK_MHZ(25), &spi1)));
    return getSdCardOk();
}

/**
 * @brief       sdCardSleep turns off the P-MOS which powers the sd card to save energy in deep sleep
 */
void EPDDriver::sdCardSleep()
{
    // Set SPI pins to input to reduce power consumption in deep sleep
    pinMode(SPECTRA73_SD_SPI_CS, INPUT);
    pinMode(SPECTRA73_SD_SPI_MOSI, INPUT);
    pinMode(SPECTRA73_SD_SPI_CLK, INPUT);
    pinMode(SPECTRA73_SD_SPI_MISO, INPUT);

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
    expander1.pinMode(SPECTRA73_BATT_MOSFET_PIN, INPUT);
    int state = expander1.digitalRead(SPECTRA73_BATT_MOSFET_PIN);
    expander1.pinMode(SPECTRA73_BATT_MOSFET_PIN, OUTPUT);

    // If the input is pulled high, it's PMOS only.
    // If it's pulled low, it's PMOS and NMOS.
    if (state)
    {
        expander1.digitalWrite(SPECTRA73_BATT_MOSFET_PIN, LOW);
    }
    else
    {
        expander1.digitalWrite(SPECTRA73_BATT_MOSFET_PIN, HIGH);
    }

    // Wait a little bit after a MOSFET enable.
    delay(5);

    // Set to the highest resolution and read the voltage.
    analogReadResolution(12);
    int adc = analogReadMilliVolts(SPECTRA73_BATT_ADC_PIN);

    // Turn off the MOSFET (and voltage divider).
    if (state)
    {
        expander1.digitalWrite(SPECTRA73_BATT_MOSFET_PIN, HIGH);
    }
    else
    {
        expander1.digitalWrite(SPECTRA73_BATT_MOSFET_PIN, LOW);
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
    while (!digitalRead(SPECTRA73_BUSYN_PIN))
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
    pinMode(SPECTRA73_DC_PIN, OUTPUT);
    pinMode(SPECTRA73_CS_PIN, OUTPUT);
    pinMode(SPECTRA73_RST_PIN, OUTPUT);
    pinMode(SPECTRA73_BUSYN_PIN, OUTPUT);
    pinMode(SPECTRA73_PWR_EN, OUTPUT);
    pinMode(SPECTRA73_BS0, OUTPUT);
    pinMode(SPECTRA73_BS1, OUTPUT);

    digitalWrite(SPECTRA73_DC_PIN, LOW);
    digitalWrite(SPECTRA73_CS_PIN, LOW);
    digitalWrite(SPECTRA73_RST_PIN, LOW);
    digitalWrite(SPECTRA73_BUSYN_PIN, LOW);
    digitalWrite(SPECTRA73_PWR_EN, LOW);
    digitalWrite(SPECTRA73_BS0, LOW);
    digitalWrite(SPECTRA73_BS1, LOW);
}


#endif
