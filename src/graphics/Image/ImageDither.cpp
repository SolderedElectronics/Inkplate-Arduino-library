/**
 **************************************************
 * @file        ImageDither.cpp
 * @brief       dither functionalities for images
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
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&           \
    !defined(ARDUINO_ESP32S3_DEV)
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

    const int rowIdx = j & ditherRowMask;
    int16_t *row = ditherBuffer[rowIdx];

    int16_t oldPixel = (int16_t)px + row[i];
    row[i] = 0; // clear after reading

    oldPixel = max((int16_t)0, min((int16_t)255, oldPixel));

    // 1-bit: snap to 0x00 or 0xFF so that >>5 produces 0 or 7 (full display range).
    // 3-bit: quantise to the top 3 bits; >>5 produces 0-7.
    uint8_t newPixel = (_inkplate->getDisplayMode() == INKPLATE_1BIT) ? ((oldPixel >= 128) ? 0xFF : 0x00)
                                                                      : ((uint8_t)oldPixel & B11100000);
    int16_t quantError = oldPixel - newPixel;

    const int minOffset = max(-currentKernel->x, -i);
    const int maxOffset = min(currentKernel->width - currentKernel->x - 1, w - 1 - i);

    for (int k = 0; k < currentKernel->height; ++k)
    {
        int16_t *nextRow = ditherBuffer[(rowIdx + k) & ditherRowMask];
        for (int l = minOffset; l <= maxOffset; ++l)
        {
            const int weight = currentKernel->data[k * currentKernel->width + (l + currentKernel->x)];
            if (!weight)
                continue;
            nextRow[i + l] += (weight * quantError) / currentKernel->coef;
        }
    }

    return newPixel >> 5;
}

#endif
