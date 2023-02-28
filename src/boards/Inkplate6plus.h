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
#define FRONTLIGHT_EN 11

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
