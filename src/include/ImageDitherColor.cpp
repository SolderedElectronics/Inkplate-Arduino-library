#include "Image.h"
#include "ImageDitherColorKernels.h"

#ifdef ARDUINO_INKPLATECOLOR
int kernelWidth = sizeof kernel[0] / sizeof kernel[0][0];
int kernelHeight = sizeof kernel / sizeof kernel[0];

static unsigned int pallete[] = {
    0x000000, 0xFFFFFF, 0x00FF00, 0x0000FF, 0xFF0000, 0xFFFF00, 0xFF8000,
};

static unsigned int width = E_INK_WIDTH, height = E_INK_HEIGHT;

uint8_t Image::findClosestPalette(uint32_t c)
{
    int mi = 0;
    for (int i = 1; i < sizeof pallete[0] / sizeof pallete; ++i)
        if (COLORDISTSQR(c, pallete[i]) < COLORDISTSQR(c, pallete[mi]))
            mi = i;
    return mi;
}

uint8_t Image::ditherGetPixelBmp(uint8_t px, int i, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    if (getDisplayMode() == INKPLATE_1BIT)
        px = (uint16_t)px >> 1;

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

uint8_t Image::ditherGetPixelJpeg(uint8_t px, int i, int j, int x, int y, int w, int h)
{
    if (blockW == -1)
    {
        blockW = w;
        blockH = h;
    }

    if (getDisplayMode() == INKPLATE_1BIT)
        px = (uint16_t)px >> 1;

    uint16_t oldPixel = min((uint16_t)0xFF, (uint16_t)((uint16_t)px + (uint16_t)jpegDitherBuffer[j + 1][i + 1] +
                                                       (j ? (uint16_t)0 : (uint16_t)ditherBuffer[0][x + i])));

    uint8_t newPixel = oldPixel & (getDisplayMode() == INKPLATE_1BIT ? B10000000 : B11100000);
    uint8_t quantError = oldPixel - newPixel;

    jpegDitherBuffer[j + 1 + 1][i + 0 + 1] += (quantError * 5) >> 4;

    jpegDitherBuffer[j + 0 + 1][i + 1 + 1] += (quantError * 7) >> 4;
    jpegDitherBuffer[j + 1 + 1][i + 1 + 1] += (quantError * 1) >> 4;

    jpegDitherBuffer[j + 1 + 1][i - 1 + 1] += (quantError * 3) >> 4;

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

void Image::ditherSwapBlockJpeg(int x)
{
    for (int i = 0; i < 18; ++i)
    {
        if (x + i)
            ditherBuffer[1][x + i - 1] += jpegDitherBuffer[blockH - 1 + 2][i];
        jpegDitherBuffer[i][0 + 1] = jpegDitherBuffer[i][blockW - 1 + 2];
    }
    for (int j = 0; j < 18; ++j)
        for (int i = 0; i < 18; ++i)
            if (i != 1)
                jpegDitherBuffer[j][i] = 0;

    jpegDitherBuffer[17][1] = 0;
}

#endif