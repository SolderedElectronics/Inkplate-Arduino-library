#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "../libs/Adafruit-GFX-Library/Adafruit_GFX.h"
#include "Arduino.h"
#include "Triangulate.h"

class Shapes : virtual public Adafruit_GFX
{
  public:
    Shapes(int16_t w, int16_t h) : Adafruit_GFX(w, h){};

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    virtual void selectDisplayMode(uint8_t _mode) = 0;
    virtual uint8_t getDisplayMode() = 0;

    void drawElipse(int rx, int ry, int xc, int yc, int c);
    void fillElipse(int rx, int ry, int xc, int yc, int c);
    void drawPolygon(int *x, int *y, int n, int color);
    void fillPolygon(int *x, int *y, int n, int color);
    void drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness);
    void drawGradientLine(int x1, int y1, int x2, int y2, int color1, int color2, float thickness = -1);

  private:
    Triangulate triangulate;

    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;
};

#endif