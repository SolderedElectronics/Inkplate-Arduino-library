/**
 **************************************************
 * @file        PCAL.h
 * @brief       class for controling PCAL expander
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

#ifndef __PCAL_H__
#define __PCAL_H__
#if !defined(ARDUINO_INKPLATE6) && !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE6PLUS)
#include "Arduino.h"
#include "Wire.h"

// PCAL6416 Register Adresses
#define PCAL6416A_INPORT0        0x00
#define PCAL6416A_INPORT1        0x01
#define PCAL6416A_OUTPORT0       0x02
#define PCAL6416A_OUTPORT1       0x03
#define PCAL6416A_POLINVPORT0    0x04
#define PCAL6416A_POLINVPORT1    0x05
#define PCAL6416A_CFGPORT0       0x06
#define PCAL6416A_CFGPORT1       0x07
#define PCAL6416A_OUTDRVST_REG00 0x40
#define PCAL6416A_OUTDRVST_REG01 0x41
#define PCAL6416A_OUTDRVST_REG10 0x42
#define PCAL6416A_OUTDRVST_REG11 0x43
#define PCAL6416A_INLAT_REG0     0x44
#define PCAL6416A_INLAT_REG1     0x45
#define PCAL6416A_PUPDEN_REG0    0x46
#define PCAL6416A_PUPDEN_REG1    0x47
#define PCAL6416A_PUPDSEL_REG0   0x48
#define PCAL6416A_PUPDSEL_REG1   0x49
#define PCAL6416A_INTMSK_REG0    0x4A
#define PCAL6416A_INTMSK_REG1    0x4B
#define PCAL6416A_INTSTAT_REG0   0x4C
#define PCAL6416A_INTSTAT_REG1   0x4D
#define PCAL6416A_OUTPORT_CONF   0x4F

// User pins on IO Expander for Inkplate 6COLOR
#define IO_PIN_A0 0
#define IO_PIN_A1 1
#define IO_PIN_A2 2
#define IO_PIN_A3 3
#define IO_PIN_A4 4
#define IO_PIN_A5 5
#define IO_PIN_A6 6
#define IO_PIN_A7 7
#define IO_PIN_B0 8
#define IO_PIN_B1 9
#define IO_PIN_B2 10
#define IO_PIN_B3 11
#define IO_PIN_B4 12
#define IO_PIN_B5 13
#define IO_PIN_B6 14
#define IO_PIN_B7 15

/**
 * @brief       PCAL class to be used for work with PCAL6416 expander
 */
class IOExpander
{
  public:
    bool begin(uint8_t _addr);
    void pinMode(uint8_t _pin, uint8_t _mode, bool _bypassCheck = false);
    void digitalWrite(uint8_t _pin, uint8_t _state, bool _bypassCheck = false);
    uint8_t digitalRead(uint8_t _pin, bool _bypassCheck = false);
    void setIntPin(uint8_t _pin);
    void setPorts(uint16_t _d);
    void removeIntPin(uint8_t _pin);
    uint16_t getInt();
    uint16_t getPorts();
    void blockPinUsage(uint8_t _pin);
    void unblockPinUsage(uint8_t _pin);

  private:
    // IO expander pins that are blocked for user usage for safety reasons.
    // User can override this by setting _bypassCheck to true AT IT'S OWN RISK!
    uint16_t _blockedPinsForUser = 0;

    uint8_t _ioExpanderI2CAddress;

    uint8_t readPCALRegister(uint8_t _regAddr);
    void updatePCALRegister(uint8_t _regAddr, uint8_t _d);

    bool checkForBlockedPins(uint8_t _pin);
};

#endif
#endif
