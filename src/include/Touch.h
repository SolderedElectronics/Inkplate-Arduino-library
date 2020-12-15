#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "Arduino.h"
#include "Graphics.h"
#include "Mcp.h"
#include "Wire.h"
#include "defines.h"

class Touch : virtual public Mcp
{
  public:
    bool tsInit(uint8_t _pwrState);
    void tsShutdown();
    bool tsAvailable();
    void tsSetPowerState(uint8_t _s);
    uint8_t tsGetPowerState();
    uint8_t tsGetData(uint16_t *xPos, uint16_t *yPos);

    virtual int getRotation() = 0;

  private:
    const char hello_packet[4] = {0x55, 0x55, 0x55, 0x55};
    uint16_t _tsXResolution;
    uint16_t _tsYResolution;

    uint8_t tsWriteRegs(uint8_t _addr, const uint8_t *_buff, uint8_t _size);
    void tsReadRegs(uint8_t _addr, uint8_t *_buff, uint8_t _size);
    void tsHardwareReset();
    bool tsSoftwareReset();
    void tsGetRawData(uint8_t *b);
    void tsGetXY(uint8_t *_d, uint16_t *x, uint16_t *y);
    void tsGetResolution(uint16_t *xRes, uint16_t *yRes);
};

#endif