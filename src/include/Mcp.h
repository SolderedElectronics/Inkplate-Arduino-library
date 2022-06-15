/**
 **************************************************
 * @file        Mcp.h
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

#ifndef __MCP_H__
#define __MCP_H__

#include "Arduino.h"
#include "Wire.h"
#include "defines.h"


#define IO_INT_ADDR      0x20
#define IO_EXT_ADDR      0x22
#define MCP23017_INT_PORTA     0x00
#define MCP23017_INT_PORTB     0x01
#define MCP23017_INT_NO_MIRROR false
#define MCP23017_INT_MIRROR    true
#define MCP23017_INT_PUSHPULL  false
#define MCP23017_INT_OPENDRAIN true
#define MCP23017_INT_ACTLOW    false
#define MCP23017_INT_ACTHIGH   true

#define MCP23017_IODIRA   0x00
#define MCP23017_IPOLA    0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA  0x06
#define MCP23017_INTCONA  0x08
#define MCP23017_IOCONA   0x0A
#define MCP23017_GPPUA    0x0C
#define MCP23017_INTFA    0x0E
#define MCP23017_INTCAPA  0x10
#define MCP23017_GPIOA    0x12
#define MCP23017_OLATA    0x14

#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATB    0x15

// User pins on MCP for Inkplate 10
// If using MCP with address 0x20 (IO_INT_ADDR) use only pins B1-B7
// Do not use others(0-8) cause they can permanently damage your screen
#define IO_PIN_B1 9
#define IO_PIN_B2 10
#define IO_PIN_B3 11
#define IO_PIN_B4 12
#define IO_PIN_B5 13
#define IO_PIN_B6 14
#define IO_PIN_B7 15

// Rest of the pins can be used only with MCP with address 0x22
// (IO_EXT_ADDR) MCP high level functions (those without address in
// signature) in Inkplate 10 use IO_EXT_ADDR by default
#define IO_PIN_A0 0
#define IO_PIN_A1 1
#define IO_PIN_A2 2
#define IO_PIN_A3 3
#define IO_PIN_A4 4
#define IO_PIN_A5 5
#define IO_PIN_A6 6
#define IO_PIN_A7 7
#define IO_PIN_B0 8

/**
 * @brief       Mcp class to be used for work with MCP expander
 */
class Mcp
{
  public:
    void pinModeIO(uint8_t _pin, uint8_t _mode, uint8_t _io_id = IO_EXT_ADDR);
    void digitalWriteIO(uint8_t _pin, uint8_t _state, uint8_t _io_id = IO_EXT_ADDR);
    uint8_t digitalReadIO(uint8_t _pin, uint8_t _io_id = IO_EXT_ADDR);
    void setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity,
                      uint8_t _io_id = IO_EXT_ADDR);
    void setPorts(uint16_t _d, uint8_t _io_id = IO_EXT_ADDR);
    void removeIntPin(uint8_t _pin, uint8_t _io_id = IO_EXT_ADDR);
    uint16_t getINT(uint8_t _io_id = IO_EXT_ADDR);
    uint16_t getINTstate(uint8_t _io_id = IO_EXT_ADDR);
    void setPorts(uint16_t _d, uint8_t _io_id = IO_EXT_ADDR);
    uint16_t getPorts(uint8_t _io_id = IO_EXT_ADDR);

    uint8_t ioRegsInt[22], ioRegsEx[22];

    bool ioBegin(uint8_t _addr, uint8_t *_r);

    void pinModeInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode);
    void digitalWriteInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _state);
    uint8_t digitalReadInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin);
    void setIntOutputInternal(uint8_t _addr, uint8_t *_r, uint8_t intPort, uint8_t mirroring, uint8_t openDrain,
                              uint8_t polarity);
    void setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode);
    void removeIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin);
    uint16_t getINTInternal(uint8_t _addr, uint8_t *_r);
    uint16_t getINTstateInternal(uint8_t _addr, uint8_t *_r);
    void setPortsInternal(uint8_t _addr, uint8_t *_r, uint16_t _d);
    uint16_t getPortsInternal(uint8_t _addr, uint8_t *_r);

  private:
    bool second_io_inited = 0;
    void readMCPRegisters(uint8_t _addr, uint8_t *k);
    void readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
    void readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k);
    void updateAllRegisters(uint8_t _addr, uint8_t *k);
    void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d);
    void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
};

#endif