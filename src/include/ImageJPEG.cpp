/**
 **************************************************
 * @file        ImageJPEG.cpp
 * @brief       Basic functionalities to work with JPEG images
 * 
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example.
 *              If you have any questions about licensing, please contact techsupport@e-radionica.com
 *              Distributed as-is; no warranty is given.
 * 
 * @authors     e-radionica.com
 ***************************************************/

#include "Image.h"

#include "../libs/TJpeg/TJpg_Decoder.h"
#include "defines.h"

extern Image *_imagePtrJpeg;

/**
 * @brief       drawJpegFromSd function draws jpeg image from sd file
 * 
 * @param       char *fileName
 *              pointer to jpeg file
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
bool Image::drawJpegFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
        return drawJpegFromSd(&dat, x, y, dither, invert);
    return 0;
}

/**
 * @brief       drawJpegFromSd function draws jpeg image from sd file
 * 
 * @param       SdFile *p
 *              pointer to jpeg file
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
bool Image::drawJpegFromSd(SdFile *p, int x, int y, bool dither, bool invert)
{
    uint8_t ret = 0;

    blockW = -1;
    blockH = -1;
    lastY = -1;
    memset(ditherBuffer, 0, sizeof ditherBuffer);

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

/**
 * @brief       drawJpegFromWeb function draws jpeg image from web
 * 
 * @param       char *url
 *              pointer to jpeg file
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
bool Image::drawJpegFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    bool ret = 0;

    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4;
    uint8_t *buff = NetworkClient::downloadFile(url, &defaultLen);

    ret = drawJpegFromBuffer(buff, defaultLen, x, y, dither, invert);
    free(buff);

    return ret;
}

/**
 * @brief       drawJpegFromWebAtPosition function draws jpeg image from web at screen position
 * 
 * @param       char *url
 *              pointer to jpeg file
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight, bottomRight, _npos)
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 * 
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawJpegFromWebAtPosition(const char *url, const Position &position, const bool dither, const bool invert)
{
    bool ret = 0;

    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4;
    uint8_t *buff = NetworkClient::downloadFile(url, &defaultLen);

    uint16_t w = 0;
    uint16_t h = 0;
    TJpgDec.setJpgScale(1);
    JRESULT r = TJpgDec.getJpgSize(&w, &h, buff, defaultLen);
    if (r != JDR_OK)
    {
        free(buff);
        return 0;
    }

    uint16_t posX, posY;
    getPointsForPosition(position, w, h, E_INK_WIDTH, E_INK_HEIGHT, &posX, &posY);

    ret = drawJpegFromBuffer(buff, defaultLen, posX, posY, dither, invert);
    free(buff);

    return ret;
}

/**
 * @brief       drawJpegFromSdAtPosition function draws jpeg image from sd card at screen position
 * 
 * @param       char *fileName
 *              pointer to jpeg file
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight, bottomRight, _npos)
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 * 
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawJpegFromSdAtPosition(const char *fileName, const Position &position, const bool dither,
                                     const bool invert)
{
    uint8_t ret = 0;

    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
        return 0;

    blockW = -1;
    blockH = -1;
    lastY = -1;
    memset(ditherBuffer, 0, sizeof ditherBuffer);

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(drawJpegChunk);

    uint32_t pnt = 0;
    uint32_t total = dat.fileSize();
    uint8_t *buff = (uint8_t *)ps_malloc(total);

    if (buff == NULL)
        return 0;

    while (pnt < total)
    {
        uint32_t toread = dat.available();
        if (toread > 0)
        {
            int read = dat.read(buff + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }
    dat.close();

    uint16_t posX, posY;

    uint16_t w = 0;
    uint16_t h = 0;
    JRESULT r = TJpgDec.getJpgSize(&w, &h, buff, total);
    if (r != JDR_OK)
    {
        free(buff);
        return 0;
    }

    getPointsForPosition(position, w, h, E_INK_WIDTH, E_INK_HEIGHT, &posX, &posY);

    if (TJpgDec.drawJpg(posX, posY, buff, total, dither, invert) == 0)
        ret = 1;

    free(buff);
    return ret;
}

/**
 * @brief       drawJpegFromWeb function draws jpeg image from web
 * 
 * @param       WiFiClient *s
 *              pointer to jpeg file on web
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight, bottomRight, _npos)
 * @param       int x
 *              x position for top left image corner
 * @param       int y
 *              y position for top left image corner
 * @param       int32_t len
 *              image length
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if using invert, 0 if not
 * 
 * @return      1 if drawn successfully, 0 if not
 */
bool Image::drawJpegFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither, bool invert)
{
    bool ret = 0;
    uint8_t *buff = NetworkClient::downloadFile(s, len);
    ret = drawJpegFromBuffer(buff, len, x, y, dither, invert);
    free(buff);

    return ret;
}

/**
 * @brief       drawJpegFromBuffer function draws jpeg image from buffer
 * 
 * @param       uint8_t *buff
 *              pointer to jpeg buffer file
 * @param       int32_t len
 *              image length
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
bool Image::drawJpegFromBuffer(uint8_t *buff, int32_t len, int x, int y, bool dither, bool invert)
{
    bool ret = 0;

    blockW = -1;
    blockH = -1;
    lastY = -1;

    memset(ditherBuffer, 0, sizeof ditherBuffer);

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(drawJpegChunk);

    int err = TJpgDec.drawJpg(x, y, buff, len, dither, invert);

    if (err == 0)
        ret = 1;

    return ret;
};

/**
 * @brief       drawJpegChunk draws one chunk of image
 * 
 * @param       int16_t x
 *              x plane starting point
 * @param       int16_t y
 *              y plane starting point
 * @param       int16_t w
 *              image width
 * @param       int16_t h
 *              image height
 * @param       int16_t *bitmap
 *              pointer to bitmap image
 * @param       int16_t dither
 *              1 if using dither, 0 if not
 * @param       int16_t invert
 *              1 if using invert, 0 if not
 */
bool Image::drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert)
{
    if (!_imagePtrJpeg)
        return 0;

    if (dither && y != _imagePtrJpeg->lastY)
    {
        _imagePtrJpeg->ditherSwap(E_INK_WIDTH);
        _imagePtrJpeg->lastY = y;
    }

    _imagePtrJpeg->startWrite();
    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            uint32_t rgb = bitmap[j * w + i];
            uint32_t val;

            uint8_t r = RED(rgb), g = GREEN(rgb), b = BLUE(rgb);

#ifdef ARDUINO_INKPLATECOLOR
            if (invert)
            {
                r = 255 - r;
                g = 255 - g;
                b = 255 - b;
            }

            if (dither)
            {
                val = _imagePtrJpeg->ditherGetPixelBmp(((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b), i + x,
                                                       j + y, _imagePtrJpeg->width(), 0);
            }
            else
            {
                val = _imagePtrJpeg->findClosestPalette(((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b));
            }

            _imagePtrJpeg->writePixel(x + i, y + j, val);
#else
            if (dither)
            {
                val = _imagePtrJpeg->ditherGetPixelJpeg(RGB8BIT(r, g, b), i, j, x, y, w, h);
            }
            else
            {
                val = RGB3BIT(r, g, b);
            }

            if (invert)
                val = 7 - val;
            if (_imagePtrJpeg->getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            _imagePtrJpeg->writePixel(x + i, y + j, val);
#endif
        }
    }

#ifndef ARDUINO_INKPLATECOLOR
    if (dither)
        _imagePtrJpeg->ditherSwapBlockJpeg(x);
    _imagePtrJpeg->endWrite();
#endif

    return 1;
}
