/**
 **************************************************
 * @file        ImageDitherColor.cpp
 * @brief       dither functionalities for colored images
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
#pragma once
#if defined(ARDUINO_INKPLATE13SPECTRA) || defined(ARDUINO_INKPLATE7SPECTRA) || defined(ARDUINO_INKPLATECOLOR) ||       \
    defined(ARDUINO_INKPLATE2)
#include "Inkplate.h"
#include "ImageColor.h"

extern ImageColor *_imagePtrJpeg;

/**
 * @brief       findClosestPalette return closes pallete for given pixel
 *
 * @param       uint32_t c
 *              color of the given pixel
 *
 * @return      closest color in pallete array
 */
uint8_t ImageColor::findClosestPalette(int16_t r, int16_t g, int16_t b, uint8_t tieRotate)
{
    constexpr uint8_t paletteSize = sizeof pallete / sizeof pallete[0];

    int32_t minDistance = INT32_MAX;
    uint8_t tieCount = 0;
    uint8_t ties[paletteSize];

    for (uint8_t i = 0; i < paletteSize; ++i)
    {
        int32_t dr = r - (int16_t)RED8(pallete[i]);
        int32_t dg = g - (int16_t)GREEN8(pallete[i]);
        int32_t db = b - (int16_t)BLUE8(pallete[i]);

        int32_t currentDistance = dr * dr + dg * dg + db * db;

        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            ties[0] = i;
            tieCount = 1;
        }
        else if (currentDistance == minDistance)
        {
            ties[tieCount++] = i;
        }
    }

    // Out of gamut colours tie between several inks (cyan: white, green, blue). Rotating through
    // the tied inks by position mixes them instead of painting the area one flat colour.
    return ties[tieRotate % tieCount];
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
uint8_t ImageColor::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    // Columns past the error buffer are off screen, match them without diffusing any error.
    if (i < 0 || i >= (int)ditherBufferWidth)
        return findClosestPalette(RED8(px), GREEN8(px), BLUE8(px), (uint8_t)(i + j));

    // Keep the taps inside the error row.
    if (w > (int)ditherBufferWidth)
        w = (int)ditherBufferWidth;

    const int rowIdx = j & ditherRowMask;
    int16_t *rowR = ditherBuffer[0][rowIdx];
    int16_t *rowG = ditherBuffer[1][rowIdx];
    int16_t *rowB = ditherBuffer[2][rowIdx];

    int16_t r = RED8(px) + rowR[i];
    int16_t g = GREEN8(px) + rowG[i];
    int16_t b = BLUE8(px) + rowB[i];

    rowR[i] = 0;
    rowG[i] = 0;
    rowB[i] = 0;

    r = max((int16_t)0, min((int16_t)255, r));
    g = max((int16_t)0, min((int16_t)255, g));
    b = max((int16_t)0, min((int16_t)255, b));

    int closest = findClosestPalette(r, g, b, (uint8_t)(i + j));

    int32_t rErr = r - (int32_t)((pallete[closest] >> 16) & 0xFF);
    int32_t gErr = g - (int32_t)((pallete[closest] >> 8) & 0xFF);
    int32_t bErr = b - (int32_t)((pallete[closest] >> 0) & 0xFF);


    const int minOffset = max(-currentKernel->x, -i);
    const int maxOffset = min(currentKernel->width - currentKernel->x - 1, w - 1 - i);
    for (int k = 0; k < currentKernel->height; ++k)
    {
        const int nextRowIdx = (rowIdx + k) & ditherRowMask;
        int16_t *nextRowR = ditherBuffer[0][nextRowIdx];
        int16_t *nextRowG = ditherBuffer[1][nextRowIdx];
        int16_t *nextRowB = ditherBuffer[2][nextRowIdx];
        for (int l = minOffset; l <= maxOffset; ++l)
        {
            const int weight = currentKernel->data[k * currentKernel->width + (l + currentKernel->x)];
            if (!weight)
                continue;
            const int idx = i + l;
            nextRowR[idx] += (weight * rErr) / currentKernel->coef;
            nextRowG[idx] += (weight * gErr) / currentKernel->coef;
            nextRowB[idx] += (weight * bErr) / currentKernel->coef;
        }
    }

    return closest;
}


#endif
