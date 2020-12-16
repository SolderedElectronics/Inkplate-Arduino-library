#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#ifdef ARDUINO_INKPLATE6PLUS

#include "Arduino.h"
#include "Mcp.h"
#include "Wire.h"
#include "defines.h"

class Backlight : virtual public Mcp
{
  public:
    void setBacklight(uint8_t _v);
    void backlight(bool _e);
};

#endif

#endif