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
 * @authors     e-radionica.com
 ***************************************************/

#if defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE6PLUSV2) ||               \
    defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATECOOL)

#include "Pcal.h"

// LOW LEVEL:

/**
 * @brief       ioBegin function starts pcal expander and sets registers values
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array to be writen in registers
 *
 * @return      true if successful, false otherwise
 *
 * @note        updates register 0 and 1 with 0xFF regardless of what array is
 * passed as _r
 */
bool Expander::ioBegin(uint8_t _addr, uint8_t *_r)
{
    Wire.beginTransmission(_addr);
    int error = Wire.endTransmission();
    if (error)
        return false;
    return true;
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
    Wire.requestFrom(_addr, (uint8_t)22);
    for (int i = 0; i < 22; ++i)
    {
        k[i] = Wire.read();
    }
}

/**
 * @brief       readPCALRegisters function uses i2c to read selected pcal
 * registers
 *
 * @param       uint8_t _regName
 *              name of register where read will start
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 * @param       uint8_t _n
 *              number of bites/registers to read
 */
void Expander::readPCALRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(_addr, _n);
    for (int i = 0; i < _n; ++i)
    {
        k[_regName + i] = Wire.read();
    }
}

/**
 * @brief       readPCALRegisters function uses i2c to read one selected pcal
 * register
 *
 * @param       uint8_t _regName
 *              name of register where read will start
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 */
void Expander::readPCALRegister(uint8_t _addr, uint8_t _regName, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)1);
    k[_regName] = Wire.read();
}

/**
 * @brief       updateAllRegisters function uses i2c to updates all pcal
 * registers
 *
 * @param       uint8_t *_k
 *              pointer to array where data to be uploaded is stored
 */
void Expander::updateAllRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    for (int i = 0; i < 22; ++i)
    {
        Wire.write(k[i]);
    }
    Wire.endTransmission();
}

/**
 * @brief       updateRegister function uses i2c to update selected pcal register
 *
 * @param       uint8_t _regName
 *              name of register that will be updated
 * @param       uint8_t _d
 *              data to be uploaded
 */
void Expander::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.write(_d);
    Wire.endTransmission();
}

/**
 * @brief       updateRegister function uses i2c to update some selected pcal
 * registers
 *
 * @param       uint8_t _regName
 *              name of register where update will start
 * @param       uint8_t *_k
 *              pointer to array that holds new data
 * @param       uint8_t _n
 *              number of bites/registers to write to
 */
void Expander::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    for (int i = 0; i < _n; ++i)
    {
        Wire.write(k[_regName + i]);
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
 */
void Expander::pinModeIO(uint8_t _pin, uint8_t _mode, uint8_t _io_id)
{
    if ((_io_id == IO_INT_ADDR) && (_pin < 9))
        return;
    pinModeInternal(_io_id, _io_id == IO_INT_ADDR ? ioRegsInt : ioRegsEx, _pin, _mode);
}

/**
 * @brief       digitalWriteIO sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 *
 */
void Expander::digitalWriteIO(uint8_t _pin, uint8_t _state, uint8_t _io_id)
{
    if ((_io_id == IO_INT_ADDR) && (_pin < 9))
        return;
    digitalWriteInternal(_io_id, _io_id == IO_INT_ADDR ? ioRegsInt : ioRegsEx, _pin, _state);
}

/**
 * @brief       digitalReadPCAL reads pcal internal pin state
 *
 * @param       uint8_t _pin
 *              pin to set mode
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Expander::digitalReadIO(uint8_t _pin, uint8_t _io_id)
{
    if ((_io_id == IO_INT_ADDR) && (_pin < 9))
        return 0;
    return digitalReadInternal(_io_id, _io_id == IO_INT_ADDR ? ioRegsInt : ioRegsEx, _pin);
}

/**
 * @brief       setIntPin function sets pcal interupt internal mode
 *
 * @param       uint8_t _pin
 *              pin to set interrupt mode to
 * @param       uint8_t _en
 *              interurpt mode (CHANGE, FALLING, RISING)
 */
void Expander::setIntPin(uint8_t _pin, uint8_t _en, uint8_t _io_id)
{
    setIntPinInternal(_io_id, _io_id == IO_INT_ADDR ? ioRegsInt : ioRegsEx, _pin, _en);
}

/**
 * @brief       getINTInternal function reads Interrupt from pin
 *
 * @return      returns interupt registers state
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t Expander::getINT(uint8_t _io_id)
{
    return getINTInternal(_io_id, _io_id == IO_INT_ADDR ? ioRegsInt : ioRegsEx);
}

/**
 * @brief       pinModeInternal sets io exapnder internal pin mode
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
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
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        updateRegister(_addr, PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case OUTPUT:
        // There is a one cacth! Pins are by default (POR) set as HIGH. So first change it to LOW and then set is as
        // output).
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] &= ~(1 << _pin);
        regs[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
        updateRegister(_addr, PCAL6416A_OUTPORT0 + _port, regs[PCAL6416A_OUTPORT0_ARRAY + _port]);
        updateRegister(_addr, PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case INPUT_PULLUP:
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] |= (1 << _pin);
        updateRegister(_addr, PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updateRegister(_addr, PCAL6416A_PUPDEN_REG0 + _port, regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updateRegister(_addr, PCAL6416A_PUPDSEL_REG0 + _port, regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    case INPUT_PULLDOWN:
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] &= ~(1 << _pin);
        updateRegister(_addr, PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updateRegister(_addr, PCAL6416A_PUPDEN_REG0 + _port, regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updateRegister(_addr, PCAL6416A_PUPDSEL_REG0 + _port, regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    }
}

/**
 * @brief       digitalWriteInternal sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
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

    _state ? regs[PCAL6416A_OUTPORT0_ARRAY + _port] |= (1 << _pin)
           : regs[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
    updateRegister(_addr, PCAL6416A_OUTPORT0 + _port, regs[PCAL6416A_OUTPORT0_ARRAY + _port]);
}

/**
 * @brief       digitalReadInternal reads io exapnder internal pin state
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
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

    readPCALRegister(_addr, PCAL6416A_INPORT0 + _port, &regs[PCAL6416A_INPORT0_ARRAY + _port]);

    return ((regs[PCAL6416A_INPORT0_ARRAY + _port] >> _pin) & 1);
}

/**
 * @brief       setIntPinInternal function sets io exapnder interupt internal mode
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @param       uint8_t *_pin
 *              pin to set interrupt mode to
 * @param       uint8_t _mode
 *              interurpt mode (CHANGE, FALLING, RISING)
 */
void Expander::setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _mode ? regs[PCAL6416A_INTMSK_REG0_ARRAY + _port] &= ~(1 << _pin)
          : regs[PCAL6416A_INTMSK_REG0_ARRAY + _port] |= (1 << _pin);

    updateRegister(_addr, PCAL6416A_INTMSK_REG0 + _port, regs[PCAL6416A_INTMSK_REG0_ARRAY + _port]);
}


/**
 * @brief       getINTInternal function reads Interrupt pin state for all pins
 *
 * @param       uint8_t _addr
 *              io exapnder i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds io exapnder registers
 * @return      returns interrupt state of both ports (INTF)
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1, bit can be set only if interrupt is enabled
 */
uint16_t Expander::getINTInternal(uint8_t _addr, uint8_t *_r)
{
    readPCALRegister(_addr, PCAL6416A_INTSTAT_REG0, &regs[PCAL6416A_INTSTAT_REG0_ARRAY]);
    readPCALRegister(_addr, PCAL6416A_INTSTAT_REG1, &regs[PCAL6416A_INTSTAT_REG1_ARRAY]);

    return ((regs[PCAL6416A_INTSTAT_REG1_ARRAY] << 8) | (regs[PCAL6416A_INTSTAT_REG0_ARRAY]));
}

/**
 * @brief       removeIntPin function removes Interrupt from pin
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 * @param       uint8_t _io_id
 *              internal or external io exapnder
 */
void Expander::removeIntPin(uint8_t _pin, uint8_t _io_id)
{
    if ((_io_id == IO_INT_ADDR) && (_pin < 9))
        return;
    removeIntPinInternal(_io_id, ioRegsEx, _pin);
}

#endif
