#include "Image.h"

#include "pgmspace.h"

#include "../libs/TJpeg/TJpg_Decoder.h"
#include "defines.h"

#define RED(a)   ((((a)&0xf800) >> 11) << 3)
#define GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define BLUE(a)  (((a)&0x001f) << 3)

Image *_imagePtrJpeg = nullptr;

Image::Image()
{
    _imagePtrJpeg = this;
}

bool Image::drawImage(const String path, int x, int y, bool dither, bool invert)
{
    return drawImage(path.c_str(), x, y, dither, invert);
};

bool Image::drawImage(const char *path, int x, int y, bool dither, bool invert)
{
    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        // TODO: Implement
        return 0;
    }
    else
    {
        if (strstr(path, ".bmp") != NULL)
            return drawBitmapFromSD(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromSD(path, x, y, dither, invert);
    }
};

bool Image::drawImage(const SdFile *path, int x, int y, bool dither, bool invert){

};

bool Image::drawImage(const WiFiClient *s, int x, int y, int len, bool dither, bool invert){

};

uint32_t Image::read32(uint8_t *c)
{
    return (*(c) | (*(c + 1) << 8) | (*(c + 2) << 16) | (*(c + 3) << 24));
}

uint16_t Image::read16(uint8_t *c)
{
    return (*(c) | (*(c + 1) << 8));
}


// Loads first line in current dither buffer
void Image::ditherStart(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits)
{
    for (int i = 0; i < w; ++i)
        if (bits == 24)
        {
            if (invert)
                ditherBuffer[0][i] = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) +
                                     ((255 - *(bufferPtr++)) * 722 / 10000);
            else
                ditherBuffer[0][i] =
                    (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
        }
        else if (bits == 8)
        {
            if (invert)
                ditherBuffer[0][i] = 255 - *(bufferPtr++);
            else
                ditherBuffer[0][i] = *(bufferPtr++);
        }
    if (bits == 4)
    {
        int _w = w / 8;
        int paddingBits = w % 8;

        for (int i = 0; i < _w; ++i)
        {
            for (int n = 0; n < 4; n++)
            {
                uint8_t temp = *(bufferPtr++);
                ditherBuffer[0][i * 8 + n * 2] = temp & 0xF0;
                ditherBuffer[0][i * 8 + n * 2 + 1] = (temp & 0x0F) << 4;

                if (invert)
                {
                    ditherBuffer[0][i * 8 + n * 2] = ~ditherBuffer[0][i * 8 + n * 2];
                    ditherBuffer[0][i * 8 + n * 2 + 1] = ~ditherBuffer[0][i * 8 + n * 2 + 1];
                }
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                ditherBuffer[0][_w * 8 + n] = (pixelRow & (0xFULL << ((7 - n) * 4))) >> ((7 - n) * 4 - 4);
            }
        }
    }
}

// Loads next line, after this ditherGetPixel can be called and alters values in next line
void Image::ditherLoadNextLine(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits)
{
    for (int i = 0; i < w; ++i)
    {
        if (bits == 24)
        {
            if (invert)
                ditherBuffer[1][i] = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) +
                                     ((255 - *(bufferPtr++)) * 722 / 10000);
            else
                ditherBuffer[1][i] =
                    (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
        }
        else if (bits == 8)
        {
            if (invert)
                ditherBuffer[1][i] = 255 - *(bufferPtr++);
            else
                ditherBuffer[1][i] = *(bufferPtr++);
        }
    }
    if (bits == 4)
    {
        int _w = w / 8;
        int paddingBits = w % 8;

        for (int i = 0; i < _w; ++i)
        {
            for (int n = 0; n < 4; n++)
            {
                uint8_t temp = *(bufferPtr++);
                ditherBuffer[0][i * 8 + n * 2] = temp & 0xF0;
                ditherBuffer[0][i * 8 + n * 2 + 1] = (temp & 0x0F) << 4;

                if (invert)
                {
                    ditherBuffer[0][i * 8 + n * 2] = ~ditherBuffer[0][i * 8 + n * 2];
                    ditherBuffer[0][i * 8 + n * 2 + 1] = ~ditherBuffer[0][i * 8 + n * 2 + 1];
                }
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                ditherBuffer[1][_w * 8 + n] = (pixelRow & (0xFULL << (28 - n * 4))) >> (28 - n * 4 - 4);
            }
        }
    }
}

// Gets specific pixel, mainly at i, j is just used for bound checking when changing next line values
uint8_t Image::ditherGetPixel(int i, int j, int w, int h)
{
    uint8_t oldpixel = ditherBuffer[0][i];
    uint8_t newpixel = (oldpixel & B11100000);

    ditherBuffer[0][i] = newpixel;

    uint8_t quant_error = oldpixel - newpixel;

    if (i + 1 < w)
        ditherBuffer[0][i + 1] = min(255, (int)ditherBuffer[0][i + 1] + (((int)quant_error * 7) >> 4));
    if (j + 1 < h && 0 <= i - 1)
        ditherBuffer[1][i - 1] = min(255, (int)ditherBuffer[1][i - 1] + (((int)quant_error * 3) >> 4));
    if (j + 1 < h)
        ditherBuffer[1][i + 0] = min(255, (int)ditherBuffer[1][i + 0] + (((int)quant_error * 5) >> 4));
    if (j + 1 < h && i + 1 < w)
        ditherBuffer[1][i + 1] = min(255, (int)ditherBuffer[1][i + 1] + (((int)quant_error * 1) >> 4));

    return newpixel;
}

// Swaps current and next line, for next one to be overwritten
uint8_t Image::ditherSwap(int w)
{
    for (int i = 0; i < w; ++i)
        ditherBuffer[0][i] = ditherBuffer[1][i];
}

void Image::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
                byte <<= 1;
            else
                byte = bitmap[j * byteWidth + i / 8];

            if (byte & 0x80)
                writePixel(x + i, y, color);
            else if (bg != 0xFFFF)
                writePixel(x + i, y, bg);
        }
    }
    endWrite();
}

void Image::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h)
{
    if (getDisplayMode() != INKPLATE_3BIT)
        return;
    uint8_t _rem = _w & 1;
    int i, j;
    int xSize = (_w >> 1) + _rem;

    startWrite();
    for (i = 0; i < _h; i++)
    {
        for (j = 0; j < xSize - 1; j++)
        {
            writePixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
        }
        writePixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
        if (_rem == 0)
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
    }
    endWrite();
}

// FUTURE COMPATIBILITY FUNCTIONS; DO NOT USE!
void Image::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
        for (int16_t i = 0; i < w; i++)
            writePixel(x + i, y, bitmap[j * w + i]);

    endWrite();
}

void Image::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
    int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
                byte <<= 1;
            else
                byte = mask[j * bw + i / 8];

            if (byte & 0x80)
            {
                writePixel(x + i, y, bitmap[j * w + i]);
            }
        }
    }
    endWrite();
}

void Image::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
        for (int16_t i = 0; i < w; i++)
            writePixel(x + i, y, bitmap[j * w + i]);

    endWrite();
}

void Image::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
    int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
                byte <<= 1;
            else
                byte = mask[j * bw + i / 8];

            if (byte & 0x80)
                writePixel(x + i, y, bitmap[j * w + i]);
        }
    }
    endWrite();
}