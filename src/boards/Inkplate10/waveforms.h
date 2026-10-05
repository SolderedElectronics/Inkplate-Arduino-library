#ifndef __WAVEFROMS_INKPLATE_10_H__
#define __WAVEFROMS_INKPLATE_10_H__

#if defined(ARDUINO_ESP32S3_DEV)

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0},                            \
     {0, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0},                            \
     {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};

#ifndef E_INK_WIDTH
#define E_INK_WIDTH 1200
#endif
#ifndef E_INK_HEIGHT
#define E_INK_HEIGHT 825
#endif

#endif
#endif