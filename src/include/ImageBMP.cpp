#include "Image.h"

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

            Serial.println(c, 2);

            uint8_t r = (c & 0xFF000000) >> 24;
            uint8_t g = (c & 0x00FF0000) >> 16;
            uint8_t b = (c & 0x0000FF00) >> 8;
            uint8_t a = (c & 0x000000FF);

            Serial.println(b, 2);
            Serial.println(g, 2);
            Serial.println(r, 2);
            Serial.println(a, 2);

            pallete[i >> 1] |= ((54UL * r + 183UL * g + 19UL * b) >> 13) << (i & 1 ? 0 : 4);
        }
    }

    for (int i = 0; i < totalColors / 2; ++i)
    {
        Serial.println(pallete[i] >> 8);
        Serial.println(pallete[i] & 0xF);
    }

    Serial.println();

    return;
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

    p->seekSet(bmpHeader.startRAW);

    if (bmpHeader.color == 1)
        Serial.println("nice");

    for (int i = 0; i < h; ++i)
        displayBmpLine(x, y + i, p, &bmpHeader, dither, invert);

    return 1;
}

void Image::displayBmpLine(int16_t x, int16_t y, SdFile *f, bitmapHeader *bmpHeader, bool dither, bool invert)
{
    int16_t w = bmpHeader->width, h = bmpHeader->height;
    int8_t c = bmpHeader->color;

    int16_t rowSize = (((int16_t)c * w + 31) >> 5) << 2;

    startWrite();

    f->read(pixelBuffer, rowSize);
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
                writePixel(x + j, y, 7 - ((54UL * r + 183UL * g + 19UL * b) >> 13));
            else
                writePixel(x + j, y, (54UL * r + 183UL * b + 19UL * g) >> 13);
            break;
        }
        case 24: {
            uint8_t r = pixelBuffer[j * 3];
            uint8_t g = pixelBuffer[j * 3 + 1];
            uint8_t b = pixelBuffer[j * 3 + 2];

            if (invert)
                writePixel(x + j, y, 7 - ((54UL * r + 183UL * g + 19UL * b) >> 13));
            else
                writePixel(x + j, y, (54UL * r + 183UL * b + 19UL * g) >> 13);
            break;
        }
        case 32:
            uint8_t r = pixelBuffer[j * 4];
            uint8_t g = pixelBuffer[j * 4 + 1];
            uint8_t b = pixelBuffer[j * 4 + 2];

            if (invert)
                writePixel(x + j, y, 7 - ((54UL * r + 183UL * g + 19UL * b) >> 13));
            else
                writePixel(x + j, y, (54UL * r + 183UL * b + 19UL * g) >> 13);
            break;
        }
    }

    endWrite();
}

// bool Image::drawMonochromeBitmapSd(SdFile *f, bitmapHeader bmpHeader, int x, int y, bool invert)
// {
//     int w = bmpHeader.width;
//     int h = bmpHeader.height;
//     uint8_t paddingBits = w % 32;
//     w /= 32;


//     int i, j;
//     for (j = 0; j < h; j++)
//     {
//         uint8_t *bufferPtr = pixelBuffer;

//         for (i = 0; i < w; i++)
//         {
//             uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
//             if (invert)
//                 pixelRow = ~pixelRow;
//             for (int n = 0; n < 32; n++)
//             {
//                 drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
//             }
//         }
//         if (paddingBits)
//         {
//             uint32_t pixelRow = f->read() << 24 | f->read() << 16 | f->read() << 8 | f->read();
//             if (invert)
//                 pixelRow = ~pixelRow;
//             for (int n = 0; n < paddingBits; n++)
//             {
//                 drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
//             }
//         }
//     }
//     f->close();
//     return 1;
// }

// bool Image::drawGrayscaleBitmap4Sd(SdFile *f, bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
// {
//     int w = bmpHeader.width;
//     int h = bmpHeader.height;
//     uint8_t paddingBits = w % 8;
//     w /= 8;

//     f->seekSet(bmpHeader.startRAW);
//     int i, j;

//     uint8_t *bufferPtr;

//     if (dither)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w * 4 + (paddingBits ? 4 : 0));

//         ditherStart(pixelBuffer, bufferPtr, w * 8 + (paddingBits ? 4 : 0), invert, 4);
//     }

//     for (j = 0; j < h; j++)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w * 4 + (paddingBits ? 4 : 0));

//         if (dither && j != h - 1)
//         {
//             ditherLoadNextLine(pixelBuffer, bufferPtr, w * 8 + (paddingBits ? 4 : 0), invert, 4);
//         }

//         for (i = 0; i < w; i++)
//         {
//             if (dither)
//             {

//                 for (int n = 0; n < 8; n++)
//                 {
//                     drawPixel((i * 8) + n + x, h - 1 - j + y,
//                               ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
//                 }
//             }
//             else
//             {
//                 uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 |
//                 *(bufferPtr++); if (invert)
//                     pixelRow = ~pixelRow;
//                 for (int n = 0; n < 8; n++)
//                 {
//                     drawPixel((i * 8) + n + x, h - 1 - j + y,
//                               (pixelRow & (0xFULL << (28 - n * 4))) >> (28 - n * 4 + 1));
//                 }
//             }
//         }
//         if (paddingBits)
//         {
//             if (dither)
//             {
//                 for (int n = 0; n < paddingBits; n++)
//                 {
//                     drawPixel((i * 8) + n + x, h - 1 - j + y,
//                               ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
//                 }
//             }
//             else
//             {
//                 uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 |
//                 *(bufferPtr++); if (invert)
//                     pixelRow = ~pixelRow;
//                 for (int n = 0; n < paddingBits; n++)
//                 {
//                     drawPixel((i * 8) + n + x, h - 1 - j + y,
//                               ((pixelRow & (0xFULL << (28 - n * 4)))) >> (28 - n * 4 + 1));
//                 }
//             }
//         }
//         if (dither)
//             ditherSwap(w * 8 + paddingBits);
//     }
//     f->close();
//     return 1;
// }

// bool Image::drawGrayscaleBitmap8Sd(SdFile *f, bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
// {
//     int w = bmpHeader.width;
//     int h = bmpHeader.height;
//     char padding = w & 3;
//     f->seekSet(bmpHeader.startRAW);
//     int i, j;

//     uint8_t *bufferPtr;

//     if (dither)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w);

//         ditherStart(pixelBuffer, bufferPtr, w, invert, 8);
//     }

//     for (j = 0; j < h; j++)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w);

//         if (dither && j != h - 1)
//         {
//             ditherLoadNextLine(pixelBuffer, bufferPtr, w, invert, 8);
//         }

//         for (i = 0; i < w; i++)
//         {
//             if (dither)
//                 drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
//             else
//             {
//                 uint8_t px = 0;
//                 if (invert)
//                     px = 255 - *(bufferPtr++);
//                 else
//                     px = *(bufferPtr++);
//                 drawPixel(i + x, h - 1 - j + y, px >> 5);
//             }
//         }
//     }

//     if (dither)
//         ditherSwap(w);

//     if (padding)
//     {
//         for (int p = 0; p < 4 - padding; p++)
//         {
//             f->read();
//         }
//     }
//     f->close();
//     return 1;
// }

// bool Image::drawGrayscaleBitmap24Sd(SdFile *f, bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
// {
//     int w = bmpHeader.width;
//     int h = bmpHeader.height;
//     char padding = w & 3;
//     f->seekSet(bmpHeader.startRAW);
//     int i, j;

//     uint8_t *bufferPtr;

//     if (dither)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w * 3);

//         ditherStart(pixelBuffer, bufferPtr, w, invert, 24);
//     }

//     for (j = 0; j < h; j++)
//     {
//         bufferPtr = pixelBuffer;
//         f->read(pixelBuffer, w * 3);

//         if (dither && j != h - 1)
//         {
//             ditherLoadNextLine(pixelBuffer, bufferPtr, w, invert, 24);
//         }

//         for (i = 0; i < w; i++)
//         {
//             if (dither)
//                 drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
//             else
//             {
//                 uint8_t px = 0;
//                 if (invert)
//                     px = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) +
//                          ((255 - *(bufferPtr++)) * 722 / 10000);
//                 else
//                     px = (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) +
//                          (*(bufferPtr++) * 722 / 10000);
//                 drawPixel(i + x, h - 1 - j + y, px >> 5);
//             }
//         }

//         if (dither)
//             ditherSwap(w);

//         if (padding)
//         {
//             for (int p = 0; p < padding; p++)
//             {
//                 f->read();
//             }
//         }
//     }
//     f->close();
//     return 1;
// }