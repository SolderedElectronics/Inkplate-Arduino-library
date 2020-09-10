#include "GFX.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

GFX::GFX(int16_t w, int16_t h)
{
    _width = WIDTH = w;
    _height = HEIGHT = h;
    setRotation(0);
    setCursor(0, 0);
    setTextSize(1);
    setTextColor(0xFFFF, 0xFFFF);
    setTextWrap(true);
    cp437(false);
    gfxFont = NULL;
}

void GFX::setRotation(uint8_t x)
{
    rotation = (x & 3);
    switch (rotation)
    {
    case 0:
    case 2:
        _width = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width = HEIGHT;
        _height = WIDTH;
        break;
    }
}

uint8_t GFX::getRotation()
{
    return rotation;
}

void GFX::drawPixel(int16_t x0, int16_t y0, uint16_t color)
{
    writePixel(x0, y0, color);
}

void GFX::startWrite()
{
}

void GFX::writePixel(int16_t x0, int16_t y0, uint16_t color)
{
    if (x0 > width() - 1 || y0 > height() - 1 || x0 < 0 || y0 < 0)
        return;

    switch (rotation)
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = width() - x0 - 1;
        break;
    case 2:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    if (getDisplayMode() == 0)
    {
        int x = x0 >> 3;
        int x_sub = x0 & 7;
        uint8_t temp = *(_partial + 100 * y0 + x);
        *(_partial + 100 * y0 + x) = ~pixelMaskLUT[x_sub] & temp | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        color &= 7;
        int x = x0 >> 1;
        int x_sub = x0 & 1;
        uint8_t temp;
        temp = *(D_memory4Bit + 400 * y0 + x);
        *(D_memory4Bit + 400 * y0 + x) = pixelMaskGLUT[x_sub] & temp | (x_sub ? color : color << 4);
    }
}

void GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            writePixel(x + j, y + i, color);
}

void GFX::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        writePixel(x, y + i, color);
}

void GFX::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    for (int j = 0; j < w; ++j)
        writePixel(x + j, y, color);
}

void GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx >> 1;
    int16_t ystep;

    if (y0 < y1)
        ystep = 1;
    else
        ystep = -1;

    for (; x0 <= x1; x0++)
    {
        if (steep)
            writePixel(y0, x0, color);
        else
            writePixel(x0, y0, color);
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

void GFX::endWrite()
{
}

void GFX::setDisplayMode(uint8_t _mode)
{
    _displayMode = _mode;
}

void GFX::selectDisplayMode(uint8_t _mode)
{
    if (_mode != _displayMode)
    {
        _displayMode = _mode & 1;
        memset(DMemoryNew, 0, 60000);
        memset(_partial, 0, 60000);
        memset(_pBuffer, 0, 120000);
        memset(D_memory4Bit, 255, 240000);
        _blockPartial = 1;
    }
}

uint8_t GFX::getDisplayMode()
{
    return _displayMode;
}

int16_t GFX::width()
{
    return _width;
};

int16_t GFX::height()
{
    return _height;
};