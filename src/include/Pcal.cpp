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

#ifdef ARDUINO_INKPLATECOLOR

#include "Pcal.h"

// LOW LEVEL:

/**
 * @brief       pcalBegin function starts pcal expander and sets registers values
 *
 * @param       uint8_t *_r
 *              pointer to array to be writen in registers
 *
 * @return      true if successful, false otherwise
 *
 * @note        updates register 0 and 1 with 0xFF regardless of what array is
 * passed as _r
 */
bool Pcal::pcalBegin(uint8_t *_r)
{
    Wire.beginTransmission(0x21);
    int error = Wire.endTransmission();
    if (error)
        return false;
    // readPCALRegisters(_addr, _r);
    //_r[0] = 0xFF;
    //_r[1] = 0xFF;
    // updateAllRegisters(_addr, _r);
    return true;
}

/**
 * @brief       readPCALRegisters function uses i2c to read all pcal registers
 *
 * @param       uint8_t *_k
 *              pointer to array where pcal registers will be stored
 */
void Pcal::readPCALRegisters(uint8_t *k)
{
    Wire.beginTransmission(0x21);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(0x21, (uint8_t)22);
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
void Pcal::readPCALRegisters(uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(0x21);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(0x21, _n);
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
void Pcal::readPCALRegister(uint8_t _regName, uint8_t *k)
{
    Wire.beginTransmission(0x21);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(0x21, (uint8_t)1);
    k[_regName] = Wire.read();
}

/**
 * @brief       updateAllRegisters function uses i2c to updates all pcal
 * registers
 *
 * @param       uint8_t *_k
 *              pointer to array where data to be uploaded is stored
 */
void Pcal::updateAllRegisters(uint8_t *k)
{
    Wire.beginTransmission(0x21);
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
void Pcal::updateRegister(uint8_t _regName, uint8_t _d)
{
    Wire.beginTransmission(0x21);
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
void Pcal::updateRegister(uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(0x21);
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
 * @brief       pinModePCAL function sets internal pin mode
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 */
void Pcal::pinModePCAL(uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    switch (_mode)
    {
    case INPUT:
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        updateRegister(PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case OUTPUT:
        // There is a one cacth! Pins are by default (POR) set as HIGH. So first change it to LOW and then set is as
        // output).
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] &= ~(1 << _pin);
        regs[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
        updateRegister(PCAL6416A_OUTPORT0 + _port, regs[PCAL6416A_OUTPORT0_ARRAY + _port]);
        updateRegister(PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        break;
    case INPUT_PULLUP:
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] |= (1 << _pin);
        updateRegister(PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updateRegister(PCAL6416A_PUPDEN_REG0 + _port, regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updateRegister(PCAL6416A_PUPDSEL_REG0 + _port, regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    case INPUT_PULLDOWN:
        regs[PCAL6416A_CFGPORT0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port] |= (1 << _pin);
        regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port] &= ~(1 << _pin);
        updateRegister(PCAL6416A_CFGPORT0 + _port, regs[PCAL6416A_CFGPORT0_ARRAY + _port]);
        updateRegister(PCAL6416A_PUPDEN_REG0 + _port, regs[PCAL6416A_PUPDEN_REG0_ARRAY + _port]);
        updateRegister(PCAL6416A_PUPDSEL_REG0 + _port, regs[PCAL6416A_PUPDSEL_REG0_ARRAY + _port]);
        break;
    }
}

/**
 * @brief       digitalWritePCAL sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 *
 */
void Pcal::digitalWritePCAL(uint8_t _pin, uint8_t _state)
{
    if (_pin > 15)
        return;
    _state &= 1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _state ? regs[PCAL6416A_OUTPORT0_ARRAY + _port] |= (1 << _pin)
           : regs[PCAL6416A_OUTPORT0_ARRAY + _port] &= ~(1 << _pin);
    updateRegister(PCAL6416A_OUTPORT0 + _port, regs[PCAL6416A_OUTPORT0_ARRAY + _port]);
}

/**
 * @brief       digitalReadPCAL reads pcal internal pin state
 *
 * @param       uint8_t _pin
 *              pin to set mode
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Pcal::digitalReadPCAL(uint8_t _pin)
{
    if (_pin > 15)
        return -1;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    readPCALRegister(PCAL6416A_INPORT0 + _port, &regs[PCAL6416A_INPORT0_ARRAY + _port]);

    return ((regs[PCAL6416A_INPORT0_ARRAY + _port] >> _pin) & 1);
}

/**
 * @brief       setIntPin function sets pcal interupt internal mode
 *
 * @param       uint8_t _pin
 *              pin to set interrupt mode to
 * @param       uint8_t _en
 *              interurpt mode (CHANGE, FALLING, RISING)
 */
void Pcal::setIntPin(uint8_t _pin, uint8_t _en)
{
    if (_pin > 15)
        return;

    uint8_t _port = _pin / 8;
    _pin %= 8;

    _en ? regs[PCAL6416A_INTMSK_REG0_ARRAY + _port] &= ~(1 << _pin)
        : regs[PCAL6416A_INTMSK_REG0_ARRAY + _port] |= (1 << _pin);

    updateRegister(PCAL6416A_INTMSK_REG0 + _port, regs[PCAL6416A_INTMSK_REG0_ARRAY + _port]);
}

/**
 * @brief       getINTInternal function reads Interrupt from pin
 *
 * @return      returns interupt registers state
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t Pcal::getINT()
{
    readPCALRegister(PCAL6416A_INTSTAT_REG0, &regs[PCAL6416A_INTSTAT_REG0_ARRAY]);
    readPCALRegister(PCAL6416A_INTSTAT_REG1, &regs[PCAL6416A_INTSTAT_REG1_ARRAY]);

    return ((regs[PCAL6416A_INTSTAT_REG1_ARRAY] << 8) | (regs[PCAL6416A_INTSTAT_REG0_ARRAY]));
}
#endif
