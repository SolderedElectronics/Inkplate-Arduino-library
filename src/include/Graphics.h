/**
 **************************************************
 * @file        Graphics.h
 * @brief       Basic graphics functionalities
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
 * @authors     e-radionica.com
 ***************************************************/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "Arduino.h"

#include "Image.h"
#include "Shapes.h"

#include <pgmspace.h>

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

/**
 * @brief       Graphics class that holds basic functionalities for Inkplate
 * display
 */
class Graphics : public Shapes, public Image
{
  public:
    Graphics(int16_t w, int16_t h) : Adafruit_GFX(w, h), Shapes(w, h), Image(w, h){};

    void setRotation(uint8_t r);
    uint8_t getRotation();

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;

#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2)
    void selectDisplayMode(uint8_t _mode);
    void setDisplayMode(uint8_t _mode);
#endif
    uint8_t getDisplayMode();

    int16_t width() override;
    int16_t height() override;

    uint8_t *DMemoryNew;
    uint8_t *_partial;
    uint8_t *DMemory4Bit;
    uint8_t *_pBuffer;

    const uint8_t LUT2[16] = {0xAA, 0xA9, 0xA6, 0xA5, 0x9A, 0x99, 0x96, 0x95,
                              0x6A, 0x69, 0x66, 0x65, 0x5A, 0x59, 0x56, 0x55};
    const uint8_t LUTW[16] = {0xFF, 0xFE, 0xFB, 0xFA, 0xEF, 0xEE, 0xEB, 0xEA,
                              0xBF, 0xBE, 0xBB, 0xBA, 0xAF, 0xAE, 0xAB, 0xAA};
    const uint8_t LUTB[16] = {0xFF, 0xFD, 0xF7, 0xF5, 0xDF, 0xDD, 0xD7, 0xD5,
                              0x7F, 0x7D, 0x77, 0x75, 0x5F, 0x5D, 0x57, 0x55};

    const uint8_t pixelMaskLUT[8] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    const uint8_t pixelMaskGLUT[2] = {0xF, 0xF0};

    const uint8_t discharge[16] = {0xFF, 0xFC, 0xF3, 0xF0, 0xCF, 0xCC, 0xC3, 0xC0,
                                   0x3F, 0x3C, 0x33, 0x30, 0xF,  0xC,  0x3,  0x0};
    uint8_t _blockPartial = 1;

  private:
    void startWrite(void) override;
    void writePixel(int16_t x, int16_t y, uint16_t color) override;
    void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
    void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
    void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
    void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
    void endWrite(void) override;

    uint8_t _displayMode = 0;

  protected:
};

#endif