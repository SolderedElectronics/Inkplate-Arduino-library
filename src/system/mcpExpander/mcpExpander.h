/**
 **************************************************
 * @file        mcpExpander.h
 * @brief       class for controling MCP expander
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#ifndef __MCP_EXPANDER_H__
#define __MCP_EXPANDER_H__
#if defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE6PLUS)
#include "Arduino.h"
#include "Wire.h"

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
 * @brief       IOExpander class to be used for work with MCP23017 expander
 */
class IOExpander
{
  public:
    bool begin(uint8_t _addr);
    void pinMode(uint8_t _pin, uint8_t _mode, bool _bypassCheck = false);
    void digitalWrite(uint8_t _pin, uint8_t _state, bool _bypassCheck = false);
    uint8_t digitalRead(uint8_t _pin, bool _bypassCheck = false);
    void setIntPin(uint8_t _pin, uint8_t _mode = CHANGE);
    void setIntOutput(uint8_t _intPort, bool _mirroring, bool _openDrain, bool _polarity);
    void setPorts(uint16_t _d);
    void removeIntPin(uint8_t _pin);
    uint16_t getInt();
    uint16_t getIntState();
    uint16_t getInterruptFlagsAtBegin();
    uint16_t getInterruptCaptureAtBegin();
    uint16_t getPorts();
    void blockPinUsage(uint8_t _pin);
    void unblockPinUsage(uint8_t _pin);
    uint8_t _ioExpanderRegs[22];

  private:
    uint16_t _blockedPinsForUser = 0;
    // INTF and INTCAP register values captured during begin()'s bulk read,
    // BEFORE the same read clears the INTF latch as a side effect of passing
    // through INTCAP. Lets callers determine which pin caused a wake from
    // deep sleep after begin() has run.
    uint16_t _interruptFlagsAtBegin = 0;
    uint16_t _interruptCaptureAtBegin = 0;

    const uint8_t regAddresses[22] = {
        MCP23017_IODIRA,  MCP23017_IODIRB,  MCP23017_IPOLA,   MCP23017_IPOLB,   MCP23017_GPINTENA, MCP23017_GPINTENB,
        MCP23017_DEFVALA, MCP23017_DEFVALB, MCP23017_INTCONA, MCP23017_INTCONB, MCP23017_IOCONA,   MCP23017_IOCONB,
        MCP23017_GPPUA,   MCP23017_GPPUB,   MCP23017_INTFA,   MCP23017_INTFB,   MCP23017_INTCAPA,  MCP23017_INTCAPB,
        MCP23017_GPIOA,   MCP23017_GPIOB,   MCP23017_OLATA,   MCP23017_OLATB};

    uint8_t _ioExpanderI2CAddress;

    void pinModeInternal(uint8_t _pin, uint8_t _mode);
    void digitalWriteInternal(uint8_t _pin, uint8_t _state);
    uint8_t digitalReadInternal(uint8_t _pin);
    void setIntPinInternal(uint8_t _pin, uint8_t _mode);
    void setIntOutputInternal(uint8_t _intPort, bool _mirroring, bool _openDrain, bool _polarity);
    void removeIntPinInternal(uint8_t _pin);
    uint16_t getINTInternal();
    uint16_t getINTStateInternal();
    void setPortsInternal(uint16_t _d);
    uint16_t getPortsInternal();

    void readMCPRegisters();
    void readMCPRegisters(uint8_t _regIndex, uint8_t _n);
    void readMCPRegister(uint8_t _regIndex);
    void updateMCPAllRegisters();
    void updateMCPRegister(uint8_t _regIndex, uint8_t _d);

    bool checkForBlockedPins(uint8_t _pin);
};

#endif
#endif