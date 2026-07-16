#include "IST9201.h"
#include "../../system/inkplateSemaphore.h"

/**
 * @brief       Store pin references and put the PMIC into its default (disabled) state.
 *              pwrEnPin/chipEnPin/vddpPin/vddnPin/vcnpPin live on the IO expander (all the
 *              panel's power-switch/enable lines are routed there on this board). pgPin,
 *              psPin, toggleSel0Pin and toggleSel1Pin are direct ESP32 GPIOs.
 */
void IST9201::begin(IOExpander *expander, uint8_t pwrEnPin, uint8_t chipEnPin, uint8_t vddpPin, uint8_t vddnPin,
                    uint8_t vcnpPin, uint8_t toggleSel0Pin, uint8_t toggleSel1Pin, uint8_t pgPin, uint8_t psPin)
{
    _expander = expander;
    _pwrEnPin = pwrEnPin;
    _chipEnPin = chipEnPin;
    _vddpPin = vddpPin;
    _vddnPin = vddnPin;
    _vcnpPin = vcnpPin;
    _toggleSel0Pin = toggleSel0Pin;
    _toggleSel1Pin = toggleSel1Pin;
    _pgPin = pgPin;
    _psPin = psPin;

    _expander->pinMode(_pwrEnPin, OUTPUT);
    _expander->pinMode(_chipEnPin, OUTPUT);
    _expander->pinMode(_vddpPin, OUTPUT);
    _expander->pinMode(_vddnPin, OUTPUT);
    _expander->pinMode(_vcnpPin, OUTPUT);
    _expander->digitalWrite(_pwrEnPin, LOW);
    _expander->digitalWrite(_chipEnPin, LOW);
    _expander->digitalWrite(_vddpPin, LOW);
    _expander->digitalWrite(_vddnPin, LOW);
    _expander->digitalWrite(_vcnpPin, LOW);

    pinMode(_pgPin, INPUT);
    pinMode(_psPin, INPUT);
    pinMode(_toggleSel0Pin, OUTPUT);
    pinMode(_toggleSel1Pin, OUTPUT);
    digitalWrite(_toggleSel0Pin, LOW);
    digitalWrite(_toggleSel1Pin, LOW);
}

/**
 * @brief       Writes the full initPmicData ladder to the IST9201 over I2C.
 *              Ported register-for-register from the vendor's setPmic().
 */
bool IST9201::setRegisters()
{
    writeReg(IST9201_REG_VPOS1, initPmicData[0], initPmicData[1]);
    writeReg(IST9201_REG_VPOS2, initPmicData[4], initPmicData[5]);
    writeReg(IST9201_REG_VPOS3, initPmicData[8], initPmicData[9]);
    writeReg(IST9201_REG_VNEG1, initPmicData[2], initPmicData[3]);
    writeReg(IST9201_REG_VNEG2, initPmicData[6], initPmicData[7]);
    writeReg(IST9201_REG_VNEG3, initPmicData[10], initPmicData[11]);
    writeReg(IST9201_REG_VCOMDC, initPmicData[12], initPmicData[13]);
    writeReg(IST9201_REG_VCOMH, initPmicData[14], initPmicData[15]);
    writeReg(IST9201_REG_VCOML, initPmicData[16], initPmicData[17]);
    writeReg(IST9201_REG_DELAY1, initPmicData[18]);
    writeReg(IST9201_REG_DELAY2, initPmicData[19]);
    writeReg(IST9201_REG_VDDH_EXT_DELAY, initPmicData[20]);
    writeReg(IST9201_REG_VGH1, initPmicData[21], initPmicData[22]);
    writeReg(IST9201_REG_VGH2, initPmicData[23], initPmicData[24]);
    writeReg(IST9201_REG_0x1A, initPmicData[25]);
    writeReg(IST9201_REG_0x1B, initPmicData[26]);
    writeReg(IST9201_REG_0x1C, initPmicData[27]);

    return true;
}

/**
 * @brief       Ramps up the VDDN/VDDP/VNCP power switches, in that order, with the
 *              settling delays specified by the vendor sample (powerSwitchEnable()).
 */
void IST9201::powerSwitchEnable()
{
    _expander->digitalWrite(_vddnPin, HIGH);
    delay(50); // Wait for VDDN to be ready.
    _expander->digitalWrite(_vddpPin, HIGH);
    delay(50); // Wait for VDDP to be ready.
    _expander->digitalWrite(_vcnpPin, HIGH);
}

/**
 * @brief       Full per-refresh power-up sequence: enable the upstream 12V buck/boost
 *              (PMIC_PWR_EN) if it isn't already on, program the voltage ladder, enable
 *              the VDDN/VDDP/VNCP switches, then enable the chip itself and wait for
 *              PMIC_PG. Mirrors the vendor's enablePmic(), extended with the
 *              board-specific PMIC_PWR_EN pre-enable stage (see CLAUDE.md).
 *
 * @param       uint16_t timeoutMs
 *              Maximum time to wait for PMIC_PG to go high.
 * @return      true if PMIC_PG asserted within the timeout, false otherwise.
 */
bool IST9201::powerUp(uint16_t timeoutMs)
{
    if (!_pwrRailEnabled)
    {
        _expander->digitalWrite(_pwrEnPin, HIGH);
        delay(10); // Let the external 12V buck/boost rail settle before using it.
        _pwrRailEnabled = true;
    }

    setRegisters();
    delay(100);
    powerSwitchEnable();
    _expander->digitalWrite(_chipEnPin, HIGH);

    unsigned long timer = millis();
    while (digitalRead(_pgPin) == LOW && (millis() - timer) < timeoutMs)
    {
        delay(1);
    }

    _poweredUp = (digitalRead(_pgPin) == HIGH);
    return _poweredUp;
}

/**
 * @brief       Lightweight per-refresh power-down: disables the chip enable and waits
 *              for PMIC_PG to fall. Does NOT touch VDDN/VDDP/VNCP or the upstream
 *              12V rail - those stay biased between refreshes, exactly like the
 *              vendor's disablePmic(). Call railsOff() before a long sleep instead.
 */
void IST9201::powerDown()
{
    _expander->digitalWrite(_chipEnPin, LOW);

    unsigned long timer = millis();
    while (digitalRead(_pgPin) == HIGH && (millis() - timer) < 250)
    {
        delay(1);
    }

    _poweredUp = false;
}

/**
 * @brief       Full rail teardown for extended sleep: VNCP, then VDDP, then VDDN, then
 *              the upstream 12V rail, each with the vendor-documented discharge delay.
 *              Mirrors powerSwitchDisable(). Not called on every refresh - only when
 *              preparing for deep sleep.
 */
void IST9201::railsOff()
{
    delay(5000); // Discharge time of PMIC's power supply (vendor-specified).
    _expander->digitalWrite(_vcnpPin, LOW);
    delay(5000); // Discharge time of VNCP power supply (vendor-specified).
    _expander->digitalWrite(_vddpPin, LOW);
    delay(3000); // Discharge time of VDDP power supply (vendor-specified).
    _expander->digitalWrite(_vddnPin, LOW);

    _expander->digitalWrite(_pwrEnPin, LOW);
    _pwrRailEnabled = false;
}

/**
 * @brief       Returns whether PMIC_PG is currently asserted.
 */
bool IST9201::isPowerGood()
{
    return digitalRead(_pgPin) == HIGH;
}

/**
 * @brief       Recomputes VPOS2/VPOS3/VNEG2/VNEG3/VCOM from the panel's own PWR/VCOM
 *              readback (as produced by the panel's setEpdPower()-equivalent flow), and
 *              stores the results back into initPmicData ready for the next setRegisters()
 *              call. Ported from the vendor's setPwrToPmic()/voltageToRegisterData().
 *
 *              Not called automatically - this is available for a future auto-VCOM
 *              calibration pass once the fixed initPmicData table has been validated
 *              against real hardware.
 *
 * @param       uint8_t *panelReadback
 *              5 bytes: [0..3] = PWR readback for VPOS2/VNEG2/VPOS3/VNEG3 (0-120 range),
 *              [4] = VCOM readback (already de-biased, i.e. raw - 128).
 */
void IST9201::setVcomFromPanel(uint8_t *panelReadback)
{
    auto voltageToRegisterData = [](double voltageData, uint8_t setSelect) -> unsigned int {
        switch (setSelect)
        {
        case 2: // VPOS/VNEG "2" ladder
            return 1023 - (unsigned int)((170 - voltageData) * 1000 / 176);
        case 3: // VPOS/VNEG "3" ladder
            return 1023 - (unsigned int)((270 - voltageData) * 1000 / 274);
        case 4: // VCOMDC
            return 255 - (unsigned int)((50 - voltageData) * 250 / 49);
        case 5: // VCOMH
            return 511 + (unsigned int)((voltageData - 20) * 1250 / 39);
        case 6: // VCOML
            return 511 - (unsigned int)((-voltageData + 25) * 1250 / 39);
        default:
            return 0;
        }
    };

    unsigned int buf;
    buf = voltageToRegisterData(panelReadback[0], 2);
    initPmicData[4] = buf / 256 + (initPmicData[4] & 0xFC);
    initPmicData[5] = buf % 256;
    buf = voltageToRegisterData(panelReadback[1], 3);
    initPmicData[8] = buf / 256 + (initPmicData[8] & 0xFC);
    initPmicData[9] = buf % 256;
    buf = voltageToRegisterData(panelReadback[2], 2);
    initPmicData[6] = buf / 256 + (initPmicData[6] & 0xFC);
    initPmicData[7] = buf % 256;
    buf = voltageToRegisterData(panelReadback[3], 3);
    initPmicData[10] = buf / 256 + (initPmicData[10] & 0xFC);
    initPmicData[11] = buf % 256;
    buf = voltageToRegisterData(panelReadback[4], 4);
    initPmicData[12] = buf / 256 + (initPmicData[12] & 0xFE);
    initPmicData[13] = buf % 256;
    buf = voltageToRegisterData(15 - panelReadback[4] * 0.05, 5);
    initPmicData[14] = buf / 256 + (initPmicData[14] & 0xFE);
    initPmicData[15] = buf % 256;
    buf = voltageToRegisterData(15 + panelReadback[4] * 0.05, 6);
    initPmicData[16] = buf / 256 + (initPmicData[16] & 0xFE);
    initPmicData[17] = buf % 256;
}

/**
 * @brief       Writes a single-byte IST9201 register.
 */
void IST9201::writeReg(uint8_t reg, uint8_t value1)
{
    i2cStart();
    Wire.beginTransmission(IST9201_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value1);
    Wire.endTransmission();
    i2cEnd();
}

/**
 * @brief       Writes a two-byte IST9201 register.
 */
void IST9201::writeReg(uint8_t reg, uint8_t value1, uint8_t value2)
{
    i2cStart();
    Wire.beginTransmission(IST9201_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value1);
    Wire.write(value2);
    Wire.endTransmission();
    i2cEnd();
}
