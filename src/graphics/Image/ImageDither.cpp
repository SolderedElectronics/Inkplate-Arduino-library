/**
 **************************************************
 * @file        ImageDither.cpp
 * @brief       dither functionalities for images
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
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&           \
    !defined(ARDUINO_INKPLATE7SPECTRA)
#include "Inkplate.h"
#include "Image.h"


/**
 * @brief       ditherGetPixelBmp calculates dither for given pixel using the
 *              currently selected kernel. The dither buffer is a circular
 *              buffer indexed by j & ditherRowMask so error propagates
 *              naturally across row and block boundaries without any swap step.
 *
 * @param       uint32_t px
 *              pixel value (8-bit grayscale or palette index)
 * @param       int i
 *              absolute column position
 * @param       int j
 *              absolute row position
 * @param       int w
 *              image width (used for error-diffusion boundary clamping)
 * @param       bool paletted
 *              true if px is a palette index that should be looked up first
 *
 * @return      new dithered pixel value (3-bit grayscale, 0-7)
 */
uint8_t Image::ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted)
{
    if (paletted)
        px = ditherPalette[px];

    // Columns past the error buffer are off screen, quantise them without diffusing any error.
    if (i < 0 || i >= ditherRowWidth)
    {
        const int16_t clamped = max((int16_t)0, min((int16_t)255, (int16_t)px));
        if (_inkplate->getDisplayMode() == INKPLATE_1BIT)
            return (clamped >= 128) ? 7 : 0;
        return (uint8_t)(((int)clamped * 7 + 127) / 255);
    }

    // Keep the taps inside the error row.
    if (w > ditherRowWidth)
        w = ditherRowWidth;

    const int rowIdx = j & ditherRowMask;
    int16_t *row = ditherBuffer[rowIdx];

    int16_t oldPixel = (int16_t)px + row[i];
    row[i] = 0; // clear after reading

    oldPixel = max((int16_t)0, min((int16_t)255, oldPixel));

    // Round to the nearest level and measure the error against the grey it really shows,
    // level k is displayed as k * 255 / 7 and not as k * 32.
    uint8_t newLevel;
    int16_t quantValue;
    if (_inkplate->getDisplayMode() == INKPLATE_1BIT)
    {
        newLevel = (oldPixel >= 128) ? 7 : 0;
        quantValue = (oldPixel >= 128) ? 255 : 0;
    }
    else
    {
        newLevel = (uint8_t)(((int)oldPixel * 7 + 127) / 255);
        quantValue = (int16_t)(((int)newLevel * 255 + 3) / 7);
    }
    const int16_t quantError = oldPixel - quantValue;

    const int minOffset = max(-currentKernel->x, -i);
    const int maxOffset = min(currentKernel->width - currentKernel->x - 1, w - 1 - i);
    const int halfCoef = currentKernel->coef / 2;

    for (int k = 0; k < currentKernel->height; ++k)
    {
        int16_t *nextRow = ditherBuffer[(rowIdx + k) & ditherRowMask];
        for (int l = minOffset; l <= maxOffset; ++l)
        {
            const int weight = currentKernel->data[k * currentKernel->width + (l + currentKernel->x)];
            if (!weight)
                continue;

            // Round the share, truncating it towards zero loses most of a small error.
            const int numerator = weight * quantError;
            const int share = (numerator >= 0) ? ((numerator + halfCoef) / currentKernel->coef)
                                               : -((-numerator + halfCoef) / currentKernel->coef);
            nextRow[i + l] += (int16_t)share;
        }
    }

    return newLevel;
}

#endif
