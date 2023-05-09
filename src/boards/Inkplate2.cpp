/**
 **************************************************
 *
 * @file        Inkplate2.cpp
 * @brief       Basic funtions for controling inkplate 2
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

#ifdef ARDUINO_INKPLATE2

SPIClass SPI2(HSPI);

SPISettings epdSpiSettings(1000000UL, MSBFIRST, SPI_MODE0);

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
    if (_color > 2)
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

    // Find the specific byte in the frame buffer that needs to be modified.
    // Also find the bit in the byte that needs modification.
    int _x = x0 / 8;
    int _xSub = x0 % 8;

    int _position = E_INK_WIDTH / 8 * y0 + _x;

    // Clear both black and red frame buffer.
    *(DMemory4Bit + _position) |= (pixelMaskLUT[7 - _xSub]);
    *(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8) + _position) |= (pixelMaskLUT[7 - _xSub]);

    // To optimize writing pixels into EPD, framebuffer is split in half, where first half is for B&W pixels and other
    // half is for red pixels only
    if (_color < 2)
    {
        *(DMemory4Bit + _position) &= ~(_color << (7 - _xSub));
    }
    else
    {
        *(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8) + _position) &= ~(pixelMaskLUT[7 - _xSub]);
    }
}

/**
 * @brief       begin function initialize Inkplate object with predefined
 * settings
 *
 * @return      True if initialization is successful, false if failed or already
 * initialized
 */
bool Inkplate::begin()
{
    if (!_beginDone)
    {
        // Set SPI pins
        SPI2.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);

        // Set up EPD communication pins
        pinMode(EPAPER_CS_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT_PULLUP);

        delay(10);

        // Allocate memory for frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);

        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Clear frame buffer
        clearDisplay();

        // Set default rotation
        setRotation(1);

        _beginDone = 1;
    }

    // Reset EPD IC
    resetPanel();

    sendCommand(0x04);
    if (!waitForEpd(BUSY_TIMEOUT_MS))
        return 0; // Waiting for the electronic paper IC to release the idle signal

    sendCommand(0x00); // Enter panel setting
    sendData(0x0f);    // LUT from OTP 128x296
    sendData(0x89);    // Temperature sensor, boost and other related timing settings

    sendCommand(0x61); // Enter panel resolution setting
    sendData(E_INK_WIDTH);
    sendData(E_INK_HEIGHT >> 8);
    sendData(E_INK_HEIGHT & 0xff);

    sendCommand(0x50); // VCOM and data interval setting
    sendData(0x77);    // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    return true;
}

/**
 * @brief       Display function that updates display with new data from buffer
 *
 */
void Inkplate::display()
{
    // First write B&W pixels to epaper
    sendCommand(0x10);
    sendData(DMemory4Bit, (E_INK_WIDTH * E_INK_HEIGHT / 8));

    // Now write red pixels to epaper
    sendCommand(0x13);
    sendData(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8), (E_INK_WIDTH * E_INK_HEIGHT / 8));

    // Stop data transfer
    sendCommand(0x11);
    sendData(0x00);

    // Send display refresh command
    sendCommand(0x12);
    delayMicroseconds(500); // Wait at least 200 uS
    waitForEpd(60000);
}

/**
 * @brief       setPanelDeepSleep puts color epaper in deep sleep, or starts
 * epaper, depending on given arguments.
 *
 * @param       bool _state
 *              -'True' sets the panel to sleep
 *              -'False' wakes the panel
 */
void Inkplate::setPanelDeepSleep(bool _state)
{
    if (!_state)
    {
        // _state is false? Wake the panel!

        // Set SPI pins
        SPI2.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);

        // Set up EPD communication pins
        pinMode(EPAPER_CS_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT_PULLUP);

        delay(10);

        // Send commands to power up panel. According to the datasheet, it can be
        // powered up from deep sleep only by reseting it and doing reinit.
        begin();
    }
    else
    {
        // _state is true? Put the panel to sleep.

        sendCommand(0X50); // VCOM and data interval setting
        sendData(0xf7);

        sendCommand(0X02); // Power  EPD off
        waitForEpd(BUSY_TIMEOUT_MS);
        sendCommand(0X07); // Put EPD in deep sleep
        sendData(0xA5);
        delay(1);

        // Disable SPI
        SPI2.end();

        // To reduce power consumption, set SPI pins as outputs
        pinMode(EPAPER_RST_PIN, INPUT);
        pinMode(EPAPER_DC_PIN, INPUT);
        pinMode(EPAPER_CS_PIN, INPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT);
        pinMode(EPAPER_CLK, INPUT);
        pinMode(EPAPER_DIN, INPUT);
    }
}

/**
 * @brief       getPanelDeepSleepState returns current state of the panel
 *
 * @return      bool _panelState
 */
bool Inkplate::getPanelDeepSleepState()
{
    return _panelState;
}

/**
 * @brief       resetPanel resets Inkplate 2
 */
void Inkplate::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(100); // Wait at least 10ms
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(100);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 2
 *
 * @param       uint8_t _command
 *              predefined command for epaper control
 */
void Inkplate::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, LOW);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.transfer(_command);
    SPI2.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 2
 *
 * @param       uint8_t *_data
 *              pointer to data buffer to be sent to epaper
 * @param       int _n
 *              number of data bytes
 */
void Inkplate::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.writeBytes(_data, _n);
    SPI2.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 2
 *
 * @param       uint8_t _data
 *              data to be sent to epaper
 */
void Inkplate::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.transfer(_data);
    SPI2.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       Waits for panel to be ready for data
 *
 * @param       uint8_t _timeout
 *              Timeout for wait
 *
 * @return      bool is panel ready or timed out
 */
bool Inkplate::waitForEpd(uint16_t _timeout)
{
    unsigned long _time = millis();
    while (!digitalRead(EPAPER_BUSY_PIN) && ((millis() - _time) < _timeout))
        ;
    if (!digitalRead(EPAPER_BUSY_PIN))
        return false;
    delay(200);
    return true;
}

#endif
