#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "../libs/SdFat/SdFat.h"
#include "Arduino.h"
#include "SPI.h"

#include "Esp.h"
#include "Mcp.h"
#include "Network.h"

class System : public Esp, public Mcp, virtual public Network
{
  public:
    void setPanelState(uint8_t s);
    uint8_t getPanelState();

    void setSdCardOk(int16_t s);
    int16_t getSdCardOk();

    void setTemperature(int8_t t);
    int8_t readTemperature();

    uint8_t readTouchpad(uint8_t _pad);
    double readBattery();

    int16_t sdCardInit();

    SdFat getSdFat();
    SPIClass getSPI();

  private:
    uint8_t _panelOn = 0;
    int8_t _temperature;
    int16_t _sdCardOk = 0;
};

#endif