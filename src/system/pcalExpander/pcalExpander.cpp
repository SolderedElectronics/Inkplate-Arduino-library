/**
 **************************************************
 * @file        Pcal.cpp
 * @brief       class for controling pcal expander
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
 * @authors     Soldered
 ***************************************************/
#if !defined(ARDUINO_INKPLATE6) && !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE6PLUS)
#include "pcalExpander.h"

/**
 * @brief       ioBegin function starts pcal expander and sets registers values
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 *
 * @return      true if successful, false otherwise
 */
bool IOExpander::begin(uint8_t _addr)
{
    _ioExpanderI2CAddress = _addr;

    Wire.beginTransmission(_ioExpanderI2CAddress);
    int error = Wire.endTransmission();
    return error == 0;
}

/**
 * @brief       readPCALRegister function uses I2C to read one selected pcal register
 *
 * @param       uint8_t _regAddr
 *              I2C register address to read from
 *
 * @return      value read from the register
 */
uint8_t IOExpander::readPCALRegister(uint8_t _regAddr)
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(_regAddr);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, (uint8_t)1);
    return Wire.read();
}

/**
 * @brief       updatePCALRegister function uses I2C to update selected pcal register
 *
 * @param       uint8_t _regAddr
 *              I2C register address to write to
 * @param       uint8_t _d
 *              data to be written
 */
void IOExpander::updatePCALRegister(uint8_t _regAddr, uint8_t _d)
{
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(_regAddr);
    Wire.write(_d);
    Wire.endTransmission();
}

/**
 * @brief       pinModeIO function sets internal pin mode
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 */
void IOExpander::pinMode(uint8_t _pin, uint8_t _mode, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    switch (_mode)
    {
    case INPUT:
        updatePCALRegister(PCAL6416A_CFGPORT0 + _port, readPCALRegister(PCAL6416A_CFGPORT0 + _port) | (1 << _pin));
        break;
    case OUTPUT:
        // There is a one cacth! Pins are by default (POR) set as HIGH. So first change it to LOW and then set is as
        // output).
        updatePCALRegister(PCAL6416A_OUTPORT0 + _port, readPCALRegister(PCAL6416A_OUTPORT0 + _port) & ~(1 << _pin));
        updatePCALRegister(PCAL6416A_CFGPORT0 + _port, readPCALRegister(PCAL6416A_CFGPORT0 + _port) & ~(1 << _pin));
        break;
    case INPUT_PULLUP:
        updatePCALRegister(PCAL6416A_CFGPORT0 + _port, readPCALRegister(PCAL6416A_CFGPORT0 + _port) | (1 << _pin));
        updatePCALRegister(PCAL6416A_PUPDEN_REG0 + _port,
                           readPCALRegister(PCAL6416A_PUPDEN_REG0 + _port) | (1 << _pin));
        updatePCALRegister(PCAL6416A_PUPDSEL_REG0 + _port,
                           readPCALRegister(PCAL6416A_PUPDSEL_REG0 + _port) | (1 << _pin));
        break;
    case INPUT_PULLDOWN:
        updatePCALRegister(PCAL6416A_CFGPORT0 + _port, readPCALRegister(PCAL6416A_CFGPORT0 + _port) | (1 << _pin));
        updatePCALRegister(PCAL6416A_PUPDEN_REG0 + _port,
                           readPCALRegister(PCAL6416A_PUPDEN_REG0 + _port) | (1 << _pin));
        updatePCALRegister(PCAL6416A_PUPDSEL_REG0 + _port,
                           readPCALRegister(PCAL6416A_PUPDSEL_REG0 + _port) & ~(1 << _pin));
        break;
    }
}

/**
 * @brief       digitalWriteIO sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 *
 */
void IOExpander::digitalWrite(uint8_t _pin, uint8_t _state, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    if (_pin > 15)
        return;
    _state &= 1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    uint8_t reg = readPCALRegister(PCAL6416A_OUTPORT0 + _port);
    _state ? reg |= (1 << _pin) : reg &= ~(1 << _pin);
    updatePCALRegister(PCAL6416A_OUTPORT0 + _port, reg);
}

/**
 * @brief       digitalReadPCAL reads pcal internal pin state
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t IOExpander::digitalRead(uint8_t _pin, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return 0;

    if (_pin > 15)
        return -1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    return (readPCALRegister(PCAL6416A_INPORT0 + _port) >> _pin) & 1;
}

/**
 * @brief       setIntPin function enables interrupt on change on IO Expander pin.
 *
 * @param       uint8_t _pin
 *              pin to set interrupt mode to
 */
void IOExpander::setIntPin(uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    updatePCALRegister(PCAL6416A_INTMSK_REG0 + _port, readPCALRegister(PCAL6416A_INTMSK_REG0 + _port) & ~(1 << _pin));
}

/**
 * @brief       getInt function reads did interrupt on change event has occur.
 *
 * @return      returns interupt registers state
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t IOExpander::getInt()
{
    return ((uint16_t)readPCALRegister(PCAL6416A_INTSTAT_REG1) << 8) | readPCALRegister(PCAL6416A_INTSTAT_REG0);
}

/**
 * @brief       removeIntPin function removes Interrupt from pin
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 */
void IOExpander::removeIntPin(uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    updatePCALRegister(PCAL6416A_INTMSK_REG0 + _port, readPCALRegister(PCAL6416A_INTMSK_REG0 + _port) | (1 << _pin));
}

/**
 * @brief       setPorts sets states on every IO Expander pin at once.
 *
 * @param       uint16_t _d
 *              GPIO pin state of all IO Expander pins.
 *
 */
void IOExpander::setPorts(uint16_t _d)
{
    updatePCALRegister(PCAL6416A_OUTPORT0, _d & 0xff);
    updatePCALRegister(PCAL6416A_OUTPORT1, (_d >> 8) & 0xff);
}

/**
 * @brief       getPorts reads GPIO pin state on every IO Expander pin at once.
 *
 */
uint16_t IOExpander::getPorts()
{
    return readPCALRegister(PCAL6416A_INPORT0) | ((uint16_t)readPCALRegister(PCAL6416A_INPORT1) << 8);
}

/**
 * @brief   Sets block on specific pin so user could not use it. This is to prevent damage to the
 *          board since some of the GPIO pins from the I/O expander are connected to something on the board
 *          (for example TPS).
 *
 * @param   uint8_t _pin
 *          I/O exapnder GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 -
 *          IO_PIN_B7).
 */
void IOExpander::blockPinUsage(uint8_t _pin)
{
    // Only pins from 0 to 15 are allowed.
    _pin &= 15;

    // Update the table.
    _blockedPinsForUser |= 1ULL << _pin;
}

/**
 * @brief   Remove block on specific pin so user could use it.
 *
 * @param   uint8_t _pin
 *          I/O exapnder GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 -
 *          IO_PIN_B7).
 */
void IOExpander::unblockPinUsage(uint8_t _pin)
{
    // Only pins from 0 to 15 are allowed.
    _pin &= 15;

    // Update the table.
    _blockedPinsForUser &= ~(1ULL << _pin);
}

/**
 * @brief   Checks for the pins taht users are not allowed to use.
 *
 * @param   uint8_t _pin
 *          GPIO pin on the I/O expander
 * @return  bool
 *          true - Pin in blocked for the user - user must not use this pin (it's internally
 *          connected to something on the board, check the schematic!
 * @return  false - User can use this pin.
 */
bool IOExpander::checkForBlockedPins(uint8_t _pin)
{
    // Check if the pin usage for this specific pin is blocked for user comparing it with the internal table.
    if (_blockedPinsForUser & (1ULL << _pin))
        return true;

    // Pin is not blocked, user can use it.
    return false;
}
#endif
