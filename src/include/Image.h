/**
 **************************************************
 * @file        Image.h
 * @brief       Basic functionalities to work with images
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
 * @authors     Soldered
 ***************************************************/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "../libs/Adafruit-GFX-Library/Adafruit_GFX.h"
#include "../libs/SdFat/SdFat.h"
#include "Arduino.h"
#include "ImageDitherColorKernels.h"
#include "NetworkClient.h"
#include "defines.h"

/**
 * @brief       Image basic class for work with images
 */
class Image : virtual public NetworkClient, virtual public Adafruit_GFX
{
  public:
    typedef enum
    {
        BMP,
        JPG,
        PNG
    } Format;

    typedef enum
    {
        Center,
        TopLeft,
        BottomLeft,
        TopRight,
        BottomRight,
        _npos
    } Position;

    Image(int16_t w, int16_t h);

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE4) && !defined(ARDUINO_INKPLATE7)
    virtual void selectDisplayMode(uint8_t _mode) = 0;
    virtual uint8_t getDisplayMode() = 0;
#endif

    virtual int16_t width() = 0;
    virtual int16_t height() = 0;

    bool drawImage(const char *path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const String path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c = BLACK, uint8_t bg = 0xFF);
    bool drawImage(const char *path, const Format &format, const int x, const int y, const bool dither = 1,
                   const bool invert = 0);
    bool drawImage(const String path, const Format &format, const int x, const int y, const bool dither = 1,
                   const bool invert = 0);
    bool drawImage(const char *path, const Format &format, const Position &position, const bool dither = 1,
                   const bool invert = 0);

    // Defined in Adafruit-GFX-Library, but should fit here
    // void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
    // int16_t h, uint16_t color); void drawBitmap(int16_t x, int16_t y, const
    // uint8_t *bitmap, int16_t w, int16_t h, uint16_t color,
    //                        uint16_t bg);

    void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h);

    bool drawBitmapFromBuffer(uint8_t *buf, int x, int y, bool dither, bool invert);

    bool drawBitmapFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawBitmapFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    bool drawBitmapFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawBitmapFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    bool drawJpegFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool dither, bool invert);

    bool drawJpegFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    bool drawJpegFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    bool drawPngFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    bool drawPngFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    // Should be private, but needed in a png callback :(
    void ditherSwap(int w);
    uint8_t ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted);

    void getPointsForPosition(const Position &position, const uint16_t imageWidth, const uint16_t imageHeight,
                              const uint16_t screenWidth, const uint16_t screenHeight, uint16_t *posX, uint16_t *posY);
    uint8_t findClosestPalette(uint32_t c);

  private:
    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;

    static bool drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert);

#ifdef ARDUINO_INKPLATE2
    uint8_t pixelBuffer[E_INK_HEIGHT * 4 + 5];
#else
    uint8_t pixelBuffer[E_INK_WIDTH * 4 + 5];
#endif

#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
    int8_t ditherBuffer[3][16][E_INK_WIDTH + 20];

    int8_t (*ditherBuffer_r)[E_INK_WIDTH + 20] = ditherBuffer[0];
    int8_t (*ditherBuffer_g)[E_INK_WIDTH + 20] = ditherBuffer[1];
    int8_t (*ditherBuffer_b)[E_INK_WIDTH + 20] = ditherBuffer[2];

    const int kernelWidth = _kernelWidth;
    const int kernelHeight = _kernelHeight;

    const int coef = _coef;
    const int kernelX = _kernelX;

    const unsigned char (*kernel)[_kernelWidth] = _kernel;
#elif ARDUINO_INKPLATE2
    int8_t ditherBuffer[3][16][E_INK_HEIGHT + 20];

    int8_t (*ditherBuffer_r)[E_INK_HEIGHT + 20] = ditherBuffer[0];
    int8_t (*ditherBuffer_g)[E_INK_HEIGHT + 20] = ditherBuffer[1];
    int8_t (*ditherBuffer_b)[E_INK_HEIGHT + 20] = ditherBuffer[2];

    const int kernelWidth = _kernelWidth;
    const int kernelHeight = _kernelHeight;

    const int coef = _coef;
    const int kernelX = _kernelX;

    const unsigned char (*kernel)[_kernelWidth] = _kernel;
#else
    uint8_t ditherBuffer[2][E_INK_WIDTH + 20];
#endif
    uint8_t jpegDitherBuffer[18][18];
    int16_t blockW = 0, blockH = 0;
    int16_t lastY = -1;

    uint32_t ditherPalette[256]; // 8 bit colors, in color, 3x8 bit colors
    uint8_t palette[128];        // 2 3 bit colors per byte, _###_###

    bool legalBmp(bitmapHeader *bmpHeader);

    uint8_t ditherGetPixelJpeg(uint8_t px, int i, int j, int x, int y, int w, int h);
    void ditherSwapBlockJpeg(int x);

    void readBmpHeader(uint8_t *buf, bitmapHeader *_h);
    void readBmpHeaderSd(SdFile *_f, bitmapHeader *_h);

    inline void displayBmpLine(int16_t x, int16_t y, bitmapHeader *bmpHeader, bool dither, bool invert);

    bool drawJpegFromWebAtPosition(const char *url, const Position &position, const bool dither = 0,
                                   const bool invert = 0);
    bool drawJpegFromSdAtPosition(const char *fileName, const Position &position, const bool dither = 0,
                                  const bool invert = 0);
    bool drawPngFromWebAtPosition(const char *url, const Position &position, const bool dither = 0,
                                  const bool invert = 0);
    bool drawPngFromSdAtPosition(const char *fileName, const Position &position, const bool dither = 0,
                                 const bool invert = 0);
    bool drawBmpFromWebAtPosition(const char *url, const Position &position, const bool dither = 0,
                                  const bool invert = 0);
    bool drawBmpFromSdAtPosition(const char *fileName, const Position &position, const bool dither = 0,
                                 const bool invert = 0);

    // FUTURE COMPATIBILITY FUNCTIONS; DO NOT USE!

    void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);

    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
    // -------------------------------------------
};

#endif