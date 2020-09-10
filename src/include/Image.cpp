#include "Image.h"

#include "../libs/TJpeg/TJpg_Decoder.h"
#include "pgmspace.h"

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
            return drawBitmapFromSd(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromSd(path, x, y, dither, invert);
    }
};

bool Image::drawImage(const SdFile *path, int x, int y, bool dither, bool invert){

};

bool Image::drawImage(const WiFiClient *s, int x, int y, int len, bool dither, bool invert){

};

void Image::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
    int16_t byteWidth = (w + 7) >> 3; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
                byte <<= 1;
            else
                byte = bitmap[j * byteWidth + (i >> 3)];

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
