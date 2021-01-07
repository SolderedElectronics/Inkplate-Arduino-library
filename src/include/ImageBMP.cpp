/*
ImageBMP.cpp
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

#include "Image.h"

bool Image::legalBmp(bitmapHeader *bmpHeader)
{
    return bmpHeader->signature == 0x4D42;

    // removed
    // && bmpHeader->compression == 0 && (bmpHeader->color == 1 || bmpHeader->color == 4 || bmpHeader->color == 8 ||
    // bmpHeader->color == 16 || bmpHeader->color == 24 || bmpHeader->color == 32)
    // because some converters don't set image depth
}

void Image::readBmpHeaderSd(SdFile *_f, bitmapHeader *_h)
{
    uint8_t header[55];

    _f->rewind();
    _f->read(header, 55);

    uint16_t color = READ16(header + 28);
    uint32_t totalColors = READ32(header + 46);

    if (color <= 8)
    {
        if (!totalColors)
            totalColors = (1ULL << color);

        uint8_t *buff = (uint8_t *)ps_malloc(totalColors * 4 + 100);

        _f->rewind();
        _f->read(buff, totalColors * 4 + 100);

        readBmpHeader(buff, _h);
        free(buff);
    }
    else
    {
        readBmpHeader(header, _h);
    }
}

void Image::readBmpHeader(uint8_t *buf, bitmapHeader *_h)
{
    _h->signature = READ16(buf + 0);
    _h->fileSize = READ32(buf + 2);
    _h->startRAW = READ32(buf + 10);
    _h->dibHeaderSize = READ32(buf + 14);
    _h->width = READ32(buf + 18);
    _h->height = READ32(buf + 22);
    _h->color = READ16(buf + 28);
    _h->compression = READ32(buf + 30);

    uint32_t totalColors = READ32(buf + 46);

    uint8_t paletteRGB[1024];

    if (_h->color <= 8)
    {
        if (!totalColors)
            totalColors = (1ULL << _h->color);

        memcpy(paletteRGB, buf + 53, totalColors * 4);
        memset(palette, 0, sizeof palette);

        for (int i = 0; i < totalColors; ++i)
        {
            uint32_t c = READ32(paletteRGB + (i << 2));

            uint8_t r = (c & 0xFF000000) >> 24;
            uint8_t g = (c & 0x00FF0000) >> 16;
            uint8_t b = (c & 0x0000FF00) >> 8;

#ifdef ARDUINO_INKPLATECOLOR
            palette[i >> 1] |= findClosestPalette(c) << (i & 1 ? 0 : 4);
            ditherPalette[i] = c;
#else
            palette[i >> 1] |= RGB3BIT(r, g, b) << (i & 1 ? 0 : 4);
            ditherPalette[i] = RGB8BIT(r, g, b);
#endif
        }
    }
};

bool Image::drawBitmapFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
        return drawBitmapFromSd(&dat, x, y, dither, invert);
    else
        return 0;
}

bool Image::drawBitmapFromSd(SdFile *p, int x, int y, bool dither, bool invert)
{
    bitmapHeader bmpHeader;

    readBmpHeaderSd(p, &bmpHeader);

    if (!legalBmp(&bmpHeader))
        return 0;

    int16_t w = bmpHeader.width, h = bmpHeader.height;
    int8_t c = bmpHeader.color;

    p->seekSet(bmpHeader.startRAW);
    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);
    for (int i = 0; i < h; ++i)
    {
        int16_t n = ROWSIZE(w, c);
        p->read(pixelBuffer, n);
        displayBmpLine(x, y + bmpHeader.height - i - 1, &bmpHeader, dither, invert);
    }
    return 1;
}

bool Image::drawBitmapFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    bool ret = 0;
    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4 + 150;
    uint8_t *buf = downloadFile(url, &defaultLen);

    ret = drawBitmapFromBuffer(buf, x, y, dither, invert);
    free(buf);

    return ret;
}

bool Image::drawBitmapFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither, bool invert)
{
    bool ret = 0;
    uint8_t *buf = downloadFile(s, len);

    ret = drawBitmapFromBuffer(buf, x, y, dither, invert);
    free(buf);

    return ret;
}

bool Image::drawBitmapFromBuffer(uint8_t *buf, int x, int y, bool dither, bool invert)
{
    bitmapHeader bmpHeader;

    readBmpHeader(buf, &bmpHeader);

    if (!legalBmp(&bmpHeader))
        return 0;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    uint8_t *bufferPtr = buf + bmpHeader.startRAW;
    for (int i = 0; i < bmpHeader.height; ++i)
    {
        memcpy(pixelBuffer, bufferPtr, ROWSIZE(bmpHeader.width, bmpHeader.color));
        displayBmpLine(x, y + bmpHeader.height - i - 1, &bmpHeader, dither, invert);
        bufferPtr += ROWSIZE(bmpHeader.width, bmpHeader.color);
    }

    return 1;
}

void Image::displayBmpLine(int16_t x, int16_t y, bitmapHeader *bmpHeader, bool dither, bool invert)
{
    int16_t w = bmpHeader->width;
    int8_t c = bmpHeader->color;

    startWrite();
    for (int j = 0; j < w; ++j)
    {
        switch (c)
        {
        case 1:
#ifdef ARDUINO_INKPLATECOLOR
            writePixel(x + j, y,
                       (!invert ^ (palette[0] > palette[1])) ^ !!(pixelBuffer[j >> 3] & (1 << (7 - (j & 7)))));
#else
            writePixel(x + j, y, (invert ^ (palette[0] > palette[1])) ^ !!(pixelBuffer[j >> 3] & (1 << (7 - (j & 7)))));
#endif
            break;
        // as for 2 bit, literally cannot find an example online or in PS, so skipped
        case 4: {
            uint8_t px = pixelBuffer[j >> 1] & (j & 1 ? 0x0F : 0xF0) >> (j & 1 ? 0 : 4);
            uint8_t val;

            if (dither)
                val = ditherGetPixelBmp(px, j, w, 1);
            else
            {
#ifdef ARUDUINO_INKPLATECOLOR
                val = palette[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4);
#else
                val = palette[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4);
#endif
            }
            if (invert)
                val = 7 - val;
            if (getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            writePixel(x + j, y, val);
            break;
        }
        case 8: {
            uint8_t px = pixelBuffer[j];
            uint8_t val;

            if (dither)
                val = ditherGetPixelBmp(px, j, w, 1);
            else
            {
#ifdef ARUDUINO_INKPLATECOLOR
                val = palette[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4);
#else
                val = palette[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4);
#endif
            }
            if (invert)
                val = 7 - val;
            if (getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            writePixel(x + j, y, val);
            break;
        }
        case 16: {
            uint16_t px = ((uint16_t)pixelBuffer[(j << 1) | 1] << 8) | pixelBuffer[(j << 1)];

            uint8_t r = (px & 0x7C00) >> 7;
            uint8_t g = (px & 0x3E0) >> 2;
            uint8_t b = (px & 0x1F) << 3;

            uint8_t val;

            if (dither)
                val = ditherGetPixelBmp(RGB8BIT(r, g, b), j, w, 0);
            else
                val = RGB3BIT(r, g, b);
            if (invert)
                val = 7 - val;
            if (getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            writePixel(x + j, y, val);
            break;
        }
        case 24: {
            uint8_t r = pixelBuffer[j * 3];
            uint8_t g = pixelBuffer[j * 3 + 1];
            uint8_t b = pixelBuffer[j * 3 + 2];

            uint8_t val;

            if (dither)
                val = ditherGetPixelBmp(RGB8BIT(r, g, b), j, w, 0);
            else
                val = RGB3BIT(r, g, b);
            if (invert)
                val = 7 - val;
            if (getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            writePixel(x + j, y, val);
            break;
        }
        case 32:
            uint8_t r = pixelBuffer[j * 4];
            uint8_t g = pixelBuffer[j * 4 + 1];
            uint8_t b = pixelBuffer[j * 4 + 2];

            uint8_t val;

            if (dither)
                val = ditherGetPixelBmp(RGB8BIT(r, g, b), j, w, 0);
            else
                val = RGB3BIT(r, g, b);
            if (invert)
                val = 7 - val;
            if (getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            writePixel(x + j, y, val);
            break;
        }
    }

    ditherSwap(w);
    endWrite();
}

bool Image::drawBmpFromWebAtPosition(const char *url, const Position &position, const bool dither, const bool invert)
{
    bool ret = 0;
    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4 + 150;
    uint8_t *buf = downloadFile(url, &defaultLen);

    bitmapHeader bmpHeader;
    readBmpHeader(buf, &bmpHeader);

    uint16_t posX, posY;
    getPointsForPosition(position, bmpHeader.width, bmpHeader.height, E_INK_WIDTH, E_INK_HEIGHT, &posX, &posY);
    ret = drawBitmapFromBuffer(buf, posX, posY, dither, invert);
    free(buf);

    return ret;
}

bool Image::drawBmpFromSdAtPosition(const char *fileName, const Position &position, const bool dither,
                                    const bool invert)
{
    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
        return 0;
    bitmapHeader bmpHeader;

    readBmpHeaderSd(&dat, &bmpHeader);

    if (!legalBmp(&bmpHeader))
        return 0;

    int16_t w = bmpHeader.width, h = bmpHeader.height;
    int8_t c = bmpHeader.color;

    dat.seekSet(bmpHeader.startRAW);
    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    uint16_t posX, posY;
    getPointsForPosition(position, bmpHeader.width, bmpHeader.height, E_INK_WIDTH, E_INK_HEIGHT, &posX, &posY);

    for (int i = 0; i < h; ++i)
    {
        int16_t n = ROWSIZE(w, c);
        dat.read(pixelBuffer, n);
        displayBmpLine(posX, posY + bmpHeader.height - i - 1, &bmpHeader, dither, invert);
    }

    return 1;
}