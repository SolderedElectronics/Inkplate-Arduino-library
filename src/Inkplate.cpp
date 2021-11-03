/**
 **************************************************
 *
 * @file        Inkplate.cpp
 * @brief       Basic funtions for controling inkplate
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
 * @authors     @ e-radionica.com
 ***************************************************/

#include "Inkplate.h"

#ifdef ARDUINO_INKPLATECOLOR
Inkplate::Inkplate() : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
#else

Inkplate::Inkplate(uint8_t _mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
#endif
{
#ifndef ARDUINO_INKPLATECOLOR
    setDisplayMode(_mode);
    for (uint32_t i = 0; i < 256; ++i)
        pinLUT[i] = ((i & B00000011) << 4) | (((i & B00001100) >> 2) << 18) | (((i & B00010000) >> 4) << 23) |
                    (((i & B11100000) >> 5) << 25);
#endif

#ifdef ARDUINO_ESP32_DEV
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, 9, HIGH);
#else
    digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, 9, LOW);
#endif
}

/**
 * @brief       clearDisplay function clears memory buffer for display
 *
 * @note        This does not clears display, only buffer, you need to call
 * display() function after this to clear display
 */
void Inkplate::clearDisplay()
{
#ifdef ARDUINO_INKPLATECOLOR
    memset(DMemory4Bit, WHITE << 4 | WHITE, E_INK_WIDTH * E_INK_HEIGHT / 2);
#else
    // Clear 1 bit per pixel display buffer
    if (getDisplayMode() == 0)
        memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Clear 3 bit per pixel display buffer
    if (getDisplayMode() == 1)
        memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);
#endif
}

#ifndef ARDUINO_INKPLATECOLOR

/**
 * @brief       display function update display with new data from buffer
 */
void Inkplate::display(bool leaveOn)
{
    if (getDisplayMode() == 0)
        display1b(leaveOn);
    else if (getDisplayMode() == 1)
        display3b(leaveOn);
}

/**
 * @brief       preloadScreen function copy data from partial to data buffer
 */
void Inkplate::preloadScreen()
{
    memcpy(DMemoryNew, _partial, 60000);
}

/**
 * @brief       einkOff turns off epaper power supply and put all digital IO
 * pins in high Z state
 */
void Inkplate::einkOff()
{
    if (getPanelState() == 0)
        return;
    OE_CLEAR;
    GMOD_CLEAR;
    GPIO.out &= ~(DATA | LE | CL);
    CKV_CLEAR;
    SPH_CLEAR;
    SPV_CLEAR;

    // Put TPS65186 into standby mode (leaving 3V3 SW active)
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(0x6f);
    Wire.endTransmission();

    // Wait for all PWR rails to shut down
    delay(100);

    // Disable 3V3 to the panel
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(0x4f);
    Wire.endTransmission();

    pinsZstate();
    setPanelState(0);
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
int Inkplate::einkOn()
{
    if (getPanelState() == 1)
        return 1;
    WAKEUP_SET;
    delay(2);
    PWRUP_SET;

    // Enable all rails
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(B00111111);
    Wire.endTransmission();
    pinsAsOutputs();
    LE_CLEAR;
    OE_CLEAR;
    CL_CLEAR;
    SPH_SET;
    GMOD_SET;
    SPV_SET;
    CKV_CLEAR;
    OE_CLEAR;
    VCOM_SET;

    unsigned long timer = millis();
    do
    {
        delay(1);
    } while ((readPowerGood() != PWR_GOOD_OK) && (millis() - timer) < 250);
    if ((millis() - timer) >= 250)
    {
        WAKEUP_CLEAR;
        VCOM_CLEAR;
        PWRUP_CLEAR;
        return 0;
    }

    OE_SET;
    setPanelState(1);

    return 1;
}

/**
 * @brief       readPowerGood reads ok status for each rail
 *
 * @return      power good status register
 */
uint8_t Inkplate::readPowerGood()
{
    Wire.beginTransmission(0x48);
    Wire.write(0x0F);
    Wire.endTransmission();

    Wire.requestFrom(0x48, 1);
    return Wire.read();
}

// LOW LEVEL FUNCTIONS

/**
 * @brief       vscan_start starts writing new frame and skips first two lines
 * that are invisible on screen
 */
void Inkplate::vscan_start()
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
 * @brief       hscan_start starts writing data into current row
 *
 * @param       uint32_t _d
 *              data to be written into current row
 */
void Inkplate::hscan_start(uint32_t _d)
{
    SPH_CLEAR;
    GPIO.out_w1ts = (_d) | CL;
    GPIO.out_w1tc = DATA | CL;
    SPH_SET;
    CKV_SET;
}

/**
 * @brief       vscan_end ends current row and prints data to screen
 */
void Inkplate::vscan_end()
{
    CKV_CLEAR;
    LE_SET;
    LE_CLEAR;
    delayMicroseconds(0);
}

/**
 * @brief       pinsZstate sets all tps pins at high z state
 *
 * @note        this is used only when turning off epaper
 */
void Inkplate::pinsZstate()
{
    pinMode(0, INPUT);
    pinMode(2, INPUT);
    pinMode(32, INPUT);
    pinMode(33, INPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, INPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, INPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, INPUT);

    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(18, INPUT);
    pinMode(19, INPUT);
    pinMode(23, INPUT);
    pinMode(25, INPUT);
    pinMode(26, INPUT);
    pinMode(27, INPUT);
}

/**
 * @brief       pinsAsOutputs sets all tps pins as outputs
 */
void Inkplate::pinsAsOutputs()
{
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, OUTPUT);
    pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, OUTPUT);

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
}

#endif