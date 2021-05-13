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
 *              This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example.
 *              If you have any questions about licensing, please contact techsupport@e-radionica.com
 *              Distributed as-is; no warranty is given.
 * 
 * @authors     @ e-radionica.com
 ***************************************************/


#ifndef __INKPLATE6PLUS_H__
#define __INKPLATE6PLUS_H__

#define E_INK_WIDTH  1024
#define E_INK_HEIGHT 758

#define MCP23017_INT_ADDR      0x20
#define MCP23017_EXT_ADDR      0x22
#define MCP23017_INT_PORTA     0x00
#define MCP23017_INT_PORTB     0x01
#define MCP23017_INT_NO_MIRROR false
#define MCP23017_INT_MIRROR    true
#define MCP23017_INT_PUSHPULL  false
#define MCP23017_INT_OPENDRAIN true
#define MCP23017_INT_ACTLOW    false
#define MCP23017_INT_ACTHIGH   true

#define MCP23017_IODIRA   0x00
#define MCP23017_IPOLA    0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA  0x06
#define MCP23017_INTCONA  0x08
#define MCP23017_IOCONA   0x0A
#define MCP23017_GPPUA    0x0C
#define MCP23017_INTFA    0x0E
#define MCP23017_INTCAPA  0x10
#define MCP23017_GPIOA    0x12
#define MCP23017_OLATA    0x14

#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATB    0x15

#define E_INK_WIDTH   1024
#define E_INK_HEIGHT  758
#define GPIO0_ENABLE  8
#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define FRONTLIGHT_EN  11
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
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, HIGH);                                               \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, LOW);                                                \
    }

#define OE 0 // GPIOA0
#define OE_SET                                                                                                         \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, HIGH);                                                 \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, LOW);                                                  \
    }

#define SPV 2 // GPIOA5
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, HIGH);                                                \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, LOW);                                                 \
    }

#define WAKEUP 3 // GPIOA3
#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, WAKEUP, HIGH);                                             \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, WAKEUP, LOW);                                              \
    }

#define PWRUP 4 // GPIOA4
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, PWRUP, HIGH);                                              \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, PWRUP, LOW);                                               \
    }

#define VCOM 5 // GPIOA6
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, VCOM, HIGH);                                               \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, VCOM, LOW);                                                \
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