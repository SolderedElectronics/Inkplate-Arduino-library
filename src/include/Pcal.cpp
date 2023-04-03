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

#if defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE6PLUSV2) ||               \
    defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATECOOL) || defined(ARDUINO_INKPLATE5) ||                   \
    defined(ARDUINO_INKPLATE4)

#include "Pcal.h"

// LOW LEVEL:

/**
 * @brief       ioBegin function starts pcal expander and sets registers values
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array to be writen in registers
 *
 * @return      true if successful, false otherwise
 */
bool Expander::ioBegin(uint8_t _addr, uint8_t *_r)
{
    Wire.beginTransmission(_addr);
    int error = Wire.endTransmission();
    if (error)
        return false;
    readPCALRegisters(_addr, _r);
    return true;
}

/**
 * @brief       ioBegin function returns the pointer to the array of PCAL6416 copy of internal registers that depends on
 * the I2C address of the IO Expander.
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 *
 * @return      pointer to the PCAL6416 register array
 */
uint8_t *Expander::getInternalRegisterArray(uint8_t _addr)
{
    return (_addr == IO_INT_ADDR ? ioRegsInt : ioRegsEx);
}

/**
 * @brief       readPCALRegisters function uses i2c to read all pcal registers
 *
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 */
void Expander::readPCALRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)23);
    for (int i = 0; i < 23; i++)
    {
        k[i] = Wire.read();
    }
}

/**
 * @brief       readPCALRegisters function uses I2C to read selected pcal
 * registers
 *
 * @param       uint8_t _regIndex
 *              Start index of the PCAL6416 registers
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 * @param       uint8_t _n
 *              number of bites/registers to read
 */
void Expander::readPCALRegisters(uint8_t _addr, uint8_t _regIndex, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_addr, _n);
    for (int i = 0; i < _n; i++)
    {
        k[_regIndex + i] = Wire.read();
    }
}

/**
 * @brief       readPCALRegisters function uses I2C to read one selected pcal
 * register
 *
 * @param       uint8_t _regIndex
 *              Start index of the PCAL6416 registers
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 */
void Expander::readPCALRegister(uint8_t _addr, uint8_t _regIndex, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)1);
    *(k) = Wire.read();
}

/**
 * @brief       updatePCALAllRegisters function uses I2C to updates all pcal
 * registers
 *
 * @param       uint8_t *_k
 *              pointer to array where data to be uploaded is stored
 */
void Expander::updatePCALAllRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    for (int i = 0; i < 23; i++)
    {
        Wire.write(k[i]);
    }
    Wire.endTransmission();
}

/**
 * @brief       updatePCALRegister function uses I2C to update selected pcal register
 *
 * @param       uint8_t _regIndex
 *              Start index of the PCAL6416 registers
 * @param       uint8_t _d
 *              data to be uploaded
 */
void Expander::updatePCALRegister(uint8_t _addr, uint8_t _regIndex, uint8_t _d)
{
    Wire.beginTransmission(_addr);
    Wire.write(regAddresses[_regIndex]);
    Wire.write(_d);
    Wire.endTransmission();
}

/**
 * @brief       updatePCALRegister function uses I2C to update some selected pcal
 * registers
 *
 * @param       uint8_t _regIndex
 *              Start index of the PCAL6416 registers
 * @param       uint8_t *_k
 *              pointer to array that holds new data
 * @param       uint8_t _n
 *              number of bites/registers to write to
 */
void Expander::updatePCALRegister(uint8_t _addr, uint8_t _regIndex, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(regAddresses[_regIndex]);
    for (int i = 0; i < _n; ++i)
    {
        Wire.write(k[_regIndex + i]);
    }
    Wire.endTransmission();
}

/**
 *  HIGH LEVEL
 */

/**
 * @brief       pinModeIO function sets internal pin mode
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 */
void Expander::pinModeIO(uint8_t _pin, uint8_t _mode, uint8_t _ioID)
{
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE4)
    if ((_ioID == IO_INT_ADDR) && (_pin < 9))
        return;
#endif
    pinModeInternal(_ioID, getInternalRegisterArray(_ioID), _pin, _mode);
}

/**
 * @brief       digitalWriteIO sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 *
 */
void Expander::digitalWriteIO(uint8_t _pin, uint8_t _state, uint8_t _ioID)
{
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE4)
    if ((_ioID == IO_INT_ADDR) && (_pin < 9))
        return;
#endif
    digitalWriteInternal(_ioID, getInternalRegisterArray(_ioID), _pin, _state);
}

/**
 * @brief       digitalReadPCAL reads pcal internal pin state
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Expander::digitalReadIO(uint8_t _pin, uint8_t _ioID)
{
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE4)
    if ((_ioID == IO_INT_ADDR) && (_pin < 9))
        return 0;
#endif
    return digitalReadInternal(_ioID, getInternalRegisterArray(_ioID), _pin);
}

/**
 * @brief       setIntPin function enables interrupt on change on IO Expander pin.
 *
 * @param       uint8_t _pin
 *              pin to set interrupt mode to
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 */
void Expander::setIntPin(uint8_t _pin, uint8_t _ioID)
{
    setIntPinInternal(_ioID, getInternalRegisterArray(_ioID), _pin);
}

/**
 * @brief       getINTInternal function reads did interrupt on change event has occur.
 *
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 *
 * @return      returns interupt registers state
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t Expander::getINT(uint8_t _ioID)
{
    return getINTInternal(_ioID, getInternalRegisterArray(_ioID));
}

/**
 * @brief       removeIntPin function removes Interrupt from pin
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 */
void Expander::removeIntPin(uint8_t _pin, uint8_t _ioID)
{
    removeIntPinInternal(_ioID, getInternalRegisterArray(_ioID), _pin);
}

/**
 * @brief       setPorts sets states on every IO Expander pin at once.
 *
 * @param       uint16_t _d
 *              GPIO pin state of all IO Expander pins.
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 *
 * @note        Writing one to single bit will casue that pin to go high. Using this function is not allowed on internal
 * IO expander (except on Inkplate color).
 */
void Expander::setPorts(uint16_t _d, uint8_t _ioID)
{
    // Can't use this function on internal IO Expander except on a Inkplate Color!
    // Using this function can damage Inkplate!
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE4)
    if ((_ioID == IO_INT_ADDR))
        return;
#endif
    setPortsInternal(_ioID, getInternalRegisterArray(_ioID), _d);
}

/**
 * @brief       getPorts reads GPIO pin state on every IO Expander pin at once.
 *
 * @param       uint8_t _ioID
 *              internal or external IO Exapnder
 */
uint16_t Expander::getPorts(uint8_t _ioID)
{
    return getPortsInternal(_ioID, getInternalRegisterArray(_ioID));
}

/**
 * @brief       pinModeInternal sets IO Exapnder internal pin mode
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds IO Exapnder registers
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pi to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 *
 * @note        modes are defined in @esp32-hal-gpio.h
 */
void Expander::pinModeInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    switch (_mode)
    {
    case INPUT:
        _r[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        updatePCALRegister(_addr, PCAL6416A_CFGPORT0_ARRAY + _port, _r[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case OUTPUT:
        // There is a one cacth! Pins are by default (POR) set as HIGH. So first change it to LOW and then set is as
        // output).
        _r[PCAL6416A_CFGPORT0_ARRAY + _port] &= ~(1 << _pin);
        _r[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
        updatePCALRegister(_addr, PCAL6416A_OUTPORT0_ARRAY + _port, _r[PCAL6416A_OUTPORT0_ARRAY + _port]);
        updatePCALRegister(_addr, PCAL6416A_CFGPORT0_ARRAY + _port, _r[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case INPUT_PULLUP:
        _r[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        _r[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        _r[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] |= (1 << _pin);
        updatePCALRegister(_addr, PCAL6416A_CFGPORT0_ARRAY + _port, _r[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updatePCALRegister(_addr, PCAL6416A_PUPDEN_REG0_ARRAY + _port, _r[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updatePCALRegister(_addr, PCAL6416A_PUPDSEL_REG0_ARRAY + _port, _r[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    case INPUT_PULLDOWN:
        _r[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        _r[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        _r[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] &= ~(1 << _pin);
        updatePCALRegister(_addr, PCAL6416A_CFGPORT0_ARRAY + _port, _r[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updatePCALRegister(_addr, PCAL6416A_PUPDEN_REG0_ARRAY + _port, _r[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updatePCALRegister(_addr, PCAL6416A_PUPDSEL_REG0_ARRAY + _port, _r[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    }
}

/**
 * @brief       digitalWriteInternal sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds IO Exapnder registers
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 *
 * @note        DO NOT USE GPA0-GPA7 and GPB0. In code those are pins from
 * 0-8!!! Using those, you might permanently damage the screen. You should only
 * use pins from 9-15. Function will exit if pin mode isnt OUTPUT.
 */
void Expander::digitalWriteInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _state)
{
    if (_pin > 15)
        return;
    _state &= 1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _state ? _r[PCAL6416A_OUTPORT0_ARRAY + _port] |= (1 << _pin) : _r[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
    updatePCALRegister(_addr, PCAL6416A_OUTPORT0_ARRAY + _port, _r[PCAL6416A_OUTPORT0_ARRAY + _port]);
}

/**
 * @brief       digitalReadInternal reads io exapnder internal pin state
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @param       uint8_t _pin
 *              pin to set mode
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Expander::digitalReadInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin)
{
    if (_pin > 15)
        return -1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    readPCALRegister(_addr, PCAL6416A_INPORT0_ARRAY + _port, &_r[PCAL6416A_INPORT0_ARRAY + _port]);
    return ((_r[PCAL6416A_INPORT0_ARRAY + _port] >> _pin) & 1);
}

/**
 * @brief       setIntPinInternal function sets Interrupt on selected pin
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds IO Exapnder registers
 * @param       uint8_t *_pin
 *              selected pin
 */
void Expander::setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _r[PCAL6416A_INTMSK_REG0_ARRAY + _port] &= ~(1 << _pin);

    updatePCALRegister(_addr, PCAL6416A_INTMSK_REG0_ARRAY + _port, _r[PCAL6416A_INTMSK_REG0_ARRAY + _port]);
}

/**
 * @brief       removeIntPinInternal function removes Interrupt on selected pin
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @param       uint8_t *_pin
 *              selected pin
 */
void Expander::removeIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _r[PCAL6416A_INTMSK_REG0_ARRAY + _port] |= (1 << _pin);

    updatePCALRegister(_addr, PCAL6416A_INTMSK_REG0_ARRAY + _port, _r[PCAL6416A_INTMSK_REG0_ARRAY + _port]);
}

/**
 * @brief       getINTInternal function reads Interrupt pin state for all pins
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @return      returns interrupt state of both ports (INTF)
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1, bit can be set only if interrupt is enabled
 */
uint16_t Expander::getINTInternal(uint8_t _addr, uint8_t *_r)
{
    readPCALRegister(_addr, PCAL6416A_INTSTAT_REG0_ARRAY, &_r[PCAL6416A_INTSTAT_REG0_ARRAY]);
    readPCALRegister(_addr, PCAL6416A_INTSTAT_REG1_ARRAY, &_r[PCAL6416A_INTSTAT_REG1_ARRAY]);

    return ((_r[PCAL6416A_INTSTAT_REG1_ARRAY] << 8) | (_r[PCAL6416A_INTSTAT_REG0_ARRAY]));
}

/**
 * @brief       setPortsInternal sets all pins at once.
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @param       uint6_t _d
 *              GPIO data. Every bit represents one GPIO pin. Writing one to one bit sets pin on high logic level (is
 * selected pin is set as GPIO).
 */
void Expander::setPortsInternal(uint8_t _addr, uint8_t *_r, uint16_t _d)
{
    _r[PCAL6416A_OUTPORT0_ARRAY] = (_d & 0xff);
    _r[PCAL6416A_OUTPORT1_ARRAY] = (_d >> 8) & 0xff;
    updatePCALRegister(_addr, PCAL6416A_OUTPORT0_ARRAY, _r[PCAL6416A_OUTPORT0_ARRAY]);
    updatePCALRegister(_addr, PCAL6416A_OUTPORT1_ARRAY, _r[PCAL6416A_OUTPORT1_ARRAY]);
}

/**
 * @brief       getPortsInternal reads all pins at once.
 *
 * @param       uint8_t _addr
 *              IO Exapnder I2C address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 *
 * @return      GPIO data. Every bit represents one GPIO pin. Reading one to one bit means selected GPIO has high logic
 * state.
 */
uint16_t Expander::getPortsInternal(uint8_t _addr, uint8_t *_r)
{
    readPCALRegister(_addr, PCAL6416A_INPORT0_ARRAY, &_r[PCAL6416A_INPORT0_ARRAY]);
    readPCALRegister(_addr, PCAL6416A_INPORT1_ARRAY, &_r[PCAL6416A_INPORT1_ARRAY]);

    return (_r[PCAL6416A_INPORT0_ARRAY] | (_r[PCAL6416A_INPORT1_ARRAY]) << 8);
}

#endif
