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

/**
 * @brief       Mcp class to be used for work with MCP expander
 */
class Mcp {
public:
  void pinModeMCP(uint8_t _pin, uint8_t _mode);
  void digitalWriteMCP(uint8_t _pin, uint8_t _state);
  uint8_t digitalReadMCP(uint8_t _pin);
  void setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain,
                    uint8_t polarity);
  void setIntPin(uint8_t _pin, uint8_t _mode);
  void removeIntPin(uint8_t _pin);
  uint16_t getINT();
  uint16_t getINTstate();
  void setPorts(uint16_t _d);
  uint16_t getPorts();
  uint8_t mcpRegsInt[22], mcpRegsEx[22];

  bool mcpBegin(uint8_t _addr, uint8_t *_r);
  void pinModeInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin, uint8_t _mode);
  void digitalWriteInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin,
                            uint8_t _state);
  uint8_t digitalReadInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin);

  void setIntOutputInternal(uint8_t _addr, uint8_t *_r, uint8_t intPort,
                            uint8_t mirroring, uint8_t openDrain,
                            uint8_t polarity);
  void setIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin,
                         uint8_t _mode);
  void removeIntPinInternal(uint8_t _addr, uint8_t *_r, uint8_t _pin);
  uint16_t getINTInternal(uint8_t _addr, uint8_t *_r);
  uint16_t getINTstateInternal(uint8_t _addr, uint8_t *_r);
  void setPortsInternal(uint8_t _addr, uint8_t *_r, uint16_t _d);
  uint16_t getPortsInternal(uint8_t _addr, uint8_t *_r);

private:
  void readMCPRegisters(uint8_t _addr, uint8_t *k);
  void readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k,
                        uint8_t _n);
  void readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k);
  void updateAllRegisters(uint8_t _addr, uint8_t *k);
  void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d);
  void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
};

#endif