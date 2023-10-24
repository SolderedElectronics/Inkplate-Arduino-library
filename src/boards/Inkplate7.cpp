/**
 **************************************************
 *
 * @file        Inkplate7.cpp
 * @brief       Basic funtions for controling Inkplate 7
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For support, please reach over forums: https://forum.soldered.com/
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact hello@soldered.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#include "../Inkplate.h"
#include "../include/Graphics.h"
#include "../include/defines.h"

#ifdef ARDUINO_INKPLATE7

SPISettings epdSpiSettings(4000000UL, MSBFIRST, SPI_MODE0);
SPIClass epdSPI(VSPI);

/**
 * @brief       begin function to initialize Inkplate object with predefined
 * settings
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
bool Inkplate::begin()
{
    if (!_beginDone)
    {
        // Allocate memory for frame buffer
        // IP7 is 2 pixels per byte
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);

        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Set SPI Pins for microSD card to input pull down to reduce power consumption.
        pinMode(12, INPUT_PULLDOWN);
        pinMode(13, INPUT_PULLDOWN);
        pinMode(14, INPUT_PULLDOWN);
        pinMode(15, INPUT_PULLDOWN);

        // Init TwoWire and GPIO expander
        setIOExpanderForLowPower();

        // Disable the microSD card pin for now.
        pinModeInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, OUTPUT);
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SD_PMOS_PIN, HIGH);

        // Set the default text color
        setTextColor(INKPLATE7_BLACK);

        // Clear frame buffer
        clearDisplay();

        _beginDone = 1;
    }

    // Wake the ePaper and initialize everything
    // If it fails, return false
    if (!setPanelDeepSleep(false))
        return false;

    // Put the panel to deep sleep
    // The panel is always in sleep unless it's being written display data to
    setPanelDeepSleep(true);
    return true;
}


/**
 * @brief       Display function that updates display with new data from buffer
 *
 */
void Inkplate::display()
{
    // Wake ePaper from deep sleep
    setPanelDeepSleep(false);
    delay(5);

    // Send pixel data
    sendCommand(0x10);

    // Send the pixel data
    sendData(DMemory4Bit, (E_INK_WIDTH * E_INK_HEIGHT / 2));

    // EPD update
    sendCommand(0x12); // Display Update Control

    delayMicroseconds(500); // Wait at least 200 uS
    waitForEpd(35000);

    // Go back to sleep
    setPanelDeepSleep(true);
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

        // Start SPI
        // Set SPI pins
        epdSPI.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);
        // Set up EPD communication pins
        pinMode(EPAPER_CS_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT_PULLUP);

        delay(10);

        // Reset EPD IC
        resetPanel();

        // Power on sequence
        sendCommand(0X65); // Flash control
        sendData(0x01);
        sendCommand(0xAB);
        sendCommand(0X65); // Flash control
        sendData(0x00);
        sendCommand(0x01);
        sendData(0x37); // Power setting
        sendData(0x00);
        sendCommand(0X00); // Panel setting
        sendData(0xCF);
        sendData(0x08);
        sendCommand(0x06); // Boost
        sendData(0xc7);
        sendData(0xcc);
        sendData(0x28);
        sendCommand(0x30); // PLL setting
        sendData(0x3c);
        sendCommand(0X41); // Temperature setting
        sendData(0x00);
        sendCommand(0X50); // VCOM and data interval setting
        sendData(0x77);
        sendCommand(0X60); // TCON setting
        sendData(0x22);
        sendCommand(0x61); // Resolution 640*384
        sendData(0x02);
        sendData(0x80);
        sendData(0x01); // Gate 384
        sendData(0x80);
        sendCommand(0X82); // VDCS setting
        sendData(0x1E);    // Decide by LUT file
        sendCommand(0xe5); // Flash mode
        sendData(0x03);
        sendCommand(0x04); // Power on

        // Wait until ePaper is ready (power on is complete)
        if (!waitForEpd(BUSY_TIMEOUT_MS))
            return false;

        return true;
    }
    else
    {
        // _state is true? Put the panel to sleep.

        sendCommand(0X65); // Flash control
        sendData(0x01);
        sendCommand(0xB9);
        sendCommand(0X65); // Flash control
        sendData(0x00);
        sendCommand(0x02); // Power off

        // Wait until the EPD is ready  (power off is complete)
        waitForEpd(BUSY_TIMEOUT_MS);

        // Disable SPI
        epdSPI.end();

        // To reduce power consumption, set SPI pins as inputs
        pinMode(EPAPER_RST_PIN, INPUT);
        pinMode(EPAPER_DC_PIN, INPUT);
        pinMode(EPAPER_CS_PIN, INPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT);
        pinMode(EPAPER_CLK, INPUT);
        pinMode(EPAPER_DIN, INPUT);

        return true;
    }
}

/**
 * @brief       resetPanel resets Inkplate 7
 */
void Inkplate::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(15); // Wait at least 10ms
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(15);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 7
 *
 * @param       uint8_t _command
 *              predefined command for ePaper control
 */
void Inkplate::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, LOW);
    delayMicroseconds(10);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes((const uint8_t *)(&_command), 1);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}


/**
 * @brief       sendData sends SPI data to Inkplate 7
 *
 * @param       uint8_t *_data
 *              pointer to data buffer to be sent to ePaper
 *
 * @param       int _n
 *              number of data bytes
 */
void Inkplate::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes((const uint8_t *)_data, _n);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 7
 *
 * @param       uint8_t _data
 *              data to be sent to ePaper
 */
void Inkplate::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    epdSPI.beginTransaction(epdSpiSettings);
    epdSPI.writeBytes((const uint8_t *)(&_data), 1);
    epdSPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       Waits for panel to be ready for data
 *
 * @param       uint8_t _timeout
 *              How long to wait
 *
 * @return      bool is panel ready or has it timed out
 */
bool Inkplate::waitForEpd(uint16_t _timeout)
{
    unsigned long _time = millis();
    while (!digitalRead(EPAPER_BUSY_PIN) && ((millis() - _time) < _timeout))
    {
        delay(1);
    }
    if (!digitalRead(EPAPER_BUSY_PIN))
        return false;
    delay(200);
    return true;
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
 *              pixel color, can be WHITE (0), BLACK (1) or RED (2)
 *
 * @note        If x0 or y0 are out of inkplate screen borders, function will
 * exit.
 */

void Graphics::writePixel(int16_t x0, int16_t y0, uint16_t _color)
{
    if (x0 > width() - 1 || y0 > height() - 1 || x0 < 0 || y0 < 0)
        return;

    switch (rotation)
    {
    case 0:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 1:
        y0 = height() - y0 - 1;
        _swap_int16_t(x0, y0);
        break;
    case 2:
        break;
    case 3:
        x0 = width() - x0 - 1;
        _swap_int16_t(x0, y0);
        break;
    }

    int _x = x0 / 2;
    int _x_sub = x0 % 2;

    // Clear the old pixel in the buffer
    *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x) &= pixelMaskGLUT[_x_sub];

    // Write the new pixel
    *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x) |= (LUT_IP7[_color] << ((1 - _x_sub) * 4));
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

    // Battery voltage Switch MOSFET
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B1, OUTPUT);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B1, LOW);

    // Rest of pins go to low because in deepSleep mode they are
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
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B3, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B4, OUTPUT);
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
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B3, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B4, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B5, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B6, LOW);
    digitalWriteInternal(IO_INT_ADDR, ioRegsInt, IO_PIN_B7, LOW);
}

#endif
