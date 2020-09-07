#include "Image.h"

#include "../libs/TJpeg/TJpg_Decoder.h"

#define RED(a)   ((((a)&0xf800) >> 11) << 3)
#define GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define BLUE(a)  (((a)&0x001f) << 3)

#define RGB3BIT(r, g, b) ((54UL * (r) + 183UL * (g) + 19UL * (b)) >> 13)

extern Image *_imagePtrJpeg;

bool Image::drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool _dither, bool _invert)
{
    if (!_imagePtrJpeg)
        return 0;

    _imagePtrJpeg->startWrite();
    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            uint16_t rgb = bitmap[j * w + i];
            if (_invert)
                _imagePtrJpeg->drawPixel(i + x, j + y, 7 - RGB3BIT(RED(rgb), GREEN(rgb), BLUE(rgb)));
            else
                _imagePtrJpeg->drawPixel(i + x, j + y, RGB3BIT(RED(rgb), GREEN(rgb), BLUE(rgb)));
        }
    }
    _imagePtrJpeg->endWrite();

    return 1;
}

bool Image::drawJpegFromSD(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
        return drawJpegFromSD(&dat, x, y, dither, invert);
    return 0;
}

bool Image::drawJpegFromSD(SdFile *p, int x, int y, bool dither, bool invert)
{
    uint8_t ret = 0;

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(drawJpegChunk);

    uint32_t pnt = 0;
    uint32_t total = p->fileSize();
    uint8_t *buf = (uint8_t *)ps_malloc(total);

    if (buf == NULL)
        return 0;

    while (pnt < total)
    {
        uint32_t toread = p->available();
        if (toread > 0)
        {
            int read = p->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }
    p->close();

    selectDisplayMode(INKPLATE_3BIT);

    if (TJpgDec.drawJpg(x, y, buf, total, dither, invert) == 0)
        ret = 1;

    free(buf);

    return ret;
}