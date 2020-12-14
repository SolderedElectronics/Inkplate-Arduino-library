/*
Mcp.cpp
Mcp 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Mcp-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.Mcp.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Mcp.h"

// LOW LEVEL:

bool Mcp::mcpBegin(uint8_t _addr, uint8_t *_r)
{
    Wire.beginTransmission(_addr);
    int error = Wire.endTransmission();
    if (error)
        return false;
    readMCPRegisters(_addr, _r);
    _r[0] = 0xff;
    _r[1] = 0xff;
    updateAllRegisters(_addr, _r);
    return true;
}

void Mcp::readMCPRegisters(uint8_t _addr, uint8_t *k)
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

void Mcp::readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
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

void Mcp::readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.endTransmission();
    Wire.requestFrom(_addr, (uint8_t)1);
    k[_regName] = Wire.read();
}

void Mcp::updateAllRegisters(uint8_t _addr, uint8_t *k)
{
    Wire.beginTransmission(_addr);
    Wire.write(0x00);
    for (int i = 0; i < 22; ++i)
    {
        Wire.write(k[i]);
    }
    Wire.endTransmission();
}

void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    Wire.write(_d);
    Wire.endTransmission();
}

void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
    Wire.beginTransmission(_addr);
    Wire.write(_regName);
    for (int i = 0; i < _n; ++i)
    {
        Wire.write(k[_regName + i]);
    }
    Wire.endTransmission();
}

// HIGH LEVEL:
void Mcp::pinModeInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case INPUT:
        _r[MCP23017_IODIRA + _port] |= 1 << _p;   // Set it to input
        _r[MCP23017_GPPUA + _port] &= ~(1 << _p); // Disable pullup on that pin
        updateRegister(_addr, MCP23017_IODIRA + _port, _r[MCP23017_IODIRA + _port]);
        updateRegister(_addr, MCP23017_GPPUA + _port, _r[MCP23017_GPPUA + _port]);
        break;

    case INPUT_PULLUP:
        _r[MCP23017_IODIRA + _port] |= 1 << _p; // Set it to input
        _r[MCP23017_GPPUA + _port] |= 1 << _p;  // Enable pullup on that pin
        updateRegister(_addr, MCP23017_IODIRA + _port, _r[MCP23017_IODIRA + _port]);
        updateRegister(_addr, MCP23017_GPPUA + _port, _r[MCP23017_GPPUA + _port]);
        break;

    case OUTPUT:
        _r[MCP23017_IODIRA + _port] &= ~(1 << _p); // Set it to output
        _r[MCP23017_GPPUA + _port] &= ~(1 << _p);  // Disable pullup on that pin
        updateRegister(_addr, MCP23017_IODIRA + _port, _r[MCP23017_IODIRA + _port]);
        updateRegister(_addr, MCP23017_GPPUA + _port, _r[MCP23017_GPPUA + _port]);
        break;
    }
}

void Mcp::digitalWriteInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _state)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    if (_r[MCP23017_IODIRA + _port] & (1 << _p))
        return; // Check if the pin is set as an output
    _state ? (_r[MCP23017_GPIOA + _port] |= (1 << _p)) : (_r[MCP23017_GPIOA + _port] &= ~(1 << _p));
    updateRegister(_addr, MCP23017_GPIOA + _port, _r[MCP23017_GPIOA + _port]);
}

uint8_t Mcp::digitalReadInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    readMCPRegister(_addr, MCP23017_GPIOA + _port, _r);
    return (_r[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

void Mcp::setIntOutputInternal(uint8_t _addr, uint8_t *_r, uint8_t intPort, uint8_t mirroring, uint8_t openDrain,
                               uint8_t polarity)
{
    intPort &= 1;
    mirroring &= 1;
    openDrain &= 1;
    polarity &= 1;
    _r[MCP23017_IOCONA + intPort] = (_r[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (mirroring << 6);
    _r[MCP23017_IOCONA + intPort] = (_r[MCP23017_IOCONA + intPort] & ~(1 << 2)) | (openDrain << 2);
    _r[MCP23017_IOCONA + intPort] = (_r[MCP23017_IOCONA + intPort] & ~(1 << 1)) | (polarity << 1);
    updateRegister(_addr, MCP23017_IOCONA + intPort, _r[MCP23017_IOCONA + intPort]);
}

void Mcp::setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case CHANGE:
        _r[MCP23017_INTCONA + _port] &= ~(1 << _p);
        break;

    case FALLING:
        _r[MCP23017_INTCONA + _port] |= (1 << _p);
        _r[MCP23017_DEFVALA + _port] |= (1 << _p);
        break;

    case RISING:
        _r[MCP23017_INTCONA + _port] |= (1 << _p);
        _r[MCP23017_DEFVALA + _port] &= ~(1 << _p);
        break;
    }
    _r[MCP23017_GPINTENA + _port] |= (1 << _p);
    updateRegister(_addr, MCP23017_GPINTENA, _r, 6);
}

void Mcp::pinModeMCP(uint8_t _pin, uint8_t _mode)
{
    pinModeInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _mode);
}

void Mcp::digitalWriteMCP(uint8_t _pin, uint8_t _state)
{
    digitalWriteInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _state);
}

uint8_t Mcp::digitalReadMCP(uint8_t _pin)
{
    return digitalReadInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin);
}

void Mcp::setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity)
{
    setIntOutputInternal(MCP23017_EXT_ADDR, mcpRegsEx, intPort, mirroring, openDrain, polarity);
}

void Mcp::setIntPin(uint8_t _pin, uint8_t _mode)
{
    setIntPinInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _mode);
}

void Mcp::removeIntPin(uint8_t _pin)
{
    removeIntPinInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin);
}

uint16_t Mcp::getINT()
{
    return getINTInternal(MCP23017_EXT_ADDR, mcpRegsEx);
}

uint16_t Mcp::getINTstate()
{
    return getINTstateInternal(MCP23017_EXT_ADDR, mcpRegsEx);
}

void Mcp::setPorts(uint16_t _d)
{
    setPortsInternal(MCP23017_EXT_ADDR, mcpRegsEx, _d);
}

uint16_t Mcp::getPorts()
{
    return getPortsInternal(MCP23017_EXT_ADDR, mcpRegsEx);
}

void Mcp::removeIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin)
{
    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    _r[MCP23017_GPINTENA + _port] &= ~(1 << _p);
    updateRegister(_addr, MCP23017_GPINTENA, _r, 2);
}

uint16_t Mcp::getINTInternal(uint8_t _addr, uint8_t *_r)
{
    readMCPRegisters(_addr, MCP23017_INTFA, _r, 2);
    return ((_r[MCP23017_INTFB] << 8) | _r[MCP23017_INTFA]);
}

uint16_t Mcp::getINTstateInternal(uint8_t _addr, uint8_t *_r)
{
    readMCPRegisters(_addr, MCP23017_INTCAPA, _r, 2);
    return ((_r[MCP23017_INTCAPB] << 8) | _r[MCP23017_INTCAPA]);
}

void Mcp::setPortsInternal(uint8_t _addr, uint8_t *_r, uint16_t _d)
{
    _r[MCP23017_GPIOA] = _d & 0xff;
    _r[MCP23017_GPIOB] = (_d >> 8) & 0xff;
    updateRegister(_addr, MCP23017_GPIOA, _r, 2);
}

uint16_t Mcp::getPortsInternal(uint8_t _addr, uint8_t *_r)
{
    readMCPRegisters(_addr, MCP23017_GPIOA, _r, 2);
    return ((_r[MCP23017_GPIOB] << 8) | (_r[MCP23017_GPIOA]));
}