/**
 **************************************************
 *
 * @file        Inkplate5.h
 * @brief       Basic funtions for controling inkplate 5
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

#ifndef INKPLATE5_H
#define INKPLATE5_H

#define E_INK_WIDTH 800
#define E_INK_HEIGHT 600

#define MCP23017_INT_ADDR 0x20
#define MCP23017_EXT_ADDR 0x22
#define MCP23017_INT_PORTA 0x00
#define MCP23017_INT_PORTB 0x01
#define MCP23017_INT_NO_MIRROR false
#define MCP23017_INT_MIRROR true
#define MCP23017_INT_PUSHPULL false
#define MCP23017_INT_OPENDRAIN true
#define MCP23017_INT_ACTLOW false
#define MCP23017_INT_ACTHIGH true

#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14

#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define WAVEFORM3BIT                                                           \
  {                                                                            \
      {0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0},                      \
      {0, 2, 2, 2, 2, 1, 1, 0}, {2, 2, 2, 2, 2, 1, 1, 0},                      \
      {0, 0, 2, 2, 1, 1, 2, 0}, {0, 2, 2, 2, 1, 1, 2, 0},                      \
      {0, 0, 0, 2, 2, 1, 2, 0}, {2, 2, 2, 2, 2, 2, 2, 0},                      \
  };

#endif