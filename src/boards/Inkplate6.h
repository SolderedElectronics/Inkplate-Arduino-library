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
 * @authors     @ e-radionica.com
 ***************************************************/

#ifndef INKPLATE6_H
#define INKPLATE6_H

#define E_INK_WIDTH  800
#define E_INK_HEIGHT 600

#define MCP23017_IODIRA 0x00
#define MCP23017_GPPUA  0x0C
#define MCP23017_ADDR   0x20

#define MCP23017_GPIOA    0x12
#define MCP23017_IOCONA   0x0A
#define MCP23017_INTCONA  0x08
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA  0x06
#define MCP23017_INTFA    0x0E
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOB    0x13

#define MCP23017_INT_ADDR 0x20
#define MCP23017_EXT_ADDR 0x22

// User pins on MCP for Inkplate 6
// Do not use others(0-8) cause they can permanently damage your screen
#define MCP23017_PIN_B1 9
#define MCP23017_PIN_B2 10
#define MCP23017_PIN_B3 11
#define MCP23017_PIN_B4 12
#define MCP23017_PIN_B5 13
#define MCP23017_PIN_B6 14
#define MCP23017_PIN_B7 15

#define WAVEFORM3BIT                                                                                                   \
    {{0, 1, 1, 0, 0, 1, 1, 0}, {0, 1, 2, 1, 1, 2, 1, 0}, {1, 1, 1, 2, 2, 1, 0, 0}, {0, 0, 0, 1, 1, 1, 2, 0},           \
     {2, 1, 1, 1, 2, 1, 2, 0}, {2, 2, 1, 1, 2, 1, 2, 0}, {1, 1, 1, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 0, 0, 2, 0}};

#endif
