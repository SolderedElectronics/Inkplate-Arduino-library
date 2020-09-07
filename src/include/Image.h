#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "../libs/SdFat/SdFat.h"
#include "Arduino.h"
#include "WiFiClient.h"
#include "defines.h"

struct bitmapHeader
{
    uint16_t signature;
    uint32_t fileSize;
    uint32_t startRAW;
    uint32_t dibHeaderSize;
    uint32_t width;
    uint32_t height;
    uint16_t color;
    uint32_t compression;
};

class Image
{
  public:
    Image();

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    virtual void selectDisplayMode(uint8_t _mode) = 0;
    virtual uint8_t getDisplayMode() = 0;
    virtual int16_t width() = 0;
    virtual int16_t height() = 0;

    bool drawImage(const char *path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const String path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const SdFile *path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const WiFiClient *s, int x, int y, int len = -1, bool dither = 1, bool invert = 0);

    void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg = 0xFFFF);
    void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h);

    bool drawBitmapFromSD(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);
    bool drawBitmapFromSD(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);

    bool drawBitmapFromWeb(WiFiClient *s, int x, int y, int len, bool dither = 0, bool invert = 0);
    bool drawBitmapFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);

    bool drawJpegFromSD(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromSD(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    bool drawJpegFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromWeb(WiFiClient *s, int x, int y, int len, bool dither = 0, bool invert = 0);


  private:
    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;

    static bool drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool _dither,
                              bool _invert);

    uint8_t pixelBuffer[800 * 4 + 5];
    uint8_t ditherBuffer[800 * 3 + 5][2];
    uint8_t pallete[128]; // 2 colors per byte, _###_###

    void ditherStart(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits);
    void ditherLoadNextLine(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits);
    uint8_t ditherGetPixel(int i, int j, int w, int h);
    uint8_t ditherSwap(int w);

    uint32_t read32(uint8_t *c);
    uint16_t read16(uint8_t *c);

    void readBmpHeaderSd(SdFile *_f, bitmapHeader *_h);
    inline void displayBmpLine(int16_t x, int16_t y, SdFile *f, bitmapHeader *bmpHeader, bool dither, bool invert);

    bool drawMonochromeBitmapWeb(WiFiClient *s, bitmapHeader bmpHeader, int x, int y, int len, bool invert);
    bool drawGrayscaleBitmap4Web(WiFiClient *s, bitmapHeader bmpHeader, int x, int y, int len, bool dither,
                                 bool invert);
    bool drawGrayscaleBitmap8Web(WiFiClient *s, bitmapHeader bmpHeader, int x, int y, int len, bool dither,
                                 bool invert);
    bool drawGrayscaleBitmap24Web(WiFiClient *s, bitmapHeader bmpHeader, int x, int y, int len, bool dither,
                                  bool invert);

    // FUTURE COMPATIBILITY FUNCTIONS; DO NOT USE!

    void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);

    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
    // -------------------------------------------
};


#endif