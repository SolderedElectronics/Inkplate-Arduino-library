/**
 **************************************************
 * @file        Mcp.cpp
 * @brief       class for controling mcp expander
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

#include "Mcp.h"

// LOW LEVEL:

/**
 * @brief       mcpBegin function starts mcp expander and sets registers values
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array to be writen in registers
 *
 * @return      true if successful, false otherwise
 *
 * @note        updates register 0 and 1 with 0xFF regardless of what array is
 * passed as _r
 */
bool Mcp::mcpBegin(uint8_t _addr, uint8_t *_r) {
  Wire.beginTransmission(_addr);
  int error = Wire.endTransmission();
  if (error)
    return false;
  readMCPRegisters(_addr, _r);
  _r[0] = 0xFF;
  _r[1] = 0xFF;
  updateAllRegisters(_addr, _r);
  return true;
}

/**
 * @brief       readMCPRegisters function uses i2c to read all mcp registers
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_k
 *              pointer to array where mcp registers will be stored
 */
void Mcp::readMCPRegisters(uint8_t _addr, uint8_t *k) {
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(_addr, (uint8_t)22);
  for (int i = 0; i < 22; ++i) {
    k[i] = Wire.read();
  }
}

/**
 * @brief       readMCPRegisters function uses i2c to read selected mcp
 * registers
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t _regName
 *              name of register where read will start
 * @param       uint8_t *_k
 *              pointer to array where mcp registers will be stored
 * @param       uint8_t _n
 *              number of bites/registers to read
 */
void Mcp::readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k,
                           uint8_t _n) {
  Wire.beginTransmission(_addr);
  Wire.write(_regName);
  Wire.endTransmission();
  Wire.requestFrom(_addr, _n);
  for (int i = 0; i < _n; ++i) {
    k[_regName + i] = Wire.read();
  }
}

/**
 * @brief       readMCPRegisters function uses i2c to read one selected mcp
 * register
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t _regName
 *              name of register where read will start
 * @param       uint8_t *_k
 *              pointer to array where mcp registers will be stored
 */
void Mcp::readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k) {
  Wire.beginTransmission(_addr);
  Wire.write(_regName);
  Wire.endTransmission();
  Wire.requestFrom(_addr, (uint8_t)1);
  k[_regName] = Wire.read();
}

/**
 * @brief       updateAllRegisters function uses i2c to updates all mcp
 * registers
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_k
 *              pointer to array where data to be uploaded is stored
 */
void Mcp::updateAllRegisters(uint8_t _addr, uint8_t *k) {
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  for (int i = 0; i < 22; ++i) {
    Wire.write(k[i]);
  }
  Wire.endTransmission();
}

/**
 * @brief       updateRegister function uses i2c to update selected mcp register
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t _regName
 *              name of register that will be updated
 * @param       uint8_t _d
 *              data to be uploaded
 */
void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d) {
  Wire.beginTransmission(_addr);
  Wire.write(_regName);
  Wire.write(_d);
  Wire.endTransmission();
}

/**
 * @brief       updateRegister function uses i2c to update some selected mcp
 * registers
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t _regName
 *              name of register where update will start
 * @param       uint8_t *_k
 *              pointer to array that holds new data
 * @param       uint8_t _n
 *              number of bites/registers to write to
 */
void Mcp::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k,
                         uint8_t _n) {
  Wire.beginTransmission(_addr);
  Wire.write(_regName);
  for (int i = 0; i < _n; ++i) {
    Wire.write(k[_regName + i]);
  }
  Wire.endTransmission();
}

/**
 *  HIGH LEVEL
 */

/**
 * @brief       pinModeInternal sets mcp internal pin mode
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pi to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 *
 * @note        modes are defined in @esp32-hal-gpio.h
 */
void Mcp::pinModeInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin,
                          uint8_t _mode) {
  uint8_t _port = (_pin / 8) & 1;
  uint8_t _p = _pin % 8;

  switch (_mode) {
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

/**
 * @brief       digitalWriteInternal sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
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
void Mcp::digitalWriteInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin,
                               uint8_t _state) {
  uint8_t _port = (_pin / 8) & 1;
  uint8_t _p = _pin % 8;

  if (_r[MCP23017_IODIRA + _port] & (1 << _p))
    return; // Check if the pin is set as an output
  _state ? (_r[MCP23017_GPIOA + _port] |= (1 << _p))
         : (_r[MCP23017_GPIOA + _port] &= ~(1 << _p));
  updateRegister(_addr, MCP23017_GPIOA + _port, _r[MCP23017_GPIOA + _port]);
}

/**
 * @brief       digitalReadInternal reads mcp internal pin state
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint8_t _pin
 *              pin to set mode
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Mcp::digitalReadInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin) {
  uint8_t _port = (_pin / 8) & 1;
  uint8_t _p = _pin % 8;
  readMCPRegister(_addr, MCP23017_GPIOA + _port, _r);
  return (_r[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

/**
 * @brief       setIntOutputInternal sets mcp interrupt port state
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint8_t intPort
 *              portA or portB
 * @param       uint8_t mirroring
 *              set 1 to make ports mirror each other so that any interrupt will
 * cause both to go HIGH
 * @param       uint8_t openDrain
 *              set 1 to set interupt port as open drain, this will override
 * port polarity
 * @param       uint8_t polarity
 *              sets port interrupt polarity, 1 active high, 0 active low
 */
void Mcp::setIntOutputInternal(uint8_t _addr, uint8_t *_r, uint8_t intPort,
                               uint8_t mirroring, uint8_t openDrain,
                               uint8_t polarity) {
  intPort &= 1;
  mirroring &= 1;
  openDrain &= 1;
  polarity &= 1;
  _r[MCP23017_IOCONA + intPort] =
      (_r[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (mirroring << 6);
  _r[MCP23017_IOCONA + intPort] =
      (_r[MCP23017_IOCONA + intPort] & ~(1 << 2)) | (openDrain << 2);
  _r[MCP23017_IOCONA + intPort] =
      (_r[MCP23017_IOCONA + intPort] & ~(1 << 1)) | (polarity << 1);
  updateRegister(_addr, MCP23017_IOCONA + intPort,
                 _r[MCP23017_IOCONA + intPort]);
}

/**
 * @brief       setIntPinInternal function sets mcp interupt internal mode
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint8_t *_pin
 *              pin to set interrupt mode to
 * @param       uint8_t _mode
 *              interurpt mode (CHANGE, FALLING, RISING)
 */
void Mcp::setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin,
                            uint8_t _mode) {
  uint8_t _port = (_pin / 8) & 1;
  uint8_t _p = _pin % 8;

  switch (_mode) {
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

/**
 * @brief       pinModeMCP function sets internal pin mode
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02,
 * INPUT_PULLUP=0x05)
 */
void Mcp::pinModeMCP(uint8_t _pin, uint8_t _mode) {
  pinModeInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _mode);
}

/**
 * @brief       digitalWriteMCP sets internal output pin state (1 or 0)
 *
 * @param       uint8_t _pin
 *              pin to set output (DO NOT USE GPA0-GPA7 and GPB0. In code those
 * are pins from 0-8) only use 9-15
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 */
void Mcp::digitalWriteMCP(uint8_t _pin, uint8_t _state) {
  digitalWriteInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _state);
}

/**
 * @brief       digitalReadMCP reads mcp internal pin state
 *
 * @param       uint8_t _pin
 *              pin to set mode
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t Mcp::digitalReadMCP(uint8_t _pin) {
  return digitalReadInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin);
}

/**
 * @brief       setIntOutput sets mcp interrupt port state
 *
 * @param       uint8_t intPort
 *              portA or portB
 * @param       uint8_t mirroring
 *              set 1 to make ports mirror each other so that any interrupt will
 * cause both to go HIGH
 * @param       uint8_t openDrain
 *              set 1 to set interupt port as open drain, this will override
 * port polarity
 * @param       uint8_t polarity
 *              sets port interrupt polarity, 1 active high, 0 active low
 */
void Mcp::setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain,
                       uint8_t polarity) {
  setIntOutputInternal(MCP23017_EXT_ADDR, mcpRegsEx, intPort, mirroring,
                       openDrain, polarity);
}

/**
 * @brief       setIntPin function sets mcp interupt internal mode
 *
 * @param       uint8_t _pin
 *              pin to set interrupt mode to
 * @param       uint8_t _mode
 *              interurpt mode (CHANGE, FALLING, RISING)
 */
void Mcp::setIntPin(uint8_t _pin, uint8_t _mode) {
  setIntPinInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin, _mode);
}

/**
 * @brief       removeIntPin function removes Interrupt from pin
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 */
void Mcp::removeIntPin(uint8_t _pin) {
  removeIntPinInternal(MCP23017_EXT_ADDR, mcpRegsEx, _pin);
}

/**
 * @brief       getINTInternal function reads Interrupt from pin
 *
 * @return      returns interupt registers state
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t Mcp::getINT() { return getINTInternal(MCP23017_EXT_ADDR, mcpRegsEx); }

/**
 * @brief       getINTstate function reads Interrupt pins state at the time
 * interrupt occured
 *
 * @return      returns interupt registers state at the time interrupt occured
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1
 */
uint16_t Mcp::getINTstate() {
  return getINTstateInternal(MCP23017_EXT_ADDR, mcpRegsEx);
}

/**
 * @brief       setPorts sets internal state of PORTAB registers
 *
 * @param       uint16_t _d
 *              data to be set to PORTAB registers
 *
 * @note        MSB byte is for PORTB, LSB byte for PORTA
 */
void Mcp::setPorts(uint16_t _d) {
  setPortsInternal(MCP23017_EXT_ADDR, mcpRegsEx, _d);
}

/**
 * @brief       getPortsInternal gets register state of PORTSAB
 *
 * @return      returns register states of PORTSAB
 *
 * @note        MSB byte is for PORTB, LSB is for PORTA
 */
uint16_t Mcp::getPorts() {
  return getPortsInternal(MCP23017_EXT_ADDR, mcpRegsEx);
}

/**
 * @brief       removeIntPinInternal function removes Interrupt from pin
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 */
void Mcp::removeIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin) {
  uint8_t _port = (_pin / 8) & 1;
  uint8_t _p = _pin % 8;
  _r[MCP23017_GPINTENA + _port] &= ~(1 << _p);
  updateRegister(_addr, MCP23017_GPINTENA, _r, 2);
}

/**
 * @brief       getINTInternal function reads Interrupt pin state for all pins
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @return      returns interrupt state of both ports (INTF)
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1, bit can be set only if interrupt is enabled
 */
uint16_t Mcp::getINTInternal(uint8_t _addr, uint8_t *_r) {
  readMCPRegisters(_addr, MCP23017_INTFA, _r, 2);
  return ((_r[MCP23017_INTFB] << 8) | _r[MCP23017_INTFA]);
}

/**
 * @brief       getINTstateInternal gets interrupt pins state at the time
 * interrupt occured
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 *
 * @return      returns interrupt state of both ports, at the time the interrupt
 * occured (INTCAP)
 *
 * @note        Every bit represents interrupt pin, MSB is  PORTB PIN7, LSB is
 * PORTA PIN1, bit can be set only if interrupt is enabled
 */
uint16_t Mcp::getINTstateInternal(uint8_t _addr, uint8_t *_r) {
  readMCPRegisters(_addr, MCP23017_INTCAPA, _r, 2);
  return ((_r[MCP23017_INTCAPB] << 8) | _r[MCP23017_INTCAPA]);
}

/**
 * @brief       setPortsInternal sets internal state of PORTAB registers
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 * @param       uint16_t _d
 *              data to be set to PORTAB registers
 *
 * @note        MSB byte is for PORTB, LSB byte for PORTA
 */
void Mcp::setPortsInternal(uint8_t _addr, uint8_t *_r, uint16_t _d) {
  _r[MCP23017_GPIOA] = _d & 0xff;
  _r[MCP23017_GPIOB] = (_d >> 8) & 0xff;
  updateRegister(_addr, MCP23017_GPIOA, _r, 2);
}

/**
 * @brief       getPortsInternal gets register state of PORTSAB
 *
 * @param       uint8_t _addr
 *              mcp i2c address
 * @param       uint8_t *_r
 *              pointer to array that holds mcp registers
 *
 * @return      returns register states of PORTSAB
 *
 * @note        MSB byte is for PORTB, LSB is for PORTA
 */
uint16_t Mcp::getPortsInternal(uint8_t _addr, uint8_t *_r) {
  readMCPRegisters(_addr, MCP23017_GPIOA, _r, 2);
  return ((_r[MCP23017_GPIOB] << 8) | (_r[MCP23017_GPIOA]));
}