/**
 **************************************************
 * @file        Mcp.cpp
 * @brief       class for controling io exapnder expander
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
#if defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE6PLUS)
#include "mcpExpander.h"
#include "../inkplateSemaphore.h"

/**
 * @brief       begin function starts MCP23017 expander and reads registers.
 *
 * @param       uint8_t _addr
 *              IO expander I2C address
 *
 * @return      true if successful, false otherwise
 */
bool IOExpander::begin(uint8_t _addr)
{
    _ioExpanderI2CAddress = _addr;

    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    int error = Wire.endTransmission();
    i2cEnd();
    if (error)
        return false;

    readMCPRegisters();

    // Configure the INT output for Inkplate-compatible defaults: active-HIGH
    // polarity, push-pull driver, ports not mirrored. The MCP23017 power-on
    // default for IOCON is 0x00 (INTPOL=0, active-LOW) which causes the INT
    // pin to idle HIGH and only drive LOW on an interrupt — the inverse of
    // what is wanted on the Inkplate boards (and on most host designs that
    // use the ESP32's ESP_EXT1_WAKEUP_ANY_HIGH for deep-sleep wake). Without
    // this step the host sees a permanently asserted wake line and exits
    // deep sleep immediately every cycle.
    //
    // v11.0.1 restored the setIntOutput() API but did not call it from
    // begin(). Setting a sensible default here means the library "just
    // works" on Inkplate hardware without requiring application code to
    // know about IOCON. Users with non-default wiring can still override by
    // calling setIntOutput() after begin().
    setIntOutput(MCP23017_INT_PORTB, MCP23017_INT_NO_MIRROR, MCP23017_INT_PUSHPULL, MCP23017_INT_ACTHIGH);
    // Snapshot the INTF and INTCAP values that we just read into the cache.
    // The bulk read in readMCPRegisters() returns registers 0x00..0x15
    // sequentially, so INTFA (0x0E) and INTFB (0x0F) are read BEFORE INTCAPA
    // (0x10) and INTCAPB (0x11). On the MCP23017, reading INTCAP is what
    // clears the INTF latch as a side effect. So the cache positions for
    // INTF (14, 15) and INTCAP (16, 17) currently hold the pre-clear values
    // captured at the moment of the most recent interrupt — but those cache
    // positions can be overwritten by subsequent reads (e.g. getInt() or
    // getIntState()). Save the pre-clear values into dedicated members so
    // application code can determine which expander pin caused a wake from
    // deep sleep after begin() has cleared the live latch.
    _interruptFlagsAtBegin = ((uint16_t)_ioExpanderRegs[15] << 8) | _ioExpanderRegs[14];
    _interruptCaptureAtBegin = ((uint16_t)_ioExpanderRegs[17] << 8) | _ioExpanderRegs[16];

#ifdef IO_INT_ADDR
    if (_addr == IO_INT_ADDR)
    {
        for (uint8_t pin = 0; pin < 9; pin++)
        {
            blockPinUsage(pin);
        }
    }
#endif

    return true;
}

/**
 * @brief       readMCPRegisters function uses I2C to read all MCP registers to avoid read-modify-write.
 */
void IOExpander::readMCPRegisters()
{
    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, (uint8_t)22);
    for (int i = 0; i < 22; i++)
    {
        _ioExpanderRegs[i] = Wire.read();
    }
    i2cEnd();
}

/**
 * @brief       readMCPRegisters function uses I2C to read selected MCP registers.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 * @param       uint8_t _n
 *              number of bytes/registers to read
 */
void IOExpander::readMCPRegisters(uint8_t _regIndex, uint8_t _n)
{
    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, _n);
    for (int i = 0; i < _n; i++)
    {
        _ioExpanderRegs[_regIndex + i] = Wire.read();
    }
    i2cEnd();
}

/**
 * @brief       readMCPRegister function uses I2C to read one selected MCP register.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 */
void IOExpander::readMCPRegister(uint8_t _regIndex)
{
    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.endTransmission();
    Wire.requestFrom(_ioExpanderI2CAddress, (uint8_t)1);
    _ioExpanderRegs[_regIndex] = Wire.read();
    i2cEnd();
}

/**
 * @brief       updateMCPAllRegisters function uses I2C to update all MCP registers.
 */
void IOExpander::updateMCPAllRegisters()
{
    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 22; i++)
    {
        Wire.write(_ioExpanderRegs[i]);
    }
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief       updateMCPRegister function uses I2C to update selected MCP register.
 *
 * @param       uint8_t _regIndex
 *              Start index of the MCP23017 registers
 * @param       uint8_t _d
 *              data to be uploaded
 */
void IOExpander::updateMCPRegister(uint8_t _regIndex, uint8_t _d)
{
    i2cStart();
    Wire.beginTransmission(_ioExpanderI2CAddress);
    Wire.write(regAddresses[_regIndex]);
    Wire.write(_d);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief       pinMode sets I/O expander pin mode.
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02, INPUT_PULLUP=0x05)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 */
void IOExpander::pinMode(uint8_t _pin, uint8_t _mode, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    pinModeInternal(_pin, _mode);
}

/**
 * @brief       digitalWrite sets I/O expander output pin state.
 *
 * @param       uint8_t _pin
 *              pin to set output
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 */
void IOExpander::digitalWrite(uint8_t _pin, uint8_t _state, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return;

    digitalWriteInternal(_pin, _state);
}

/**
 * @brief       digitalRead reads I/O expander pin state.
 *
 * @param       uint8_t _pin
 *              pin to read
 * @param       bool _bypassCheck
 *              Setting this to true will bypass user block on this GPIO pin.
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t IOExpander::digitalRead(uint8_t _pin, bool _bypassCheck)
{
    if (checkForBlockedPins(_pin) && !_bypassCheck)
        return 0;

    return digitalReadInternal(_pin);
}

/**
 * @brief       setIntPin enables interrupt on IO Expander pin.
 *
 * @param       uint8_t _pin
 *              pin to set interrupt on
 * @param       uint8_t _mode
 *              interrupt mode: CHANGE, FALLING, or RISING
 */
void IOExpander::setIntPin(uint8_t _pin, uint8_t _mode)
{
    setIntPinInternal(_pin, _mode);
}

/**
 * @brief       setIntOutput configures the interrupt output pin behavior.
 *
 * @param       uint8_t _intPort
 *              port whose IOCON to configure (MCP23017_INT_PORTA or MCP23017_INT_PORTB)
 * @param       bool _mirroring
 *              true = both INT pins mirror each other
 * @param       bool _openDrain
 *              true = INT pin is open-drain (overrides polarity)
 * @param       bool _polarity
 *              true = INT pin active high, false = active low
 */
void IOExpander::setIntOutput(uint8_t _intPort, bool _mirroring, bool _openDrain, bool _polarity)
{
    setIntOutputInternal(_intPort, _mirroring, _openDrain, _polarity);
}

/**
 * @brief       removeIntPin removes interrupt from pin.
 *
 * @param       uint8_t _pin
 *              pin to remove interrupt from
 */
void IOExpander::removeIntPin(uint8_t _pin)
{
    removeIntPinInternal(_pin);
}

/**
 * @brief       getInt reads interrupt register state (INTF - which pins triggered).
 *
 * @return      returns interrupt registers state
 */
uint16_t IOExpander::getInt()
{
    return getINTInternal();
}

/**
 * @brief       getIntState reads pin state captured at the moment interrupt fired (INTCAP).
 *
 * @return      captured GPIO state at interrupt time, both ports
 */
uint16_t IOExpander::getIntState()
{
    return getINTStateInternal();
}

/**
 * @brief       getInterruptFlagsAtBegin returns the value of the MCP23017's
 *              INTFA/INTFB latch as it was at the moment begin() was called.
 *
 *              begin()'s bulk register read passes through INTCAP, which
 *              clears the INTF latch as a side effect. After begin() returns,
 *              calling getInt() reads the live INTF register, which is now 0
 *              even if a wake-triggering interrupt was pending. This getter
 *              returns the value that was captured BEFORE that clear happened
 *              — useful for determining which pin triggered a wake from deep
 *              sleep on hardware where the MCP is kept powered during sleep
 *              (e.g. Inkplate 6 / 10).
 *
 * @return      INTFA in low byte, INTFB in high byte
 */
uint16_t IOExpander::getInterruptFlagsAtBegin()
{
    return _interruptFlagsAtBegin;
}

/**
 * @brief       getInterruptCaptureAtBegin returns the value of the MCP23017's
 *              INTCAPA/INTCAPB register as it was at the moment begin() was
 *              called.
 *
 *              Pairing this with getInterruptFlagsAtBegin() lets callers
 *              distinguish rising vs falling edges on each interrupt-enabled
 *              pin even after begin() has cleared the latch.
 *
 *              The library's getIntState() also returns INTCAP but does so
 *              via a fresh I2C read; that fresh value can be overwritten by
 *              any subsequent interrupt event (e.g. capacitive noise during
 *              application startup), whereas this snapshot is fixed at
 *              begin() time.
 *
 * @return      INTCAPA in low byte, INTCAPB in high byte
 */
uint16_t IOExpander::getInterruptCaptureAtBegin()
{
    return _interruptCaptureAtBegin;
}

/**
 * @brief       setPorts sets states on every IO Expander pin at once.
 *
 * @param       uint16_t _d
 *              GPIO pin state of all IO Expander pins.
 */
void IOExpander::setPorts(uint16_t _d)
{
    setPortsInternal(_d);
}

/**
 * @brief       getPorts reads GPIO pin state on every IO Expander pin at once.
 */
uint16_t IOExpander::getPorts()
{
    return getPortsInternal();
}

/**
 * @brief   Sets block on specific pin so user could not use it.
 *
 * @param   uint8_t _pin
 *          I/O expander GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 - IO_PIN_B7).
 */
void IOExpander::blockPinUsage(uint8_t _pin)
{
    _pin &= 15;
    _blockedPinsForUser |= 1ULL << _pin;
}

/**
 * @brief   Remove block on specific pin so user could use it.
 *
 * @param   uint8_t _pin
 *          I/O expander GPIO pin (IO_PIN_A0 - IO_PIN_A7, IO_PIN_B0 - IO_PIN_B7).
 */
void IOExpander::unblockPinUsage(uint8_t _pin)
{
    _pin &= 15;
    _blockedPinsForUser &= ~(1ULL << _pin);
}

/**
 * @brief       pinModeInternal sets MCP expander pin mode.
 *
 * @param       uint8_t _pin
 *              pin to set mode
 * @param       uint8_t _mode
 *              mode for pin to be set (INPUT=0x01, OUTPUT=0x02, INPUT_PULLUP=0x05)
 */
void IOExpander::pinModeInternal(uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case INPUT:
        _ioExpanderRegs[MCP23017_IODIRA + _port] |= 1 << _p;
        _ioExpanderRegs[MCP23017_GPPUA + _port] &= ~(1 << _p);
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;

    case INPUT_PULLUP:
        _ioExpanderRegs[MCP23017_IODIRA + _port] |= 1 << _p;
        _ioExpanderRegs[MCP23017_GPPUA + _port] |= 1 << _p;
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;

    case OUTPUT:
        _ioExpanderRegs[MCP23017_IODIRA + _port] &= ~(1 << _p);
        _ioExpanderRegs[MCP23017_GPPUA + _port] &= ~(1 << _p);
        updateMCPRegister(MCP23017_IODIRA + _port, _ioExpanderRegs[MCP23017_IODIRA + _port]);
        updateMCPRegister(MCP23017_GPPUA + _port, _ioExpanderRegs[MCP23017_GPPUA + _port]);
        break;
    }
}

/**
 * @brief       digitalWriteInternal sets MCP expander output pin state.
 *
 * @param       uint8_t _pin
 *              pin to set output
 * @param       uint8_t _state
 *              output pin state (0 or 1)
 */
void IOExpander::digitalWriteInternal(uint8_t _pin, uint8_t _state)
{
    if (_pin > 15)
        return;
    _state &= 1;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    if (_ioExpanderRegs[MCP23017_IODIRA + _port] & (1 << _p))
        return;

    _state ? (_ioExpanderRegs[MCP23017_GPIOA + _port] |= (1 << _p))
           : (_ioExpanderRegs[MCP23017_GPIOA + _port] &= ~(1 << _p));
    updateMCPRegister(MCP23017_GPIOA + _port, _ioExpanderRegs[MCP23017_GPIOA + _port]);
}

/**
 * @brief       digitalReadInternal reads MCP expander pin state.
 *
 * @param       uint8_t _pin
 *              pin to read
 *
 * @return      HIGH or LOW (1 or 0) value
 */
uint8_t IOExpander::digitalReadInternal(uint8_t _pin)
{
    if (_pin > 15)
        return 0;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    readMCPRegister(MCP23017_GPIOA + _port);
    return (_ioExpanderRegs[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

/**
 * @brief       setIntPinInternal enables interrupt for selected pin with configurable mode.
 *
 * @param       uint8_t _pin
 *              selected pin
 * @param       uint8_t _mode
 *              CHANGE: trigger on any change (INTCON compare disabled)
 *              FALLING: trigger when pin goes low (compare against DEFVAL=1)
 *              RISING: trigger when pin goes high (compare against DEFVAL=0)
 */
void IOExpander::setIntPinInternal(uint8_t _pin, uint8_t _mode)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;

    switch (_mode)
    {
    case CHANGE:
        _ioExpanderRegs[MCP23017_INTCONA + _port] &= ~(1 << _p);
        break;

    case FALLING:
        _ioExpanderRegs[MCP23017_INTCONA + _port] |= (1 << _p);
        _ioExpanderRegs[MCP23017_DEFVALA + _port] |= (1 << _p);
        break;

    case RISING:
        _ioExpanderRegs[MCP23017_INTCONA + _port] |= (1 << _p);
        _ioExpanderRegs[MCP23017_DEFVALA + _port] &= ~(1 << _p);
        break;

    default:
        return;
    }

    _ioExpanderRegs[MCP23017_GPINTENA + _port] |= (1 << _p);

    updateMCPRegister(MCP23017_GPINTENA + _port, _ioExpanderRegs[MCP23017_GPINTENA + _port]);
    updateMCPRegister(MCP23017_DEFVALA + _port, _ioExpanderRegs[MCP23017_DEFVALA + _port]);
    updateMCPRegister(MCP23017_INTCONA + _port, _ioExpanderRegs[MCP23017_INTCONA + _port]);
}

/**
 * @brief       setIntOutputInternal configures IOCON for the interrupt output pin.
 *
 * @param       uint8_t _intPort
 *              MCP23017_INT_PORTA (0) or MCP23017_INT_PORTB (1)
 * @param       bool _mirroring
 *              true = INTA/INTB mirror each other
 * @param       bool _openDrain
 *              true = open-drain output (overrides polarity)
 * @param       bool _polarity
 *              true = active high, false = active low
 */
void IOExpander::setIntOutputInternal(uint8_t _intPort, bool _mirroring, bool _openDrain, bool _polarity)
{
    _intPort &= 1;
    uint8_t &iocon = _ioExpanderRegs[MCP23017_IOCONA + _intPort];
    iocon = (iocon & ~(1 << 6)) | (_mirroring << 6);
    iocon = (iocon & ~(1 << 2)) | (_openDrain << 2);
    iocon = (iocon & ~(1 << 1)) | (_polarity << 1);
    updateMCPRegister(MCP23017_IOCONA + _intPort, iocon);
}

/**
 * @brief       removeIntPinInternal removes interrupt from selected pin.
 *
 * @param       uint8_t _pin
 *              selected pin
 */
void IOExpander::removeIntPinInternal(uint8_t _pin)
{
    if (_pin > 15)
        return;

    uint8_t _port = (_pin / 8) & 1;
    uint8_t _p = _pin % 8;
    _ioExpanderRegs[MCP23017_GPINTENA + _port] &= ~(1 << _p);
    updateMCPRegister(MCP23017_GPINTENA + _port, _ioExpanderRegs[MCP23017_GPINTENA + _port]);
}

/**
 * @brief       getINTInternal reads interrupt pin state for all pins.
 *
 * @return      returns interrupt state of both ports (INTF)
 */
uint16_t IOExpander::getINTInternal()
{
    readMCPRegisters(MCP23017_INTFA, 2);
    return ((_ioExpanderRegs[MCP23017_INTFB] << 8) | _ioExpanderRegs[MCP23017_INTFA]);
}

/**
 * @brief       getINTStateInternal reads captured GPIO state at the moment interrupt fired (INTCAP).
 *
 * @return      captured pin state of both ports at interrupt time
 */
uint16_t IOExpander::getINTStateInternal()
{
    readMCPRegisters(MCP23017_INTCAPA, 2);
    return ((_ioExpanderRegs[MCP23017_INTCAPB] << 8) | _ioExpanderRegs[MCP23017_INTCAPA]);
}

/**
 * @brief       setPortsInternal sets all pins at once.
 *
 * @param       uint16_t _d
 *              GPIO data. Every bit represents one GPIO pin.
 */
void IOExpander::setPortsInternal(uint16_t _d)
{
    _ioExpanderRegs[MCP23017_GPIOA] = _d & 0xff;
    _ioExpanderRegs[MCP23017_GPIOB] = (_d >> 8) & 0xff;
    updateMCPRegister(MCP23017_GPIOA, _ioExpanderRegs[MCP23017_GPIOA]);
    updateMCPRegister(MCP23017_GPIOB, _ioExpanderRegs[MCP23017_GPIOB]);
}

/**
 * @brief       getPortsInternal reads all pins at once.
 *
 * @return      GPIO data. Every bit represents one GPIO pin.
 */
uint16_t IOExpander::getPortsInternal()
{
    readMCPRegisters(MCP23017_GPIOA, 2);
    return (_ioExpanderRegs[MCP23017_GPIOA] | (_ioExpanderRegs[MCP23017_GPIOB] << 8));
}

/**
 * @brief   Checks for the pins that users are not allowed to use.
 *
 * @param   uint8_t _pin
 *          GPIO pin on the I/O expander
 * @return  bool
 *          true - Pin is blocked for the user
 * @return  false - User can use this pin.
 */
bool IOExpander::checkForBlockedPins(uint8_t _pin)
{
    if (_blockedPinsForUser & (1ULL << _pin))
        return true;

    return false;
}
#endif