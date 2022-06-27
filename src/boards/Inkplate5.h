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

#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x22

#define E_INK_WIDTH  800
#define E_INK_HEIGHT 600

#define WAVEFORM3BIT                                                                                                   \
    {                                                                                                                  \
        {0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 2, 2, 2, 2, 1, 1, 0}, {2, 2, 2, 2, 2, 1, 1, 0},        \
        {0, 0, 2, 2, 1, 1, 2, 0}, {0, 2, 2, 2, 1, 1, 2, 0}, {0, 0, 0, 2, 2, 1, 2, 0}, {2, 2, 2, 2, 2, 2, 2, 0},        \
    };

#endif