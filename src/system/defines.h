/**
 **************************************************
 * @file        deines.h
 * @brief       Various defines/macros used for inkplate control
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

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "Arduino.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#define INKPLATE6_WAVEFORM1     0
#define INKPLATE10_WAVEFORM1    20
#define INKPLATE10_WAVEFORM2    21
#define INKPLATE10_WAVEFORM3    22
#define INKPLATE10_WAVEFORM4    23
#define INKPLATE10_WAVEFORM5    24
#define INKPLATE6PLUS_WAVEFORM1 40

#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&           \
    !defined(ARDUINO_INKPLATE31SPECTRA)
#define BLACK 1
#define WHITE 0
#else
#define BLACK 0
#define WHITE 1
#endif

#define INKPLATE_1BIT          0
#define INKPLATE_3BIT          1
#define PWR_GOOD_OK            0b11111010
#define INKPLATE_FORCE_PARTIAL true

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

#define BOUND(a, b, c) ((a) <= (b) && (b) <= (c))

#define RGB3BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 13)
#define RGB8BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 8)

#define READ32(c)     (uint32_t)(*(c) | (*((c) + 1) << 8) | (*((c) + 2) << 16) | (*((c) + 3) << 24))
#define READ16(c)     (uint16_t)(*(c) | (*((c) + 1) << 8))
#define ROWSIZE(w, c) (((int16_t)c * w + 31) >> 5) << 2

#define _RED(a)   ((((a)&0xf800) >> 11) << 3)
#define _GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define _BLUE(a)  (((a)&0x001f) << 3)


#define RED8(a)   (((a) >> 16) & 0xff)
#define GREEN8(a) (((a) >> 8) & 0xff)
#define BLUE8(a)  (((a)) & 0xff)


#define GPIO0_ENABLE 8

#define DATA 0x0E8C0030

#define SQR(a)             ((int32_t)(a) * (int32_t)(a))
#define COLORDISTSQR(x, y) (SQR(RED8(x) - RED8(y)) + SQR(GREEN8(x) - GREEN8(y)) + SQR(BLUE8(x) - BLUE8(y)))

#endif
