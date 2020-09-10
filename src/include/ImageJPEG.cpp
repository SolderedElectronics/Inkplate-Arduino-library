#include "Image.h"

#include "../libs/TJpeg/TJpg_Decoder.h"

#define RED(a)   ((((a)&0xf800) >> 11) << 3)
#define GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define BLUE(a)  (((a)&0x001f) << 3)

extern Image *_imagePtrJpeg;

bool Image::drawJpegFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
        return drawJpegFromSd(&dat, x, y, dither, invert);
    return 0;
}

bool Image::drawJpegFromSd(SdFile *p, int x, int y, bool dither, bool invert)
{
    uint8_t ret = 0;

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(drawJpegChunk);

    uint32_t pnt = 0;
    uint32_t total = p->fileSize();
    uint8_t *buff = (uint8_t *)ps_malloc(total);

    if (buff == NULL)
        return 0;

    while (pnt < total)
    {
        uint32_t toread = p->available();
        if (toread > 0)
        {
            int read = p->read(buff + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }
    p->close();

    if (TJpgDec.drawJpg(x, y, buff, total, dither, invert) == 0)
        ret = 1;

    free(buff);

    return ret;
}

bool Image::drawJpegFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    bool ret = 0;

    int32_t defaultLen = 800 * 600 * 4;
    uint8_t *buff = downloadFile(url, &defaultLen);
    ret = drawJpegFromBuffer(buff, x, y, defaultLen, dither, invert);
    free(buff);

    return ret;
}

bool Image::drawJpegFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither, bool invert)
{
    bool ret = 0;
    uint8_t *buff = downloadFile(s, len);
    ret = drawJpegFromBuffer(buff, x, y, len, dither, invert);
    free(buff);

    return ret;
}

bool Image::drawJpegFromBuffer(uint8_t *buff, int32_t len, int x, int y, bool dither, bool invert)
{
    bool ret = 0;

    if (TJpgDec.drawJpg(x, y, buff, len, dither, invert) == 0)
        ret = 1;

    return ret;
};

bool Image::drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert)
{
    if (!_imagePtrJpeg)
        return 0;

    _imagePtrJpeg->startWrite();
    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            uint16_t rgb = bitmap[j * w + i];
            uint8_t val = RGB3BIT(RED(rgb), GREEN(rgb), BLUE(rgb));

            if (invert)
                val = 7 - val;

            _imagePtrJpeg->writePixel(x + i, y + j, val);
        }
    }
    _imagePtrJpeg->endWrite();

    return 1;
}