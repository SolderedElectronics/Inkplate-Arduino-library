/**
 **************************************************
 * @file        ImageDither.cpp
 * @brief       dither functionalities for images
 * 
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example.
 *              If you have any questions about licensing, please contact techsupport@e-radionica.com
 *              Distributed as-is; no warranty is given.
 * 
 * @authors     e-radionica.com
 ***************************************************/

#include "Image.h"

#ifndef ARDUINO_INKPLATECOLOR

/**
 * @brief       ditherGetPixelBmp calculates dither for given pixel in bmp images
 * 
 * @param       uint32_t px
 *              pixel value with color information
 * @param       int i
 *              ditherBuffer width plane position
 * @param       int w
 *              image width
 * @param       bool paletted
 *              1 if paletted image, 0 if not
 * 
 * @return      new pixel value (dithered pixel)
 */
uint8_t Image::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
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

/**
 * @brief       ditherGetPixelJpeg calculates dither for given pixel in jpeg images
 * 
 * @param       uint8_t px
 *              pixel value with color information
 * @param       int i
 *              ditherBuffer width plane position
 * @param       int j
 *              ditherBuffer height plane position
 * @param       int x
 *              x image starting position
 * @param       int y
 *              y image starting position
 * @param       int w
 *              image width
 * @param       int h
 *              image height
 * 
 * @return      new pixel value (dithered pixel)
 */
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

/**
 * @brief       ditherSwap swaps ditherBuffer values
 * 
 * @param       int w
 *              screen width
 */
void Image::ditherSwap(int w)
{
    for (int i = 0; i < w; ++i)
    {
        ditherBuffer[0][i] = ditherBuffer[1][i];
        ditherBuffer[1][i] = 0;
    }
}

/**
 * @brief       ditherSwapBlockJpeg swaps ditherBuffer values
 * 
 * @param       int x
 *              x plane image starting point
 */
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