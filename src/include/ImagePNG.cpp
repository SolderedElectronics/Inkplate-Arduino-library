/*
ImagePNG.cpp
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

#include "../libs/pngle/pngle.h"
#include "defines.h"

extern Image *_imagePtrPng;

static bool _pngInvert = 0;
static bool _pngDither = 0;
static int16_t lastY = -1;
static int16_t _pngX = 0;
static int16_t _pngY = 0;

void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    if (rgba[3])
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
            {
                uint8_t px = RGB3BIT(rgba[0], rgba[1], rgba[2]);
                if (_pngDither)
                    px = _imagePtrPng->ditherGetPixelBmp(RGB8BIT(rgba[0], rgba[1], rgba[2]), x + i,
                                                         _imagePtrPng->width(), 0);
                if (_pngInvert)
                    px = 7 - px;
                if (_imagePtrPng->getDisplayMode() == INKPLATE_1BIT)
                    px = (~px >> 2) & 1;
                _imagePtrPng->drawPixel(_pngX + x + i, _pngY + y + j, px);
            }
    if (lastY != y)
    {
        lastY = y;
        _imagePtrPng->ditherSwap(_imagePtrPng->width());
    }
}

bool Image::drawPngFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
    {
        return drawPngFromSd(&dat, x, y, dither, invert);
    }
    return 0;
}

bool Image::drawPngFromSd(SdFile *p, int x, int y, bool dither, bool invert)
{
    _pngDither = dither;
    _pngInvert = invert;
    lastY = y;

    bool ret = 1;
    uint32_t remain = 0;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    pngle_t *pngle = pngle_new();
    _pngX = x;
    _pngY = y;
    pngle_set_draw_callback(pngle, pngle_on_draw);
    uint32_t total = p->fileSize();
    uint8_t buff[2048];
    uint32_t pnt = 0;

    while (pnt < total)
    {
        uint32_t toread = p->available();
        if (toread > 0)
        {
            int len = p->read(buff, min((uint32_t)2048, toread));
            int fed = pngle_feed(pngle, buff, len);
            if (fed < 0)
            {
                ret = 0;
                break;
            }
            remain = remain + len - fed;
            pnt += len;
        }
    }

    p->close();
    pngle_destroy(pngle);
    return ret;
}

bool Image::drawPngFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    _pngDither = dither;
    _pngInvert = invert;
    lastY = y;

    bool ret = 1;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    pngle_t *pngle = pngle_new();
    _pngX = x;
    _pngY = y;
    pngle_set_draw_callback(pngle, pngle_on_draw);

    int32_t defaultLen = 800 * 600 * 4 + 100;
    uint8_t *buff = downloadFile(url, &defaultLen);

    if (!buff)
        return 0;

    if (pngle_feed(pngle, buff, defaultLen) < 0)
        ret = 0;

    pngle_destroy(pngle);
    free(buff);
    return ret;
}

bool Image::drawPngFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither, bool invert)
{
    _pngDither = dither;
    _pngInvert = invert;
    lastY = y;

    bool ret = 1;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    pngle_t *pngle = pngle_new();
    _pngX = x;
    _pngY = y;
    pngle_set_draw_callback(pngle, pngle_on_draw);

    uint8_t *buff = downloadFile(s, len);

    if (!buff)
        return 0;

    if (pngle_feed(pngle, buff, len) < 0)
        ret = 0;
    pngle_destroy(pngle);

    free(buff);
    return ret;
}