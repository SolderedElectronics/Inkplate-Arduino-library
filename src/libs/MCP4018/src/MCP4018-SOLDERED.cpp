/**
 **************************************************
 *
 * @file        MCP4018-SOLDERED.cpp
 * @brief       Source file for MCP4018 library.
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Borna Biro @ soldered.com
 ***************************************************/


#include "MCP4018-SOLDERED.h"

/**
 * @brief                   MCP4018 constructor.
 *
 */
MCP4018_SOLDERED::MCP4018_SOLDERED()
{
    native = 1;
}

/**
 * @brief                   Overloaded function for virtual in base class to initialize sensor specific.
 */
void MCP4018_SOLDERED::initializeNative()
{
    address = MCP4018_I2C_ADDRESS;
    Wire.begin();
    getWiperValue();
}

/**
 * @brief                   Set wiper new position in percents.
 *
 * @param int _wiper        Wiper position in percents (0 - 100 percent) from GND potentiometer pin.
 */
void MCP4018_SOLDERED::setWiperPercent(int _wiper)
{
    if ((_wiper > 100) || (_wiper < 0))
        return;

    setWiperValue(round(_wiper / 100.0 * 127.0));
}

/**
 * @brief                  Set wiper new position with digital word.
 *
 * @param int _byte        Wiper position with digital word (0 - 127) from GND potentiometer pin.
 */
void MCP4018_SOLDERED::setWiperValue(int _byte)
{
    _value = _byte & 0x7F;
    const uint8_t _reg = _value;
    sendData(&_reg, 1);
}

/**
 * @brief                   Read current wiper position in percents.
 *
 * @return                  Wiper position in percents (0 - 100) with respect to GND potentiometer pin.
 */
int MCP4018_SOLDERED::getWiperPercent()
{
    return (int)round((getWiperValue() / 127.0 * 100.0));
}


/**
 * @brief                   Read current wiper position in form of digital word.
 *
 * @return                  Wiper position in digital word (0 - 127) with respect to GND potentiometer pin.
 */
uint8_t MCP4018_SOLDERED::getWiperValue()
{
    char _reg;
    readData(&_reg, 1);
    _value = _reg & 0x7F;
    return _value;
}

/**
 * @brief                   Increment current wiper position by one digital word (not percent!).
 *
 */
void MCP4018_SOLDERED::increment()
{
    if (_value < 127)
    {
        _value++;
        setWiperValue(_value);
    }
}

/**
 * @brief                   Decrement current wiper position by one digital word (not percent!).
 *
 */
void MCP4018_SOLDERED::decrement()
{
    if (_value > 0)
    {
        _value--;
        setWiperValue(_value);
    }
}