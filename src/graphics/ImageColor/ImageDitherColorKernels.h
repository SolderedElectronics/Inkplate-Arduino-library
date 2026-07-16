/**
 **************************************************
 * @file        ImageDitherColorKernels.h
 * @brief       color kernels for dither
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
#if defined(ARDUINO_INKPLATE13SPECTRA) || defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) ||              \
    defined(ARDUINO_INKPLATE31SPECTRA)

struct DitherKernelDef
{
    uint8_t width;
    uint8_t height;
    uint8_t x;
    uint16_t coef;
    const unsigned char *data;
};

// Floyd Steinberg
static const unsigned char _kernelFloydSteinberg[] = {
    0, 0, 7, 3, 5, 1,
};

// J F Jarvis, C N Judice, and W H Ninke "Minimized Average Error"
static const unsigned char _kernelJarvisJudiceNinke[] = {
    0, 0, 0, 7, 5, 3, 5, 7, 5, 3, 1, 3, 5, 3, 1,
};

// Atkinson
static const unsigned char _kernelAtkinson[] = {
    0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0,
};

// Burkes
static const unsigned char _kernelBurkes[] = {
    0, 0, 0, 8, 4, 2, 4, 8, 4, 2, 0, 0, 0, 0, 0,
};

// Stucki
static const unsigned char _kernelStucki[] = {
    0, 0, 0, 8, 4, 2, 4, 8, 4, 2, 1, 2, 4, 2, 1,
};

// Sierra lite
static const unsigned char _kernelSierraLite[] = {
    0, 0, 2, 1, 1, 0, 0, 0, 0,
};

static const unsigned char _kernelReducedDiffusion[] = {
    0, 0, 5, 2, 3, 1,
};

static const DitherKernelDef DITHER_KERNELS[] = {
    {3, 2, 1, 16, _kernelFloydSteinberg},   {5, 3, 2, 48, _kernelJarvisJudiceNinke},
    {4, 3, 1, 8, _kernelAtkinson},          {5, 3, 2, 32, _kernelBurkes},
    {5, 3, 2, 42, _kernelStucki},           {3, 3, 1, 4, _kernelSierraLite},
    {3, 2, 1, 26, _kernelReducedDiffusion},
};

static const uint8_t DITHER_KERNEL_COUNT = sizeof DITHER_KERNELS / sizeof DITHER_KERNELS[0];

#endif