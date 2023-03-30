/**
 **************************************************
 * @file        ImageDitherColor.cpp
 * @brief       dither functionalities for colored images
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#include "Image.h"

#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4)

extern Image *_imagePtrJpeg;
/*
static uint32_t pallete[] = {
    0x000000ll, 0xFFFFFFll, 0x008000ll, 0x0000FFll, 0xFF0000ll, 0xFFFF00ll,
0xFFAA00ll,
};
*/

#if defined(ARDUINO_INKPLATECOLOR)

static uint32_t pallete[] = {0x000000, 0xFFFFFF, 0x438A1C, 0x555E7E, 0x8A4C5B, 0xFFF338, 0xE87E00, 0xC2A4F4};
static unsigned int width = E_INK_WIDTH, height = E_INK_HEIGHT;

#elif defined(ARDUINO_INKPLATE2)

static uint32_t pallete[] = {0xFFFFFF, 0x000000, 0xFF0000};
static unsigned int width = E_INK_HEIGHT, height = E_INK_WIDTH;

#elif defined(ARDUINO_INKPLATE4)

static uint32_t pallete[] = {0xFFFFFF, 0x000000, 0xFF0000};
static unsigned int width = E_INK_WIDTH, height = E_INK_HEIGHT;

#endif

/**
 * @brief       findClosestPalette return closes pallete for given pixel
 *
 * @param       uint32_t c
 *              color of the given pixel
 *
 * @return      closest color in pallete array
 */
uint8_t Image::findClosestPalette(uint32_t c)
{
    int mi = 0;
    for (int i = 1; i < sizeof pallete / sizeof pallete[0]; ++i)
    {
        if (COLORDISTSQR(c, pallete[i]) < COLORDISTSQR(c, pallete[mi]))
            mi = i;
    }

    return mi;
}

/**
 * @brief       ditherGetPixelBmp finds dithered value for given pixel
 *
 * @param       uint32_t px
 *              pixel to find value for
 * @param       int i
 *              x plane pixel position
 * @param       int j
 *              y plane pixel position
 * @param       int w
 *              image width
 * @param       bool paletted
 *              1 if palleted image, 0 if not
 *
 * @return      new dithered pixel
 */
uint8_t Image::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    int16_t r = RED8(px) + ditherBuffer[0][j % 15][i];
    int16_t g = GREEN8(px) + ditherBuffer[1][j % 15][i];
    int16_t b = BLUE8(px) + ditherBuffer[2][j % 15][i];

    ditherBuffer[0][j % 15][i] = 0;
    ditherBuffer[1][j % 15][i] = 0;
    ditherBuffer[2][j % 15][i] = 0;

    r = max((int16_t)0, min((int16_t)255, r));
    g = max((int16_t)0, min((int16_t)255, g));
    b = max((int16_t)0, min((int16_t)255, b));

    int closest = findClosestPalette(((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b));

    int32_t rErr = r - (int32_t)((pallete[closest] >> 16) & 0xFF);
    int32_t gErr = g - (int32_t)((pallete[closest] >> 8) & 0xFF);
    int32_t bErr = b - (int32_t)((pallete[closest] >> 0) & 0xFF);

    for (int k = 0; k < kernelHeight; ++k)
    {
        for (int l = -kernelX; l < kernelWidth - kernelX; ++l)
        {
            if (!(0 <= i + l && i + l < w))
                continue;
            ditherBuffer[0][(j + k) % 15][i + l] += (kernel[k][l + kernelX] * rErr) / coef;
            ditherBuffer[1][(j + k) % 15][i + l] += (kernel[k][l + kernelX] * gErr) / coef;
            ditherBuffer[2][(j + k) % 15][i + l] += (kernel[k][l + kernelX] * bErr) / coef;
        }
    }

    return closest;
}

/**
 * @brief       ditherGetPixelJpeg finds dithered value for given pixel
 *
 * @param       uint32_t px
 *              pixel to find value for
 * @param       int i
 *              x plane pixel position
 * @param       int j
 *              y plane pixel position
 * @param       int w
 *              image width
 * @param       bool paletted
 *              1 if palleted image, 0 if not
 *
 * @note        currently not used
 */
uint8_t Image::ditherGetPixelJpeg(uint8_t px, int i, int j, int x, int y, int w, int h)
{
    // Not used
    return 0;
}

/**
 * @brief       ditherSwap
 *
 * @note        not used
 */
void Image::ditherSwap(int)
{
    // Not used
}

/**
 * @brief       ditherSwapBlockJpeg
 *
 * @note        not used
 */
void Image::ditherSwapBlockJpeg(int x)
{
    // Not used
}

#endif
