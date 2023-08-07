/**
 **************************************************
 *
 * @file        Inkplate6.h
 * @brief       Basic funtions for controling inkplate 6
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

#ifndef INKPLATE6_H
#define INKPLATE6_H

#ifdef ARDUINO_ESP32_DEV
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x22
#endif

#ifdef ARDUINO_INKPLATE6V2
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21
#endif

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B2 // 10

#define E_INK_WIDTH  800
#define E_INK_HEIGHT 600

// Last element (index = 8) in waveform array is not used!
#ifdef ARDUINO_ESP32_DEV
#define WAVEFORM3BIT                                                                                                   \
    {{0, 1, 1, 0, 0, 1, 1, 0, 0}, {0, 1, 2, 1, 1, 2, 1, 0, 0}, {1, 1, 1, 2, 2, 1, 0, 0, 0},                            \
     {0, 0, 0, 1, 1, 1, 2, 0, 0}, {2, 1, 1, 1, 2, 1, 2, 0, 0}, {2, 2, 1, 1, 2, 1, 2, 0, 0},                            \
     {1, 1, 1, 2, 1, 2, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 2, 0, 0}};

#endif


#ifdef ARDUINO_INKPLATE6V2
#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 1, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 1, 1, 0, 0}, {1, 1, 1, 1, 0, 2, 1, 0, 0},                            \
     {1, 1, 1, 2, 2, 1, 1, 0, 0}, {1, 1, 1, 1, 2, 2, 1, 0, 0}, {0, 1, 1, 1, 2, 2, 1, 0, 0},                            \
     {0, 0, 0, 0, 1, 1, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 2, 0, 0}};

#endif

#endif