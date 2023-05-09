/**
 **************************************************
 *
 * @file        Inkplate4.cpp
 * @brief       Basic funtions for controling Inkplate 4
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

#ifdef ARDUINO_INKPLATE4

SPISettings epdSpiSettings(4000000UL, MSBFIRST, SPI_MODE0);
SPIClass SPI2(HSPI);

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
        // Begin SPI
        SPI2.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);

        // Set up EPD communication pins
        pinMode(EPAPER_CS_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT);

        delay(10);

        // Allocate memory for frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);

        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Init TwoWire and GPIO expander
        setIOExpanderForLowPower();

        // Set the default text color
        setTextColor(INKPLATE_BLACK);

        // Clear frame buffer
        clearDisplay();

        _beginDone = 1;
    }

    // Reset EPD IC
    resetPanel();

    // SW Reset
    sendCommand(0x12);
    if (!waitForEpd(BUSY_TIMEOUT_MS))
        return 0;

    sendCommand(0x74);
    sendData(0x54);
    sendCommand(0x7E);
    sendData(0x3B);

    // Reduce glitch under AVCOM
    sendCommand(0x2B);
    sendData(0x04);
    sendData(0x63);

    // Soft start setting
    sendCommand(0x0C);
    sendData(0x8B);
    sendData(0x9C);
    sendData(0x96);
    sendData(0x0F);

    // Set MUX as 300
    sendCommand(0x01);
    sendData(0x2B);
    sendData(0x01);
    sendData(0x00);

    // Data entry mode
    sendCommand(0x11);
    sendData(0x01);

    // RAM x address start at 0
    // RAM x address end at 31h(49+1)*8->400
    sendCommand(0x44);
    sendData(0x00);
    sendData(0x31);

    // RAM y address start at 12Bh
    // RAM y address end at 00h
    sendCommand(0x45);
    sendData(0x2B);
    sendData(0x01);
    sendData(0x00);
    sendData(0x00);

    sendCommand(0x3C);
    sendData(0x01);

    sendCommand(0x18);
    sendData(0X80);
    sendCommand(0x22);
    sendData(0XB1);

    // Load Temperature and waveform setting
    sendCommand(0x20);

    if (!waitForEpd(BUSY_TIMEOUT_MS))
        return 0; // Waiting for the electronic paper IC to release the idle signal

    _panelState = true;
    return true;
}


/**
 * @brief       Display function that updates display with new data from buffer
 *
 */
void Inkplate::display() // Leave on does nothing
{
    // Write RAM for Black/White pixels
    sendCommand(0x24);
    sendData(DMemory4Bit, (E_INK_WIDTH * E_INK_HEIGHT / 8));

    // Write RAM for Red pixels
    sendCommand(0x26);
    sendData(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8), (E_INK_WIDTH * E_INK_HEIGHT / 8));

    // EPD update
    sendCommand(0x22); // Display Update Control
    sendData(0xF7);
    sendCommand(0x20); // Activate Display Update Sequence

    delayMicroseconds(500); // Wait at least 200 uS
    waitForEpd(24000);
}

/**
 * @brief       setPanelDeepSleep puts color epaper in deep sleep, or starts
 * epaper, depending on given arguments.
 *
 * @param       bool _state
 *              -'True' sets the panel to sleep
 *              -'False' wakes the panel
 *
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

        _panelState = false;
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
 * @brief       resetPanel resets Inkplate 4
 */
void Inkplate::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(15); // Wait at least 10ms
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(15);
}

/**
 * @brief       sendCommand sends SPI command to Inkplate 4
 *
 * @param       uint8_t _command
 *              predefined command for ePaper control
 */
void Inkplate::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, LOW);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.writeBytes((const uint8_t *)(&_command), 1);
    SPI2.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}


/**
 * @brief       sendData sends SPI data to Inkplate 4
 *
 * @param       uint8_t *_data
 *              pointer to data buffer to be sent to ePaper
 * @param       int _n
 *              number of data bytes
 */
void Inkplate::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.writeBytes((const uint8_t *)_data, _n);
    SPI2.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
    delay(1);
}

/**
 * @brief       sendData sends SPI data to Inkplate 4
 *
 * @param       uint8_t _data
 *              data to be sent to ePaper
 */
void Inkplate::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_CS_PIN, LOW);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    delayMicroseconds(10);
    SPI2.beginTransaction(epdSpiSettings);
    SPI2.writeBytes((const uint8_t *)(&_data), 1);
    SPI2.endTransaction();
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
    while (digitalRead(EPAPER_BUSY_PIN) && ((millis() - _time) < _timeout))
    {
        delay(1);
    }
    if (digitalRead(EPAPER_BUSY_PIN))
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

    if (_color > 2)
        return;

    // Fix for ePaper buffer
    y0 += 1;
    if (y0 == 300)
        y0 = 0;

    switch (rotation)
    {
    case 0:
        x0 = width() - 1 - x0;
        break;
    case 1:
        _swap_int16_t(x0, y0);
        break;
    case 2:
        y0 = height() - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        x0 = height() - x0 - 1;
        break;
    }

    int _x = x0 / 8;
    int _x_sub = x0 % 8;

    // To optimize writing pixels into EPD, framebuffer is split in half, where first half is for B&W pixels and other
    // half is for red pixels only
    if (_color < 2)
    {
        // Draw the Black/White pixel and clear the red pixel
        *(DMemory4Bit + E_INK_WIDTH / 8 * y0 + _x) |= (pixelMaskLUT[7 - _x_sub]);
        *(DMemory4Bit + E_INK_WIDTH / 8 * y0 + _x) &= ~(_color << (7 - _x_sub));
        *(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8) + E_INK_WIDTH / 8 * y0 + _x) &= ~(pixelMaskLUT[7 - _x_sub]);
    }
    else
    {
        // Draw a red pixel in the set coordinates
        *(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8) + E_INK_WIDTH / 8 * y0 + _x) |= (pixelMaskLUT[7 - _x_sub]);
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

#endif
