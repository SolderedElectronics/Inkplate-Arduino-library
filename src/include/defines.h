#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "Arduino.h"

#define E_INK_WIDTH  800
#define E_INK_HEIGHT 600

#define BLACK 1
#define WHITE 0

#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define PAD1          0
#define PAD2          1
#define PAD3          2

#define RGB3BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 13)
#define RGB8BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 8)

#define READ32(c)     (uint32_t)(*(c) | (*((c) + 1) << 8) | (*((c) + 2) << 16) | (*((c) + 3) << 24));
#define READ16(c)     (uint16_t)(*(c) | (*((c) + 1) << 8));
#define ROWSIZE(w, c) (((int16_t)c * w + 31) >> 5) << 2

#endif