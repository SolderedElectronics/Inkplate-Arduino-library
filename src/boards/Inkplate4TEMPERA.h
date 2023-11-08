/**
 **************************************************
 *
 * @file        Inkplate4TEMPERA.h
 * @brief       Basic funtions for controlling Inkplate 4TEMPERA
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
 * @authors     @ e-radionica.com
 ***************************************************/

#ifndef __INKPLATE4TEMPERA_H__
#define __INKPLATE4TEMPERA_H__

// Panel resolution
#define E_INK_WIDTH  600
#define E_INK_HEIGHT 600

// IO Expander addresses
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21

// Some constants used in the library
#define GPIO0_ENABLE  8
#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define PWR_GOOD_OK   0b11111010

// Addresss to control the digipot (and affect beep frequency)
#define DIGIPOT_ADDR 0x2F

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN 11

// Constants for selecting a sensor when turning them on or off
#define INKPLATE_BUZZER        0x01
#define INKPLATE_ACCELEROMETER 0x02
#define INKPLATE_BME688        0x04
#define BME_CONTROL_ADDR       0x74
#define INKPLATE_APDS9960      0x08
#define INKPLATE_FUEL_GAUGE    0x1

// Pin interrupts connected to the internal gpio expander
#define INT_APDS 9
#define INT2_LSM 13
#define INT1_LSM 14
// Buzzer enable pin (pin P1_4 on internal IO expander)
#define BUZZ_EN 12

// Pin for Fuel Gauge GPOUT.
#define FG_GPOUT    15

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

// Frontlight
#define FRONTLIGHT_EN 10

// Touchscreen defines
#define TOUCHSCREEN_IO_EXPANDER IO_EXT_ADDR
#define TOUCHSCREEN_IO_REGS     ioRegsEx
#define TOUCHSCREEN_EN          0
#define TS_RTS                  1
#define TS_INT                  36
#define TS_ADDR                 0x15

static volatile bool _tsFlag = false;
static void IRAM_ATTR tsInt()
{
    _tsFlag = true;
}

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 1, 1, 1, 1, 1, 0}, {1, 1, 1, 2, 1, 1, 0, 0}, {2, 1, 1, 0, 2, 1, 1, 0}, {0, 0, 0, 1, 1, 1, 2, 0},           \
     {2, 1, 1, 2, 1, 1, 2, 0}, {1, 2, 1, 1, 2, 1, 2, 0}, {1, 1, 1, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 0, 2, 2, 0}};

#endif
