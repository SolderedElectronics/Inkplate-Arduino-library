/**
 **************************************************
 * @file        deines.h
 * @brief       Various defines/macros used for inkplate control
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

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "Arduino.h"

#if (defined(ARDUINO_ESP32_DEV) || defined(ARDUINO_INKPLATE6V2))
#include "../boards/Inkplate6.h"
#elif ARDUINO_INKPLATE5
#include "../boards/Inkplate5.h"
#elif (defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2))
#include "../boards/Inkplate10.h"
#elif (defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2))
#include "../boards/Inkplate6plus.h"
#elif ARDUINO_INKPLATECOLOR
#include "../boards/Inkplate6Color.h"
#elif ARDUINO_INKPLATE2
#include "../boards/Inkplate2.h"
#endif

#define INKPLATE6_WAVEFORM1     0
#define INKPLATE10_WAVEFORM1    20
#define INKPLATE10_WAVEFORM2    21
#define INKPLATE10_WAVEFORM3    22
#define INKPLATE6PLUS_WAVEFORM1 40

#ifdef ARDUINO_INKPLATECOLOR

#define BLACK 0
#define WHITE 1

#else

#define BLACK 1
#define WHITE 0

#endif

#define INKPLATE_1BIT          0
#define INKPLATE_3BIT          1
#define PAD1                   10
#define PAD2                   11
#define PAD3                   12
#define PWR_GOOD_OK            0b11111010
#define INKPLATE_FORCE_PARTIAL true

#define WAKEUP 3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, HIGH);                                                    \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, LOW);                                                     \
    }
#define PWRUP 4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, PWRUP, HIGH);                                                     \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, PWRUP, LOW);                                                      \
    }
#define VCOM 5
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, VCOM, HIGH);                                                      \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, VCOM, LOW);                                                       \
    }

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

#define CL 0x01
#define CL_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = CL;                                                                                            \
    }
#define CL_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = CL;                                                                                            \
    }
#define CKV 0x01
#define CKV_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = CKV;                                                                                      \
    }
#define CKV_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = CKV;                                                                                      \
    }
#define SPH 0x02
#define SPH_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = SPH;                                                                                      \
    }
#define SPH_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = SPH;                                                                                      \
    }
#define LE 0x04
#define LE_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = LE;                                                                                            \
    }
#define LE_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = LE;                                                                                            \
    }
#define OE 0
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, OE, HIGH);                                                        \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, OE, LOW);                                                         \
    }
#define GMOD 1
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, GMOD, HIGH);                                                      \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, GMOD, LOW);                                                       \
    }
#define SPV 2
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SPV, HIGH);                                                       \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SPV, LOW);                                                        \
    }

#define GPIO0_ENABLE 8

#define DATA 0x0E8C0030

#define SQR(a)             ((int32_t)(a) * (int32_t)(a))
#define COLORDISTSQR(x, y) (SQR(RED8(x) - RED8(y)) + SQR(GREEN8(x) - GREEN8(y)) + SQR(BLUE8(x) - BLUE8(y)))

#endif