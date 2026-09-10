/**
 **************************************************
 *
 * @file        Inkplate2Driver.cpp
 * @brief       Low level driver for the Inkplate 2 e-paper panel
 *
 *              The Inkplate 2 uses a 2.13" 212x104 three colour (black, white and red)
 *              e-paper panel driven over SPI. Unlike the larger Inkplate boards, this one has
 *              no GPIO expander and no PMIC - the panel controller generates its own driving
 *              voltages, so the driver only handles reset, the busy line and the SPI traffic.
 *
 *              The frame buffer is split into a black and a red plane, and both planes are
 *              sent to the panel on every display() call. Only full updates are supported.
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE2
#include "Inkplate2Driver.h"
#include "Inkplate.h"
#include "../../system/inkplateSemaphore.h"

SPIClass epdSPI(VSPI);

SPISettings epdSpiSettings(1000000UL, MSBFIRST, SPI_MODE0);

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
void EPDDriver::writePixelInternal(int16_t x0, int16_t y0, uint16_t color)
{
    if (x0 > _inkplate->width() - 1 || y0 > _inkplate->height() - 1 || x0 < 0 || y0 < 0)
        return;
    if (color > 2)
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
    if (color < 2)
    {
        *(DMemory4Bit + _position) &= ~(color << (7 - _xSub));
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
    if (!_beginDone)
    {

        // Allocate memory for frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4);

        _inkplate = _inkplatePtr;

        image.begin(_inkplatePtr);

        pinMode(5, OUTPUT);
        pinMode(18, OUTPUT);
        pinMode(19, OUTPUT);
        pinMode(23, OUTPUT);
        pinMode(33, OUTPUT);
        pinMode(32, OUTPUT);

        digitalWrite(5, LOW);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(23, LOW);
        digitalWrite(33, LOW);
        digitalWrite(32, LOW);

        delay(2000);

        if (DMemory4Bit == NULL)
        {
            return false;
        }

        // Clear frame buffer
        clearDisplay();

        // Set default rotation
        _inkplate->setRotation(1);

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
 * @brief       clearDisplay function clears memory buffer for display
 *
 * @note        This does not clear the actual display, only the memory buffer, you need to call
 * display() function after this to clear the display
 */
void EPDDriver::clearDisplay()
{
    memset(DMemory4Bit, 0xFF, E_INK_WIDTH * E_INK_HEIGHT / 4);
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
    // Wake the panel and wait a bit
    // The refresh time is long anyway so this delay doesn't make much impact
    setPanelDeepSleep(false);
    delay(20);

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

    // Go back to sleep
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
 * @brief       resetPanel resets Inkplate 6COLOR
 */
void EPDDriver::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(100);
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(100);
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

        // Set SPI pins
        epdSPI.begin(EPAPER_CLK, -1, EPAPER_DIN, -1);

        // Set up EPD communication pins
        pinMode(EPAPER_CS_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_BUSY_PIN, INPUT_PULLUP);

        delay(10);

        // Reinit the panel
        // Reset EPD IC
        resetPanel();

        sendCommand(0x04);
        if (!waitForEpd(BUSY_TIMEOUT_MS))
            return false; // Waiting for the electronic paper IC to release the idle signal

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
        epdSPI.end();

        // To reduce power consumption, set SPI pins as outputs
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
 * @brief       Waits for panel to be ready for data
 *
 * @param       uint8_t _timeout
 *              Timeout for wait
 *
 * @return      bool is panel ready or timed out
 */
bool EPDDriver::waitForEpd(uint16_t _timeout)
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