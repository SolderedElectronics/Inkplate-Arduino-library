/*
ImageDither.cpp
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

#ifndef __IMAGE_DITHER_COLOR_KERNELS
#define __IMAGE_DITHER_COLOR_KERNELS

// If you want to experiment with different color dither kernels, uncomment the one you want below

// Floyd Steinberg
// int coef = 16;
// int kernelX = 1;
// unsigned char kernel[3][4] = {
//     {0, 0, 7},
//     {3, 5, 1},
// };

// J F Jarvis, C N Judice, and W H Ninke "Minimized Average Error"
// int coef = 48;
// int kernelX = 2;
// unsigned char kernel[3][5] = {
//     {0, 0, 0, 7, 5},
//     {3, 5, 7, 5, 3},
//     {1, 3, 5, 3, 1},
// };

// Atkinson
const int _coef = 8;
const int _kernelX = 1;
const unsigned char _kernel[3][4] = {
    {0, 0, 1, 1},
    {1, 1, 1, 0},
    {0, 1, 0, 0},
};

// Burkes
// int coef = 32;
// int kernelX = 2;
// unsigned char kernel[3][5] = {
//     {0, 0, 0, 8, 4},
//     {2, 4, 8, 4, 2},
//     {0, 0, 0, 0, 0},
// };

// Stucki
// int coef = 42;
// int kernelX = 2;
// unsigned char kernel[3][5] = {
//     {0, 0, 0, 8, 4},
//     {2, 4, 8, 4, 2},
//     {1, 2, 4, 2, 1},
// };

// Sierra lite
// int coef = 4;
// int kernelX = 1;
// unsigned char kernel[3][3] = {
//     {0, 0, 2},
//     {1, 1, 0},
//     {0, 0, 0},
// };

const int _kernelWidth = (sizeof _kernel[0] / sizeof _kernel[0][0]);
const int _kernelHeight = (sizeof _kernel / sizeof _kernel[0]);

#endif