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
 * @authors     @ Soldered
 ***************************************************/

#include "Inkplate.h"

#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4)
Inkplate::Inkplate() : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
#else

Inkplate::Inkplate(uint8_t _mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
#endif
{
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE4)
    setDisplayMode(_mode);
#endif
}

/**
 * @brief       clearDisplay function clears memory buffer for display
 *
 * @note        This does not clear the actual display, only the memory buffer, you need to call
 * display() function after this to clear the display
 */
void Inkplate::clearDisplay()
{
#if defined(ARDUINO_INKPLATECOLOR)
    memset(DMemory4Bit, WHITE << 4 | WHITE, E_INK_WIDTH * E_INK_HEIGHT / 2);
#elif defined(ARDUINO_INKPLATE2)
    memset(DMemory4Bit, 0xFF, E_INK_WIDTH * E_INK_HEIGHT / 4);
#elif defined(ARDUINO_INKPLATE4)
    memset(DMemory4Bit, 0xFF, (E_INK_WIDTH * E_INK_HEIGHT / 8));
    memset(DMemory4Bit + (E_INK_WIDTH * E_INK_HEIGHT / 8), 0x00, (E_INK_WIDTH * E_INK_HEIGHT / 8));
#else
    // Clear 1 bit per pixel display buffer
    if (getDisplayMode() == 0)
        memset(_partial, 0, E_INK_WIDTH * E_INK_HEIGHT / 8);

    // Clear 3 bit per pixel display buffer
    if (getDisplayMode() == 1)
        memset(DMemory4Bit, 255, E_INK_WIDTH * E_INK_HEIGHT / 2);
#endif
}

#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE4)

/**
 * @brief       display function update display with new data from buffer
 *
 * @param       bool leaveOn
 *              if set to 1, it will disable turning supply for eink after
 *              display update in order to save some time needed for power supply
 *              to save some time at next display update or increase refreshing speed
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
    delay(5);

    // Modify power up sequence  (VEE and VNEG are swapped)
    Wire.beginTransmission(0x48);
    Wire.write(0x09);
    Wire.write(B11100001);
    Wire.endTransmission();

    // Enable all rails
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(B00111111);
    Wire.endTransmission();

    PWRUP_SET;

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
        VCOM_CLEAR;
        PWRUP_CLEAR;
        return 0;
    }

    OE_SET;
    setPanelState(1);

    return 1;
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

    VCOM_CLEAR;
    PWRUP_CLEAR;

    unsigned long timer = millis();
    do
    {
        delay(1);
    } while ((readPowerGood() != 0) && (millis() - timer) < 250);

    // Do not disable WAKEUP if older Inkplate6Plus is used.
#ifndef ARDUINO_INKPLATE6PLUS
    WAKEUP_CLEAR;
#endif

    setPanelState(0);
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
    pinModeInternal(IO_INT_ADDR, ioRegsInt, OE, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, GMOD, INPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SPV, INPUT);

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
    pinModeInternal(IO_INT_ADDR, ioRegsInt, OE, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, GMOD, OUTPUT);
    pinModeInternal(IO_INT_ADDR, ioRegsInt, SPV, OUTPUT);

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