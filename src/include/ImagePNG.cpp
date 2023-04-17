/**
 **************************************************
 * @file        ImagePNG.cpp
 * @brief       Basic functionalities to work with PNG images
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered.com
 ***************************************************/

#include "Image.h"

#include "../libs/pngle/pngle.h"
#include "defines.h"

extern Image *_imagePtrPng;

static bool _pngInvert = 0;
static bool _pngDither = 0;
static int16_t lastY = -1;
static uint16_t _pngX = 0;
static uint16_t _pngY = 0;
static Image::Position _pngPosition = Image::_npos;

/**
 * @brief       pngle_on_draw
 *
 * @param       pngle_t *pngle
 *              pointer to image
 * @param       uint32_t x
 *              x plane position
 * @param       uint32_t y
 *              y plane position
 * @param       uint32_t w
 *              image width
 * @param       uint32_t h
 *              image height
 * @param       uint8_t rgba[4]
 *              color
 */
void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    if (_pngPosition != Image::_npos)
    {
        _imagePtrPng->getPointsForPosition(_pngPosition, pngle_get_width(pngle), pngle_get_height(pngle), E_INK_WIDTH,
                                           E_INK_HEIGHT, &_pngX, &_pngY);
        lastY = _pngY;
        _pngPosition = Image::_npos;
    }
    if (rgba[3])
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
            {
                uint8_t r = rgba[0];
                uint8_t g = rgba[1];
                uint8_t b = rgba[2];

                pngle_ihdr_t *ihdr = pngle_get_ihdr(pngle);

                if (ihdr->depth == 1)
                    r = g = b = (b ? 0xFF : 0);

#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
                if (_pngInvert)
                {
                    r = 255 - r;
                    g = 255 - g;
                    b = 255 - b;
                }

                uint8_t px = _imagePtrPng->findClosestPalette((r << 16) | (g << 8) | (b));
#else
                uint8_t px = RGB3BIT(r, g, b);
#endif

                if (_pngDither)
                {
#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
                    px = _imagePtrPng->ditherGetPixelBmp((r << 16) | (g << 8) | (b), x + i, y + j,
                                                         _imagePtrPng->width(), 0);
#else
                    px = _imagePtrPng->ditherGetPixelBmp(RGB8BIT(r, g, b), x + i, y + j, _imagePtrPng->width(), 0);
                    if (_pngInvert)
                        px = 7 - px;
                    if (_imagePtrPng->getDisplayMode() == INKPLATE_1BIT)
                        px = (~px >> 2) & 1;
#endif
                }
                _imagePtrPng->drawPixel(_pngX + x + i, _pngY + y + j, px);
            }
    if (lastY != y)
    {
        lastY = y;
        _imagePtrPng->ditherSwap(_imagePtrPng->width());
    }
}

/**
 * @brief       drawPngFromSd function draws png image from sd file
 *
 * @param       char *fileName
 *              pointer to png file
 * @param       int x
 *              x position for top left image corner
 * @param       int y
 *              y position for top left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawPngFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
    {
        return drawPngFromSd(&dat, x, y, dither, invert);
    }
    return 0;
}

/**
 * @brief       drawPngFromSd function draws png image from sd file
 *
 * @param       SdFile *p
 *              pointer to png file
 * @param       int x
 *              x position for top left image corner
 * @param       int y
 *              y position for top left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
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

/**
 * @brief       drawPngFromWeb function draws png image from web
 *
 * @param       char *url
 *              pointer to png file
 * @param       int x
 *              x position for top left image corner
 * @param       int y
 *              y position for top left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
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

    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 8 + 100;
    uint8_t *buf = 0;

    if (strncmp(url, "http://", 7) == 0)
    {
        buf = downloadFile(url, &defaultLen);
    }
    else if (strncmp(url, "https://", 8) == 0)
    {
        buf = downloadFileHTTPS(url, &defaultLen);
    }

    if (!buf)
        return 0;

    if (pngle_feed(pngle, buf, defaultLen) < 0)
        ret = 0;

    pngle_destroy(pngle);
    free(buf);
    return ret;
}

/**
 * @brief       drawPngFromWeb function draws png image from sd file
 *
 * @param       WiFiClient *s
 *              pointer to png file
 * @param       int x
 *              x position for top left image corner
 * @param       int y
 *              y position for top left image corner
 * @param       int32_t len
 *              Image length
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
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

/**
 * @brief       drawPngFromWebAtPosition function draws png image from web at
 * screen position
 *
 * @param       char *url
 *              pointer to png file
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight,
 * bottomRight, _npos)
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawPngFromWebAtPosition(const char *url, const Position &position, const bool dither, const bool invert)
{
    _pngDither = dither;
    _pngInvert = invert;

    bool ret = 1;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    pngle_t *pngle = pngle_new();

    _pngPosition = position;
    pngle_set_draw_callback(pngle, pngle_on_draw);

    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4 + 100;
    uint8_t *buff = downloadFile(url, &defaultLen);

    if (!buff)
        return 0;

    if (pngle_feed(pngle, buff, defaultLen) < 0)
        ret = 0;

    pngle_destroy(pngle);
    free(buff);
    _pngPosition = _npos;

    return ret;
}

/**
 * @brief       drawPngFromSdAtPosition function draws png image from sd card at
 * screen position
 *
 * @param       char *fileName
 *              pointer to png file
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight,
 * bottomRight, _npos)
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 *
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawPngFromSdAtPosition(const char *fileName, const Position &position, const bool dither,
                                    const bool invert)
{
    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
    {
        return 0;
    }

    _pngDither = dither;
    _pngInvert = invert;

    bool ret = 1;
    uint32_t remain = 0;

    if (dither)
        memset(ditherBuffer, 0, sizeof ditherBuffer);

    pngle_t *pngle = pngle_new();

    _pngPosition = position;
    pngle_set_draw_callback(pngle, pngle_on_draw);
    uint32_t total = dat.fileSize();
    uint8_t buff[2048];
    uint32_t pnt = 0;

    while (pnt < total)
    {
        uint32_t toread = dat.available();
        if (toread > 0)
        {
            int len = dat.read(buff, min((uint32_t)2048, toread));
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

    dat.close();
    pngle_destroy(pngle);
    _pngPosition = _npos;

    return ret;
}