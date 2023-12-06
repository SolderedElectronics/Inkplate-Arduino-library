/**
 **************************************************
 *
 * @file        InkplateColor.cpp
 * @brief       Basic funtions for controling Inkplate 6COLOR
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

#ifdef ARDUINO_INKPLATECOLOR

SPIClass epdSPI(VSPI);

SPISettings epdSpiSettings(2000000, MSBFIRST, SPI_MODE0);

/**
 * @brief       begin function initialize Inkplate object with predefined
 * settings
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
bool Inkplate::begin(void)
{
    unsigned long _timeout = 0;

    // Check if begin is called already. If it is, that means that panel needs to
    // be reseted in order to be woken up from deep sleep. If is this first time
    // begin has been called, init SPI, init pins, allocate memory for frame
    // buffer and clear frame buffer
    if (!_beginDone)
    {
        Wire.begin();

        // Allocate memory for internal frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Color whole frame buffer in white color
        memset(DMemory4Bit, INKPLATE_WHITE | (INKPLATE_WHITE << 4), E_INK_WIDTH * E_INK_HEIGHT / 2);


        _beginDone = true;

        Serial.println("the display has NOT been previously init'ed, starting up...");
        // The display has not been initialized before
        // Wake the ePaper and initialize everything
        // If it fails, return false
        if (!setPanelDeepSleep(false))
            return false;


        delay(1000);
    }
    else
    {
        Serial.println("the display has been previously init'ed, resetting...");
        resetPanel();
    }

    return true;
}

/**
 * @brief       display function update display with new data from buffer
 *
 */
void Inkplate::display()
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
    epdSPI.writeBytes(DMemory4Bit, E_INK_WIDTH * E_INK_HEIGHT / 2);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);

    sendCommand(POWER_OFF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(POWER_OFF_REGISTER);
    while (digitalRead(EPAPER_BUSY_PIN))
        ; // Wait for busy low signal
    delay(200);
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
void Graphics::writePixel(int16_t x0, int16_t y0, uint16_t _color)
{
    if (x0 > width() - 1 || y0 > height() - 1 || x0 < 0 || y0 < 0)
        return;
    if (_color > 6)
        return;

    switch (rotation) // FIXED
    {
    case 3:
        _swap_int16_t(x0, y0);
        x0 = height() - x0 - 1;
        break;
    case 0:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 1:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    int _x = x0 / 2;
    int _x_sub = x0 % 2;
    uint8_t temp;
    temp = *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x);
    *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x) = (pixelMaskGLUT[_x_sub] & temp) | (_x_sub ? _color : _color << 4);
}

/**
 * @brief       clean function cleans screen of any potential burn in
 *
 * @note        Should not be used in intervals smaller than 5 seconds

 */
void Inkplate::clean()
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
        ; // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(POWER_OFF_REGISTER);
    while (digitalRead(EPAPER_BUSY_PIN))
        ; // Wait for busy low signal
    delay(200);
}

/**
 * @brief       resetPanel resets Inkplate 6COLOR
 */
void Inkplate::resetPanel()
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
void Inkplate::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_DC_PIN, LOW);
    digitalWrite(EPAPER_CS_PIN, LOW);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.transfer(_command);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
}

/**
 * @brief       sendData sends SPI data to Inkplate 6COLOR
 *
 * @param       uint8_t *_data
 *              pointer to data buffer to be sent to epaper
 * @param       int _n
 *              number of data bytes
 */
void Inkplate::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes(_data, _n);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
}

/**
 * @brief       sendData sends SPI data to Inkplate 6COLOR
 *
 * @param       uint8_t _data
 *              data buffer to be sent to epaper
 */
void Inkplate::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.transfer(_data);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
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
bool Inkplate::setPanelDeepSleep(bool _state)
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
        // This is not used in the driver because waking up from deep sleep causes brownout

        delay(10);
        sendCommand(DEEP_SLEEP_REGISTER);
        sendData(0xA5);
        delay(100);
        digitalWrite(EPAPER_RST_PIN, LOW);
        digitalWrite(EPAPER_DC_PIN, LOW);
        digitalWrite(EPAPER_CS_PIN, LOW);

        epdSPI.end();

        return true;
    }
}


/**
 * @brief       setIOExpanderForLowPower initiates I/O Expander pins for low power, and puts
 * them in OUTPUT LOW because they are using least amount of current in deep
 * sleep that way
 */
void Inkplate::setIOExpanderForLowPower()
{
    Wire.begin();
    memset(ioRegsInt, 0, 22);
    ioBegin(IO_INT_ADDR, ioRegsInt);

    // TOUCHPAD PINS
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B2, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B3, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B4, INPUT);

    // Battery voltage Switch MOSFET
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B1, OUTPUT);

    // Rest of pins go to OUTPUT LOW state because in deepSleep mode they are
    // using least amount of power
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A0, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A1, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A2, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A3, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A4, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A5, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A6, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A7, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B0, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B5, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B6, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B7, OUTPUT);

    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A0, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A1, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A2, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A3, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A4, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A5, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A6, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_A7, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B0, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B5, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B6, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B7, LOW);
}

void Inkplate::sleepColorPanel()
{
    Serial.println("putting the color panel to sleep...");
    delay(10);
    sendCommand(DEEP_SLEEP_REGISTER);
    sendData(0x10);
    sendData(0xA5);
    delay(100);
    digitalWrite(EPAPER_RST_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, LOW);
    digitalWrite(EPAPER_CS_PIN, LOW);

}

#endif