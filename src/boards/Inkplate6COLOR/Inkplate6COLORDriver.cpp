/**
 **************************************************
 *
 * @file        Inkplate6COLORDriver.cpp
 * @brief       Low level driver for the Inkplate 6 COLOR e-paper panel
 *
 *              The Inkplate 6 COLOR uses a 5.83" 600x448 seven colour e-paper panel driven
 *              over SPI. The panel controller generates its own driving voltages, so the
 *              driver only handles the panel power enable, reset, the busy line and the SPI
 *              traffic, while the on board GPIO expander takes care of the peripherals.
 *
 *              The frame buffer holds one 4 bit colour index per pixel, two pixels per byte,
 *              and supports the seven panel colours.
 *              Only full updates are supported. The board also exposes the touchpad buttons.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATECOLOR
#include "Inkplate6COLORDriver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

SPIClass spi2(2);
SdFat sd;

SPIClass epdSPI(VSPI);
SPISettings epdSpiSettings(2000000, MSBFIRST, SPI_MODE0);

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
    if (color > 6)
        return;

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
    unsigned long _timeout = 0;

    // Check if begin is called already. If it is, that means that panel needs to
    // be reseted in order to be woken up from deep sleep. If is this first time
    // begin has been called, init SPI, init pins, allocate memory for frame
    // buffer and clear frame buffer
    if (!_beginDone)
    {

        Wire.begin();

        // Save the given inkplate pointer for internal use
        _inkplate = _inkplatePtr;

        image.begin(_inkplatePtr);

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

    // Set IO expander to the output low state to reduce current consumption in sleep.
    setIOExpanderForLowPower();

    // Disable microSD card for the same reason.
    sdCardSleep();

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
    // Wake the panel back up
    setPanelDeepSleep(false);

    // Set resolution setting
    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xc0};
    sendCommand(0x61);
    sendData(res_set_data, 4);

    // Push pixel data to the epaper RAM
    sendCommand(0x10);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    spiStart();
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes(DMemory4Bit, E_INK_WIDTH * E_INK_HEIGHT / 2);
    epdSPI.endTransaction();
    spiEnd();
    digitalWrite(EPAPER_CS_PIN, HIGH);

    sendCommand(POWER_OFF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        delay(1); // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        delay(1); // Wait for busy high signal
    sendCommand(0x02);
    while (digitalRead(EPAPER_BUSY_PIN))
        delay(1); // Wait for busy low signal
    delay(200);

    // Put the panel to sleep again
    setPanelDeepSleep(true);
    displayEnd();
}


/**
 * @brief       Returns the current panel power state.
 *
 * @return      1 if the panel is powered on, 0 if powered off.
 */
uint8_t EPDDriver::getPanelState()
{
    return _panelState;
}

/**
 * @brief       Sets the panel power state variable.
 *
 * @param       uint8_t state
 *              1 to mark the panel as powered on, 0 for powered off.
 */
void EPDDriver::setPanelState(uint8_t state)
{
    _panelState = state;
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
void EPDDriver::clean()
{

    // Set resolution setting
    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xc0};
    sendCommand(0x61);
    sendData(res_set_data, 4);

    // Push pixel data to the epaper RAM
    sendCommand(0x10);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    epdSPI.beginTransaction(epdSpiSettings);
    for (uint32_t i = 0; i < (E_INK_WIDTH * E_INK_HEIGHT / 2); i++)
    {
        epdSPI.transfer(INKPLATE_WHITE | INKPLATE_WHITE << 4);
    }
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);

    sendCommand(POWER_OFF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        delay(1); // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        delay(1); // Wait for busy high signal
    sendCommand(POWER_OFF_REGISTER);
    while (digitalRead(EPAPER_BUSY_PIN))
        delay(1); // Wait for busy low signal
    delay(200);
}

/**
 * @brief       resetPanel resets Inkplate 6COLOR
 */
void EPDDriver::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(1);
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(200);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 6COLOR
 *
 * @param       uint8_t _command
 *              predefined command for epaper control
 */
void EPDDriver::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, LOW);
    delayMicroseconds(10);
    spiStart();
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.transfer(_command);
    epdSPI.endTransaction();
    spiEnd();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 6COLOR
 *
 * @param       uint8_t *_data
 *              pointer to data buffer to be sent to epaper
 * @param       int _n
 *              number of data bytes
 */
void EPDDriver::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    spiStart();
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes(_data, _n);
    epdSPI.endTransaction();
    spiEnd();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 6COLOR
 *
 * @param       uint8_t _data
 *              data buffer to be sent to epaper
 */
void EPDDriver::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    spiStart();
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.transfer(_data);
    epdSPI.endTransaction();
    spiEnd();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       setPanelDeepSleep puts the color ePaper into deep sleep, or wakes it and reinitializes it
 *
 * @param       bool _state
 *              -'True' sets the panel to sleep
 *              -'False' wakes the panel
 *
 * @returns     True if successful, False if unsuccessful
 *
 */
bool EPDDriver::setPanelDeepSleep(bool _state)
{
    if (!_state)
    {
        // _state is false? Wake the panel!

        // Send commands to power up panel. According to the datasheet, it can be
        // powered up from deep sleep only by reseting it and doing reinit.

        // Init SPI
        epdSPI.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);
        pinMode(EPAPER_BUSY_PIN, INPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_CS_PIN, OUTPUT);

        // De-select epaper (and also try to charge capacitors to avoid big in-rush current).
        digitalWrite(EPAPER_DC_PIN, HIGH);
        digitalWrite(EPAPER_CS_PIN, HIGH);

        // Wait a little bit.
        delay(100);

        // Reset epaper
        resetPanel();

        // Wait for ePaper to be ready by reading busy high signal
        float _timeout = millis();
        while ((!digitalRead(EPAPER_BUSY_PIN)) && ((millis() - _timeout) < INIT_TIMEOUT))
            ;

        if (!digitalRead(EPAPER_BUSY_PIN))
            return false;

        // Send whole bunch of commands and data
        uint8_t panel_set_data[] = {0xEF, 0x08};
        sendCommand(PANEL_SET_REGISTER);
        sendData(panel_set_data, 2);

        uint8_t power_set_data[] = {0x37, 0x00, 0x05, 0x05};
        sendCommand(POWER_SET_REGISTER);
        sendData(power_set_data, 4);

        sendCommand(POWER_OFF_SEQ_SET_REGISTER);
        sendData(0x00);

        uint8_t booster_softstart_data[] = {0xC7, 0xC7, 0x1D};
        sendCommand(BOOSTER_SOFTSTART_REGISTER);
        sendData(booster_softstart_data, 3);

        sendCommand(TEMP_SENSOR_EN_REGISTER);
        sendData(0x00);

        sendCommand(VCOM_DATA_INTERVAL_REGISTER);
        sendData(0x37);

        sendCommand(0x60);
        sendData(0x20);

        uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xC0};
        sendCommand(RESOLUTION_SET_REGISTER);
        sendData(res_set_data, 4);

        sendCommand(0xE3);
        sendData(0xAA);

        delay(100);
        sendCommand(VCOM_DATA_INTERVAL_REGISTER);
        sendData(0x37);

        return true;
    }
    else
    {
        // _state is true? Put the panel to sleep.

        delay(10);
        sendCommand(DEEP_SLEEP_REGISTER);
        sendData(0xA5);
        delay(100);
        digitalWrite(EPAPER_RST_PIN, LOW);
        delay(100);
        digitalWrite(EPAPER_DC_PIN, LOW);
        digitalWrite(EPAPER_CS_PIN, LOW);
        epdSPI.end();

        pinMode(EPAPER_CLK, OUTPUT);
        pinMode(EPAPER_DIN, OUTPUT);
        digitalWrite(EPAPER_CLK, LOW);
        digitalWrite(EPAPER_DIN, LOW);

        return true;
    }
}

/**
 * @brief       setIOExpanderForLowPower initiates I/O Expander pins for low power, and puts
 * them in OUTPUT LOW because they are using least amount of current in deep
 * sleep that way
 */
void EPDDriver::setIOExpanderForLowPower()
{
    Wire.begin();
    expander1.begin(IO_INT_ADDR);

    // TOUCHPAD PINS
    expander1.pinMode(IO_PIN_B2, INPUT);
    expander1.pinMode(IO_PIN_B3, INPUT);
    expander1.pinMode(IO_PIN_B4, INPUT);

    // Battery voltage Switch MOSFET
    expander1.pinMode(IO_PIN_B1, OUTPUT);
    expander1.digitalWrite(IO_PIN_B1, LOW);

    // Rest of pins go to OUTPUT LOW state because in deepSleep mode they are
    // using least amount of power
    expander1.pinMode(IO_PIN_A0, OUTPUT);
    expander1.pinMode(IO_PIN_A1, OUTPUT);
    expander1.pinMode(IO_PIN_A2, OUTPUT);
    expander1.pinMode(IO_PIN_A3, OUTPUT);
    expander1.pinMode(IO_PIN_A4, OUTPUT);
    expander1.pinMode(IO_PIN_A5, OUTPUT);
    expander1.pinMode(IO_PIN_A6, OUTPUT);
    expander1.pinMode(IO_PIN_A7, OUTPUT);
    expander1.pinMode(IO_PIN_B0, OUTPUT);
    expander1.pinMode(IO_PIN_B5, OUTPUT);
    expander1.pinMode(IO_PIN_B6, OUTPUT);
    expander1.pinMode(IO_PIN_B7, OUTPUT);

    expander1.digitalWrite(IO_PIN_A0, LOW);
    expander1.digitalWrite(IO_PIN_A1, LOW);
    expander1.digitalWrite(IO_PIN_A2, LOW);
    expander1.digitalWrite(IO_PIN_A3, LOW);
    expander1.digitalWrite(IO_PIN_A4, LOW);
    expander1.digitalWrite(IO_PIN_A5, LOW);
    expander1.digitalWrite(IO_PIN_A6, LOW);
    expander1.digitalWrite(IO_PIN_A7, LOW);
    expander1.digitalWrite(IO_PIN_B0, LOW);
    expander1.digitalWrite(IO_PIN_B5, LOW);
    expander1.digitalWrite(IO_PIN_B6, LOW);
    expander1.digitalWrite(IO_PIN_B7, LOW);
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


#endif
