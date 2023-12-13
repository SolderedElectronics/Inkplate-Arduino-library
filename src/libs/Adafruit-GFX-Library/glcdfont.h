#ifndef FONT8x16_H
#define FONT8x16_H

#include <stdint.h>

#define UNIFONT_BLOCK_HAS_NON_SPACING_MARKS (1)
#define UNIFONT_BLOCK_IS_NARROW (1<<1)
#define UNIFONT_BLOCK_IS_WIDE (1<<2)
#define UNIFONT_BLOCK_IN_PROGMEM (1<<7)

#define UNIFONT_NARROW_BLOCK_LENGTH (4096)
#define UNIFONT_WIDE_BLOCK_LENGTH (8192)
#define UNIFONT_BITMASK_LENGTH (32)

typedef union {
    const uint8_t* location;
    int32_t offset;
} UnifontLocation;

typedef struct {
    UnifontLocation glyphs;      // If low bit of flags is set, use the location pointer.
                                 // Otherwise, use the offset and look in the unifont.bin file.
    uint8_t flags;               // 0bx0000xxx
                                 //   |    |||
                                 //   |    ||\__ This block contains non-spacing code points
                                 //   |    ||    (check spacing data after length data to determine advance)
                                 //   |    |\___ This block contains exclusively narrow (16-byte) glyphs
                                 //   |    \____ This block contains exclusively wide (32-byte) glyphs
                                 //   |          (if both of these are 0, all glyphs are 32 bytes but some are
                                 //   |           half-width, check width bitmasks after glyph data for advance)
                                 //   \_ This block is included in PROGMEM
} UnifontBlock;

typedef struct {
    uint8_t blockNumber;
    UnifontBlock blockData;
} UnifontInclusion;

#endif // FONT8x16_H
