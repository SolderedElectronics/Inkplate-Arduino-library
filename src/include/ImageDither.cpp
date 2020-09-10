#include "Image.h"

uint8_t Image::ditherGetPixelBmp(uint8_t px, int i, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    uint8_t oldPixel = min((uint16_t)0xFF, (uint16_t)((uint16_t)ditherBuffer[0][i] + px));

    uint8_t newPixel = oldPixel & (getDisplayMode() == INKPLATE_1BIT ? B10000000 : B11100000);
    uint8_t quantError = oldPixel - newPixel;

    ditherBuffer[1][i + 0] += (quantError * 5) >> 4;
    if (i != w - 1)
    {
        ditherBuffer[0][i + 1] += (quantError * 7) >> 4;
        ditherBuffer[1][i + 1] += (quantError * 1) >> 4;
    }
    if (i != 0)
        ditherBuffer[1][i - 1] += (quantError * 3) >> 4;

    return newPixel >> 5;
}

uint8_t Image::ditherGetPixelJpeg(uint8_t px, int x, int y, int w)
{
    uint8_t oldPixel = min((uint16_t)0xFF, (uint16_t)((uint16_t)ditherBuffer[0][x] + px));

    uint8_t newPixel = oldPixel & (getDisplayMode() == INKPLATE_1BIT ? B10000000 : B11100000);
    uint8_t quantError = oldPixel - newPixel;

    ditherBuffer[1][x + 0] += (quantError * 5) >> 4;
    if (x != w - 1)
    {
        ditherBuffer[0][x + 1] += (quantError * 7) >> 4;
        ditherBuffer[1][x + 1] += (quantError * 1) >> 4;
    }
    if (x != 0)
        ditherBuffer[1][x - 1] += (quantError * 3) >> 4;

    return newPixel >> 5;
}

void Image::ditherSwap(int w)
{
    for (int i = 0; i < w; ++i)
    {
        ditherBuffer[0][i] = ditherBuffer[1][i];
        ditherBuffer[1][i] = 0;
    }
}