/**
 **************************************************
 * @file        ImageDitherColor.cpp
 * @brief       dither functionalities for colored images
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact assistance@soldered.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
#pragma once
#if defined(ARDUINO_INKPLATE13SPECTRA) || defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_INKPLATECOLOR) ||            \
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
uint8_t ImageColor::findClosestPalette(int16_t r, int16_t g, int16_t b)
{
    int64_t minDistance = INT64_MAX;
    uint8_t contenderCount = 0;
    uint8_t contenderList[sizeof pallete / sizeof pallete[0]];

    for (int i = 0; i < sizeof pallete / sizeof pallete[0]; ++i)
    {
        int16_t pr = RED8(pallete[i]);
        int16_t pg = GREEN8(pallete[i]);
        int16_t pb = BLUE8(pallete[i]);

        int32_t dr = r - pr;
        int32_t dg = g - pg;
        int32_t db = b - pb;

        int32_t currentDistance = dr * dr + dg * dg + db * db;

        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            contenderList[0] = i;
            contenderCount = 1;
        }
        else if (currentDistance == minDistance)
        {
            if (contenderCount < sizeof pallete / sizeof pallete[0])
                contenderList[contenderCount++] = i;
        }
    }

    return contenderList[0];
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

    int closest = findClosestPalette(r, g, b);

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
