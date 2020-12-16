#include "Backlight.h"
#include "defines.h"

#ifdef ARDUINO_INKPLATE6PLUS

void Backlight::setBacklight(uint8_t _v)
{
    Wire.beginTransmission(0x5C >> 1);
    Wire.write(0);
    Wire.write(63 - (_v & 0b00111111));
    Serial.println(Wire.endTransmission());
}

void Backlight::backlight(bool _e)
{
    if (_e)
    {
        pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, OUTPUT);
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, HIGH);
    }
    else
    {
        pinModeInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, OUTPUT);
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, BACKLIGHT_EN, LOW);
    }
}

#endif