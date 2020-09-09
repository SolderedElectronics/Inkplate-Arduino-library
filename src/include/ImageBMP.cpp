#include "Image.h"

void Image::readBmpHeaderSd(SdFile *_f, bitmapHeader *_h)
{
    uint8_t header[55];

    _f->rewind();
    _f->read(header, 55);

    uint16_t color = read16(header + 28);
    uint32_t totalColors = read32(header + 46);

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

bool Image::legalBmp(bitmapHeader *bmpHeader)
{
    return bmpHeader->signature == 0x4D42 && bmpHeader->compression == 0 &&
           (bmpHeader->color == 1 || bmpHeader->color == 4 || bmpHeader->color == 8 || bmpHeader->color == 16 ||
            bmpHeader->color == 24 || bmpHeader->color == 32);
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
            writePixel(x + j, y, (invert ^ (pallete[0] < pallete[1])) ^ !!(pixelBuffer[j >> 3] & (1 << (7 - j & 7))));
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

    if (!legalBmp(&bmpHeader))
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
        int16_t n = rowSize(w, c);

        p->read(pixelBuffer, n);
        displayBmpLine(x, y + bmpHeader.height - i, &bmpHeader, dither, invert);
    }
    return 1;
}

bool Image::drawBitmapFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    uint8_t *buf = downloadFile(url, 800 * 600 * 4);

    bitmapHeader bmpHeader;
    readBmpHeader(buf, &bmpHeader);

    if (!legalBmp(&bmpHeader))
        return 0;

    if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT)
        selectDisplayMode(INKPLATE_1BIT);

    if ((bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 16 || bmpHeader.color == 24 ||
         bmpHeader.color == 32) &&
        getDisplayMode() != INKPLATE_3BIT)
        selectDisplayMode(INKPLATE_3BIT);

    uint8_t *bufferPtr = buf + bmpHeader.startRAW;
    for (int i = 0; i < bmpHeader.height; ++i)
    {
        memcpy(pixelBuffer, bufferPtr, rowSize(bmpHeader.width, bmpHeader.color));
        displayBmpLine(x, y + bmpHeader.height - i, &bmpHeader, dither, invert);
        bufferPtr += rowSize(bmpHeader.width, bmpHeader.color);
    }

    free(buf);

    return 1;
}
