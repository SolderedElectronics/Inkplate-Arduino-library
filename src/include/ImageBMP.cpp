#include "Image.h"

#define RGB3BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 13)

void Image::readBmpHeaderSd(SdFile *_f, bitmapHeader *_h)
{
    uint8_t header[100];

    _f->rewind();
    _f->read(header, 100);
    _h->signature = read16(header + 0);
    _h->fileSize = read32(header + 2);
    _h->startRAW = read32(header + 10);
    _h->dibHeaderSize = read32(header + 14);
    _h->width = read32(header + 18);
    _h->height = read32(header + 22);
    _h->color = read16(header + 28);
    _h->compression = read32(header + 30);

    uint32_t totalColors = read32(header + 46);

    uint8_t paletteRGB[1024];

    if (_h->color <= 8)
    {
        if (!totalColors)
            totalColors = (1ULL << _h->color);

        _f->rewind();
        _f->read(paletteRGB, 53);
        _f->read(paletteRGB, totalColors * 4);

        memset(pallete, 0, sizeof pallete);
        for (int i = 0; i < totalColors; ++i)
        {
            uint32_t c = read32(paletteRGB + (i << 2));

            uint8_t r = (c & 0xFF000000) >> 24;
            uint8_t g = (c & 0x00FF0000) >> 16;
            uint8_t b = (c & 0x0000FF00) >> 8;

            pallete[i >> 1] |= RGB3BIT(r, g, b) << (i & 1 ? 0 : 4);
        }
    }
}

void Image::readBmpHeader(uint8_t *buf, bitmapHeader *_h)
{
    _h->signature = read16(buf + 0);
    _h->fileSize = read32(buf + 2);
    _h->startRAW = read32(buf + 10);
    _h->dibHeaderSize = read32(buf + 14);
    _h->width = read32(buf + 18);
    _h->height = read32(buf + 22);
    _h->color = read16(buf + 28);
    _h->compression = read32(buf + 30);

    uint32_t totalColors = read32(buf + 46);

    uint8_t paletteRGB[1024];

    if (_h->color <= 8)
    {
        if (!totalColors)
            totalColors = (1ULL << _h->color);

        memcpy(paletteRGB, buf + 53, totalColors * 4);
        memset(pallete, 0, sizeof pallete);

        for (int i = 0; i < totalColors; ++i)
        {
            uint32_t c = read32(paletteRGB + (i << 2));

            uint8_t r = (c & 0xFF000000) >> 24;
            uint8_t g = (c & 0x00FF0000) >> 16;
            uint8_t b = (c & 0x0000FF00) >> 8;

            pallete[i >> 1] |= RGB3BIT(r, g, b) << (i & 1 ? 0 : 4);
        }
    }
};

bool Image::drawBitmapFromSD(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
        return drawBitmapFromSD(&dat, x, y, dither, invert);
    else
        return 0;
}

bool Image::drawBitmapFromSD(SdFile *p, int x, int y, bool dither, bool invert)
{
    bitmapHeader bmpHeader;
    readBmpHeaderSd(p, &bmpHeader);

    if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 ||
        !(bmpHeader.color == 1 || bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 16 ||
          bmpHeader.color == 24 || bmpHeader.color == 32))
        return 0;

    if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT)
        selectDisplayMode(INKPLATE_1BIT);

    if ((bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 16 || bmpHeader.color == 24 ||
         bmpHeader.color == 32) &&
        getDisplayMode() != INKPLATE_3BIT)
        selectDisplayMode(INKPLATE_3BIT);

    int16_t w = bmpHeader.width, h = bmpHeader.height;
    int8_t c = bmpHeader.color;

    p->seekSet(bmpHeader.startRAW);

    for (int i = 0; i < h; ++i)
    {
        int16_t rowSize = (((int16_t)c * w + 31) >> 5) << 2;

        p->read(pixelBuffer, rowSize);
        displayBmpLine(x, y + i, &bmpHeader, dither, invert);
    }
    return 1;
}

void Image::displayBmpLine(int16_t x, int16_t y, bitmapHeader *bmpHeader, bool dither, bool invert)
{
    int16_t w = bmpHeader->width, h = bmpHeader->height;
    int8_t c = bmpHeader->color;

    startWrite();

    for (int j = 0; j < w; ++j)
    {
        switch (c)
        {
        case 1:
            // Should we ignore palette on 1 bit?
            writePixel(x + j, y, (invert | (pallete[0] > pallete[1])) ^ !!(pixelBuffer[j >> 3] & (1 << (7 - j & 7))));
            break;
        // as for 2 bit, literally cannot find an example online or in PS, so skipped
        case 4: {
            uint8_t px = pixelBuffer[j >> 1] & (j & 1 ? 0x0F : 0xF0) >> (j & 1 ? 0 : 4);
            if (invert)
                writePixel(x + j, y, 7 - (pallete[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4)));
            else
                writePixel(x + j, y, pallete[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4));
            break;
        }
        case 8: {
            uint8_t px = pixelBuffer[j];
            if (invert)
                writePixel(x + j, y, 7 - (pallete[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4)));
            else
                writePixel(x + j, y, pallete[px >> 1] & (px & 1 ? 0x0F : 0xF0) >> (px & 1 ? 0 : 4));
            break;
        }
        case 16: {
            uint16_t px = ((uint16_t)pixelBuffer[(j << 1) | 1] << 8) | pixelBuffer[(j << 1)];

            uint8_t r = (px & 0x7C00) >> 7;
            uint8_t g = (px & 0x3E0) >> 2;
            uint8_t b = (px & 0x1F) << 3;

            if (invert)
                writePixel(x + j, y, 7 - RGB3BIT(r, g, b));
            else
                writePixel(x + j, y, RGB3BIT(r, g, b));
            break;
        }
        case 24: {
            uint8_t r = pixelBuffer[j * 3];
            uint8_t g = pixelBuffer[j * 3 + 1];
            uint8_t b = pixelBuffer[j * 3 + 2];

            if (invert)
                writePixel(x + j, y, 7 - RGB3BIT(r, g, b));
            else
                writePixel(x + j, y, RGB3BIT(r, g, b));
            break;
        }
        case 32:
            uint8_t r = pixelBuffer[j * 4];
            uint8_t g = pixelBuffer[j * 4 + 1];
            uint8_t b = pixelBuffer[j * 4 + 2];

            if (invert)
                writePixel(x + j, y, 7 - RGB3BIT(r, g, b));
            else
                writePixel(x + j, y, RGB3BIT(r, g, b));
            break;
        }
    }

    endWrite();
}

bool Image::drawBitmapFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    uint8_t *buf = (uint8_t *)ps_malloc(800 * 600 * 4 + 100); // TODO: allocate as mush as used
    downloadFile(buf, url);

    struct bitmapHeader bmpHeader;

    readBmpHeader(buf, &bmpHeader);

    uint8_t *bufferPtr = buf + bmpHeader.startRAW;
    for (int i = 0; i < bmpHeader.height; ++i)
    {
        memcpy(pixelBuffer, bufferPtr, bmpHeader.width);
        displayBmpLine(x, y + i, &bmpHeader, dither, invert);
        bufferPtr += bmpHeader.width;
    }

    return 1;
}

bool Image::drawBitmapFromWeb(WiFiClient *s, int x, int y, int len, bool dither, bool invert)
{
    struct bitmapHeader bmpHeader;
    // readBmpHeaderWeb(s, &bmpHeader);

    if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 ||
        !(bmpHeader.color == 1 || bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 16 ||
          bmpHeader.color == 24))
        return 0;

    if ((bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 16 || bmpHeader.color == 24 ||
         bmpHeader.color == 32) &&
        getDisplayMode() != INKPLATE_3BIT)
    {
        selectDisplayMode(INKPLATE_3BIT);
    }

    if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT)
    {
        selectDisplayMode(INKPLATE_1BIT);
    }

    if (bmpHeader.color == 1)
        drawMonochromeBitmapWeb(s, bmpHeader, x, y, len, invert);
    if (bmpHeader.color == 4)
        drawGrayscaleBitmap4Web(s, bmpHeader, x, y, len, dither, invert);
    if (bmpHeader.color == 8)
        drawGrayscaleBitmap8Web(s, bmpHeader, x, y, len, dither, invert);
    if (bmpHeader.color == 24)
        drawGrayscaleBitmap24Web(s, bmpHeader, x, y, len, dither, invert);

    return 1;
}

bool Image::drawMonochromeBitmapWeb(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    uint8_t paddingBits = w % 32;
    int total = len - 34;
    w /= 32;

    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    int pnt = 0;
    while (pnt < total)
    {
        int toread = s->available();
        if (toread > 0)
        {
            int read = s->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }

    int i, j, k = bmpHeader.startRAW - 34;
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            uint32_t pixelRow = buf[k++] << 24 | buf[k++] << 16 | buf[k++] << 8 | buf[k++];
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < 32; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = buf[k++] << 24 | buf[k++] << 16 | buf[k++] << 8 | buf[k++];
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
    }

    free(buf);

    return 1;
}