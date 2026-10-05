#ifndef __TPS65186_H__
#define __TPS65186_H__

#include "Arduino.h"
#include "Wire.h"
#include "../../system/ExpanderSelect.h"
#include "../../system/defines.h"

// Boards may override this from their pins.h, which is pulled in through the
// driver header before this file.
#ifndef TPS65186_I2C_ADDR
#define TPS65186_I2C_ADDR 0x48
#endif

#define TPS65186_REG_TEMP    0x00
#define TPS65186_REG_ENABLE  0x01
#define TPS65186_REG_VCOML   0x03
#define TPS65186_REG_VCOMH   0x04
#define TPS65186_REG_UPSEQ0  0x09
#define TPS65186_REG_UPSEQ1  0x0A
#define TPS65186_REG_DWNSEQ0 0x0B
#define TPS65186_REG_DWNSEQ1 0x0C
#define TPS65186_REG_TMST1   0x0D
#define TPS65186_REG_PWRGOOD 0x0F

class TPS65186
{
  public:
    void begin(IOExpander *expander, uint8_t wakeupPin, uint8_t pwrupPin, uint8_t vcomPin);
    bool enableRails(bool enable);
    bool powerUp(uint16_t timeout = 250);
    void powerDown();
    int8_t readTemperature();
    bool isPowerGood();
    uint8_t readPowerGood();
    void writeReg(uint8_t reg, uint8_t data);
    uint8_t readReg(uint8_t reg);

  private:
    IOExpander *_expander = nullptr;
    uint8_t _wakeupPin = 0;
    uint8_t _pwrupPin = 0;
    uint8_t _vcomPin = 0;
    bool _poweredUp = false;
};

#endif
