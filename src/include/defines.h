/*
defines.h
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "Arduino.h"

#ifdef ARDUINO_ESP32_DEV
#include "../boards/Inkplate6.h"
#elif ARDUINO_INKPLATE10
#include "../boards/Inkplate10.h"
#endif

#define BLACK                       1
#define WHITE                       0

#define INKPLATE_1BIT               0
#define INKPLATE_3BIT               1
#define PAD1                        0
#define PAD2                        1
#define PAD3                        2
#define PWR_GOOD_OK                 0b11111010
#define INKPLATE_FORCE_PARTIAL      true

#define WAKEUP 3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        digitalWriteMCP(WAKEUP, HIGH);                                                                                 \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        digitalWriteMCP(WAKEUP, LOW);                                                                                  \
    }
#define PWRUP 4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        digitalWriteMCP(PWRUP, HIGH);                                                                                  \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        digitalWriteMCP(PWRUP, LOW);                                                                                   \
    }
#define VCOM 5
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteMCP(VCOM, HIGH);                                                                                   \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteMCP(VCOM, LOW);                                                                                    \
    }

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

#define RGB3BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 13)
#define RGB8BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 8)

#define READ32(c)     (uint32_t)(*(c) | (*((c) + 1) << 8) | (*((c) + 2) << 16) | (*((c) + 3) << 24))
#define READ16(c)     (uint16_t)(*(c) | (*((c) + 1) << 8))
#define ROWSIZE(w, c) (((int16_t)c * w + 31) >> 5) << 2

#define RED(a)   ((((a)&0xf800) >> 11) << 3)
#define GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define BLUE(a)  (((a)&0x001f) << 3)


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
        digitalWriteMCP(OE, HIGH);                                                                                     \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        digitalWriteMCP(OE, LOW);                                                                                      \
    }
#define GMOD 1
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteMCP(GMOD, HIGH);                                                                                   \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteMCP(GMOD, LOW);                                                                                    \
    }
#define SPV 2
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        digitalWriteMCP(SPV, HIGH);                                                                                    \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        digitalWriteMCP(SPV, LOW);                                                                                     \
    }

#define GPIO0_ENABLE 8

#define DATA 0x0E8C0030

#endif