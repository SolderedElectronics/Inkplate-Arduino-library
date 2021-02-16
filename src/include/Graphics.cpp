/*
Graphics.cpp
Inkplate Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
February 12, 2021
https://github.com/e-radionicacom/Inkplate-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Graphics.h"

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

void Graphics::setRotation(uint8_t x)
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

uint8_t Graphics::getRotation()
{
    return rotation;
}

void Graphics::drawPixel(int16_t x0, int16_t y0, uint16_t color)
{
    writePixel(x0, y0, color); // Specified in boards folder
}

void Graphics::startWrite()
{
}

void Graphics::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            writePixel(x + j, y + i, color);
}

void Graphics::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    for (int i = 0; i < h; ++i)
        writePixel(x, y + i, color);
}

void Graphics::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    for (int j = 0; j < w; ++j)
        writePixel(x + j, y, color);
}

void Graphics::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
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

void Graphics::endWrite()
{
}

#ifndef ARDUINO_INKPLATECOLOR
void Graphics::setDisplayMode(uint8_t _mode)
{
    _displayMode = _mode;
}

uint8_t Graphics::getDisplayMode()
{
    return _displayMode;
}
#endif

void Graphics::selectDisplayMode(uint8_t _mode)
{
    if (_mode != _displayMode)
    {
        _displayMode = _mode & 1;
        memset(DMemoryNew, 0, 60000);
        memset(_partial, 0, 60000);
        memset(_pBuffer, 0, 120000);
        memset(DMemory4Bit, 255, 240000);
        _blockPartial = 1;
    }
}

int16_t Graphics::width()
{
    return _width;
};

int16_t Graphics::height()
{
    return _height;
};