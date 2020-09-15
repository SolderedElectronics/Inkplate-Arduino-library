#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "../libs/Adafruit-GFX-Library/Adafruit_GFX.h"
#include "../libs/SdFat/SdFat.h"
#include "Arduino.h"
#include "NetworkClient.h"
#include "WiFiClient.h"
#include "defines.h"

class Image : virtual public NetworkClient, virtual public Adafruit_GFX
{
public:
    Image(int16_t w, int16_t h);

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    virtual void selectDisplayMode(uint8_t _mode) = 0;
    virtual uint8_t getDisplayMode() = 0;
    virtual int16_t width() = 0;
    virtual int16_t height() = 0;

    bool drawImage(const char *path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const String path, int x, int y, bool dither = 1, bool invert = 0);
    bool drawImage(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c = BLACK, uint8_t bg = 0xFF);

    // Defined in Adafruit-GFX-Library, but should fit here
    // void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    // void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color,
    //                        uint16_t bg);

    void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h);

    bool drawBitmapFromBuffer(uint8_t *buf, int x, int y, bool dither, bool invert);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawBitmapFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawBitmapFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawBitmapFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawBitmapFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    bool drawJpegFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool dither, bool invert);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawJpegFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawJpegFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawJpegFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawPngFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    [[deprecated("Use drawImage, as this will soon become a private method.")]] bool drawPngFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

private:
    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;

    static bool drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert);

    uint8_t pixelBuffer[800 * 4 + 5];
    uint8_t ditherBuffer[2][800 + 20];
    uint8_t jpegDitherBuffer[18][18];
    int16_t blockW = 0, blockH = 0;
    int16_t lastY = -1;

    uint8_t ditherPalette[256]; // 8 bit colors
    uint8_t palette[128];       // 2 3 bit colors per byte, _###_###

    bool legalBmp(bitmapHeader *bmpHeader);

    uint8_t ditherGetPixelBmp(uint8_t px, int i, int w, bool paletted);
    uint8_t ditherGetPixelJpeg(uint8_t px, int i, int j, int x, int y, int w, int h);
    void ditherSwap(int w);
    void ditherSwapBlockJpeg(int x);

    void readBmpHeader(uint8_t *buf, bitmapHeader *_h);
    void readBmpHeaderSd(SdFile *_f, bitmapHeader *_h);

    inline void displayBmpLine(int16_t x, int16_t y, bitmapHeader *bmpHeader, bool dither, bool invert);

    // FUTURE COMPATIBILITY FUNCTIONS; DO NOT USE!

    void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);

    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
    // -------------------------------------------
};

#endif