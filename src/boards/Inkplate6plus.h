/**
 **************************************************
 *
 * @file        Inkplate6plus.h
 * @brief       Basic funtions for controling inkplate 6PLUS
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
 * @authors     @ Soldered
 ***************************************************/

#ifndef __INKPLATE6PLUS_H__
#define __INKPLATE6PLUS_H__

#ifdef ARDUINO_INKPLATE6PLUS
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x22
#endif

#ifdef ARDUINO_INKPLATE6PLUSV2
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21
#endif

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B5 // 13

#define E_INK_WIDTH   1024
#define E_INK_HEIGHT  758
#define GPIO0_ENABLE  8
#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define FRONTLIGHT_EN 11
#define PWR_GOOD_OK   0b11111010

#define DATA 0x0E8C0030 // D0-D7 = GPIO4 GPIO5 GPIO18 GPIO19 GPIO23 GPIO25 GPIO26 GPIO27

#define CL 0x01 // GPIO0
#define CL_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = CL;                                                                                            \
    }
#define CL_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = CL;                                                                                            \
    }

#define LE 0x04 // GPIO2
#define LE_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = LE;                                                                                            \
    }
#define LE_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = LE;                                                                                            \
    }

#define CKV 0x01 // GPIO32
#define CKV_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = CKV;                                                                                      \
    }
#define CKV_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = CKV;                                                                                      \
    }

#define SPH 0x02 // GPIO33
#define SPH_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out1_w1ts.val = SPH;                                                                                      \
    }
#define SPH_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out1_w1tc.val = SPH;                                                                                      \
    }

// I/O Expander - A Channel
#define GMOD 1 // GPIOA1
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, GMOD, HIGH);                                                      \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, GMOD, LOW);                                                       \
    }

#define OE 0 // GPIOA0
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, OE, HIGH);                                                        \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, OE, LOW);                                                         \
    }

#define SPV 2 // GPIOA5
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SPV, HIGH);                                                       \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, SPV, LOW);                                                        \
    }

#define WAKEUP 3 // GPIOA3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, HIGH);                                                    \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, WAKEUP, LOW);                                                     \
    }

#define PWRUP 4 // GPIOA4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, PWRUP, HIGH);                                                     \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        digitalWriteInternal(IO_INT_ADDR, ioRegsInt, PWRUP, LOW);                                                      \
    }

#define VCOM 5 // GPIOA6
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

// Touchscreen defines
#define TOUCHSCREEN_EN 12
#define TS_RTS         10
#define TS_INT         36
#define TS_ADDR        0x15

static volatile bool _tsFlag = false;
static void IRAM_ATTR tsInt()
{
    _tsFlag = true;
}

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 0, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 1, 2, 1, 0}, {0, 2, 2, 2, 1, 1, 2, 1, 0},                            \
     {0, 0, 2, 2, 2, 1, 2, 1, 0}, {0, 0, 0, 0, 2, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 1, 1, 2, 0},                            \
     {0, 0, 2, 2, 2, 1, 1, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};

#endif
