#ifndef __IST9201_H__
#define __IST9201_H__

#include "Arduino.h"
#include "Wire.h"
#include "../../system/pcalExpander/pcalExpander.h"

#define IST9201_I2C_ADDR 0x48

// IST9201 register addresses (from vendor pmic.c/pmic.h)
#define IST9201_REG_VPOS1          0x01
#define IST9201_REG_VNEG1          0x03
#define IST9201_REG_VPOS2          0x05
#define IST9201_REG_VNEG2          0x07
#define IST9201_REG_VPOS3          0x09
#define IST9201_REG_VNEG3          0x0B
#define IST9201_REG_VCOMDC         0x0D
#define IST9201_REG_VCOMH          0x0F
#define IST9201_REG_VCOML          0x11
#define IST9201_REG_DELAY1         0x13
#define IST9201_REG_DELAY2         0x14
#define IST9201_REG_VDDH_EXT_DELAY 0x15
#define IST9201_REG_VGH1           0x16
#define IST9201_REG_VGH2           0x18
#define IST9201_REG_0x1A           0x1A
#define IST9201_REG_0x1B           0x1B
#define IST9201_REG_0x1C           0x1C

/**
 * @brief       Driver for the IST9201 PMIC used on the Inkplate 31SPECTRA to generate
 *              the panel bias voltages (VPOS/VNEG/VCOM/VGH ladders).
 *
 *              Ported from E Ink's vendor sample code (pmic.c/pmic.h, SampleCodeFor315).
 *              Register addresses and the initPmicData magic values are used verbatim
 *              from that sample - see src/boards/Inkplate31SPECTRA/CLAUDE.md for the
 *              full reasoning behind the enable sequencing used here.
 */
class IST9201
{
  public:
    void begin(IOExpander *expander, uint8_t pwrEnPin, uint8_t chipEnPin, uint8_t vddpPin, uint8_t vddnPin,
               uint8_t vcnpPin, uint8_t toggleSel0Pin, uint8_t toggleSel1Pin, uint8_t pgPin, uint8_t psPin);

    bool setRegisters();
    bool powerUp(uint16_t timeoutMs = 200);
    void powerDown();
    void railsOff();

    bool isPowerGood();
    void setVcomFromPanel(uint8_t *panelReadback);

    void writeReg(uint8_t reg, uint8_t value1);
    void writeReg(uint8_t reg, uint8_t value1, uint8_t value2);

    // Register init values, in the same order/packing as the vendor's initPmicData[28].
    uint8_t initPmicData[28] = {
        // VPOS1
        0x02, 0xE3,
        // VNEG1
        0x02, 0xE3,
        // VPOS2
        0x01, 0x33,
        // VNEG2
        0x01, 0x6C,
        // VPOS3
        0x01, 0x34,
        // VNEG3
        0x00, 0xA2,
        // DC VCOM (AC VCOM variant used - see vendor comment)
        0x30, 0x9A,
        // VCOMH
        0x21, 0x33,
        // VCOML
        0x00, 0xEA,
        // Delay Time 1
        0xAA,
        // Delay Time 2
        0xAA,
        // VDDH_EXT delay time
        0xA0,
        // VGH1
        0xD8, 0xBC,
        // VGH2
        0x01, 0x94,
        // 0x1A (VPDD_LEN, XON discharge 5s+2s)
        0x6B,
        // 0x1B
        0x80,
        // 0x1C
        0x00};

  private:
    IOExpander *_expander = nullptr;
    uint8_t _pwrEnPin = 0;
    uint8_t _chipEnPin = 0;
    uint8_t _vddpPin = 0;
    uint8_t _vddnPin = 0;
    uint8_t _vcnpPin = 0;
    uint8_t _toggleSel0Pin = 0;
    uint8_t _toggleSel1Pin = 0;
    uint8_t _pgPin = 0;
    uint8_t _psPin = 0;

    bool _pwrRailEnabled = false;
    bool _poweredUp = false;

    void powerSwitchEnable();
};

#endif
