/**
 **************************************************
 * @file        Image.h
 * @brief       Basic functionalities to work with images
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact assistance@soldered.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#ifndef __IMAGE_H__
#define __IMAGE_H__
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&            \
    !defined(ARDUINO_INKPLATE31SPECTRA)
#include "../../features/SdFat/SdFat.h"
#include "ImageDitherKernels.h"
#include "WiFi.h"

class Inkplate;

/**
 * @brief       BitmapHeader structure that includes standard bitmap parameters
 */
struct bitmapHeader
{
    uint16_t signature;     // Is picture a legal BMP
    uint32_t fileSize;      // Size of image in bytes
    uint32_t startRAW;      // Address where raw data (pixel array) can be found/is started
    uint32_t dibHeaderSize; // Size of the header in bytes
    uint32_t width;         // Width of image
    uint32_t height;        // Height of image
    uint16_t color;         // The number of bits per pixel, which is the color depth of
                            // the image. Typical values are 1, 4, 8, 16, 24 and 32
    uint32_t compression;   // The compression method being used. See the next table
                            // for a list of possible values
};

/**
 * @brief       Image basic class for work with images
 */
class Image
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

    typedef enum
    {
        FloydSteinberg = 0,
        JarvisJudiceNinke,
        Atkinson,
        Burkes,
        Stucki,
        SierraLite,
        ReducedDiffusion
    } DitherKernel;


    Inkplate *_inkplate = NULL;

    void begin(Inkplate *inkplateptr);

    bool draw(const char *path, int x, int y, bool dither = 1, bool invert = 0);
    bool draw(const String path, int x, int y, bool dither = 1, bool invert = 0);
    bool draw(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c = 1, uint8_t bg = 0xFF);
    bool draw(const char *path, const Format &format, const int x, const int y, const bool dither = 1,
              const bool invert = 0);
    bool draw(const String path, const Format &format, const int x, const int y, const bool dither = 1,
              const bool invert = 0);
    bool draw(const char *path, const Format &format, const Position &position, const bool dither = 1,
              const bool invert = 0);

    bool getFileExtension(char *_filename, char *_extension);

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

    bool drawPngFromBuffer(uint8_t *buf, int32_t len, int x, int y, bool dither = 0, bool invert = 0);

    bool drawPngFromSd(const char *fileName, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromSd(SdFile *p, int x, int y, bool dither = 0, bool invert = 0);

    bool drawPngFromWeb(const char *url, int x, int y, bool dither = 0, bool invert = 0);
    bool drawPngFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither = 0, bool invert = 0);

    uint8_t ditherGetPixelBmp(uint32_t px, int i, int j, int w, bool paletted);

    void setDitherKernel(const DitherKernel kernel);

    void getPointsForPosition(const Position &position, const uint16_t imageWidth, const uint16_t imageHeight,
                              const uint16_t screenWidth, const uint16_t screenHeight, uint16_t *posX, uint16_t *posY);


  private:
    static uint8_t *pixelBuffer;
    static const uint8_t ditherRowCount = 4;
    static const uint8_t ditherRowMask = ditherRowCount - 1;
    static int16_t (*ditherBuffer)[E_INK_WIDTH + 20];
    static constexpr size_t ditherBufferSizeBytes = ditherRowCount * (E_INK_WIDTH + 20) * sizeof(int16_t);
    static uint32_t *ditherPalette; // 8 bit colors, in color, 3x8 bit colors
    static uint8_t *palette;        // 2 3 bit colors per byte, _###_###

    const DitherKernelDef *currentKernel = &DITHER_KERNELS[0];

    uint16_t _lastTileRowY = -1;

    static bool drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert);

    int16_t blockW = 0, blockH = 0;
    int16_t lastY = -1;

    uint16_t *jpegRowBuffer = nullptr;
    uint16_t jpegImageWidth = 0;
    uint8_t jpegMcuH = 0;
    int jpegDrawX = 0, jpegDrawY = 0;
    bool jpegDither = false, jpegInvert = false;

    bool legalBmp(bitmapHeader *bmpHeader);

    void flushJpegRow(int rowY);

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
#endif
