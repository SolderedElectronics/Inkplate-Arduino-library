/**
 **************************************************
 * @file        ImageJPEG.cpp
 * @brief       Basic functionalities to work with JPEG images
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact assistance@soldered.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE2) && !defined(ARDUINO_INKPLATE13SPECTRA) &&        \
    !defined(ARDUINO_ESP32S3_DEV)
#include "../../../system/defines.h"
#include "Inkplate.h"
#include "../Image.h"
#include "../../Tjpeg/TJpg_Decoder.h"


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

    bool ret = drawJpegFromBuffer(buff, total, x, y, dither, invert);
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
    uint8_t *buf = 0;

    if (strncmp(url, "http://", 7) == 0)
    {
        buf = _inkplate->downloadFile(url, &defaultLen);
    }
    else if (strncmp(url, "https://", 8) == 0)
    {
        buf = _inkplate->downloadFileHTTPS(url, &defaultLen);
    }

    // Image sometimes doesn't download, so
    // check if buffer is empty to avoid trying to draw an empty image
    if (!buf)
        return 0;

    ret = drawJpegFromBuffer(buf, defaultLen, x, y, dither, invert);
    free(buf);

    return ret;
}

/**
 * @brief       drawJpegFromWebAtPosition function draws jpeg image from web at
 * screen position
 *
 * @param       char *url
 *              pointer to jpeg file
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
bool Image::drawJpegFromWebAtPosition(const char *url, const Position &position, const bool dither, const bool invert)
{
    bool ret = 0;

    int32_t defaultLen = E_INK_WIDTH * E_INK_HEIGHT * 4;

    uint8_t *buff = 0;
    if (strncmp(url, "http://", 7) == 0)
    {
        buff = _inkplate->downloadFile(url, &defaultLen);
    }
    else if (strncmp(url, "https://", 8) == 0)
    {
        buff = _inkplate->downloadFileHTTPS(url, &defaultLen);
    }

    // Image sometimes doesn't download, so
    // check if buffer is empty to avoid trying to draw an empty image
    if (!buff)
        return 0;

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
 * @brief       drawJpegFromSdAtPosition function draws jpeg image from sd card
 * at screen position
 *
 * @param       char *fileName
 *              pointer to jpeg file
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
bool Image::drawJpegFromSdAtPosition(const char *fileName, const Position &position, const bool dither,
                                     const bool invert)
{
    SdFile dat;
    if (!dat.open(fileName, O_RDONLY))
        return 0;

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

    uint16_t w = 0;
    uint16_t h = 0;
    TJpgDec.setJpgScale(1);
    JRESULT r = TJpgDec.getJpgSize(&w, &h, buff, total);
    if (r != JDR_OK)
    {
        free(buff);
        return 0;
    }

    uint16_t posX, posY;
    getPointsForPosition(position, w, h, E_INK_WIDTH, E_INK_HEIGHT, &posX, &posY);

    bool ret = drawJpegFromBuffer(buff, total, posX, posY, dither, invert);
    free(buff);
    return ret;
}

/**
 * @brief       drawJpegFromWeb function draws jpeg image from web
 *
 * @param       WiFiClient *s
 *              pointer to jpeg file on web
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight,
 * bottomRight, _npos)
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
    uint8_t *buff = _inkplate->downloadFile(s, len);

    // Image sometimes doesn't download, so
    // check if buffer is empty to avoid trying to draw an empty image
    if (!buff)
        return 0;

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
    blockW = -1;
    blockH = -1;
    lastY = -1;
    jpegMcuH = 0;
    jpegDrawX = x;
    jpegDrawY = y;
    jpegDither = dither;
    jpegInvert = invert;

    memset(ditherBuffer, 0, ditherBufferSizeBytes);

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(drawJpegChunk);

    uint16_t imgW = 0, imgH = 0;
    if (TJpgDec.getJpgSize(&imgW, &imgH, buff, len) != JDR_OK)
        return false;
    jpegImageWidth = imgW;

    // Allocate row buffer: one MCU row wide (max 16 rows tall) at full image width
    jpegRowBuffer = (uint16_t *)ps_malloc(jpegImageWidth * 16 * sizeof(uint16_t));
    if (!jpegRowBuffer)
        return false;

    int err = TJpgDec.drawJpg(x, y, buff, len, dither, invert);

    // Flush the final MCU row which never triggered a y-change flush
    if (jpegMcuH > 0 && lastY != -1)
        flushJpegRow(lastY);

    free(jpegRowBuffer);
    jpegRowBuffer = nullptr;

    return err == 0;
}

/**
 * @brief       drawJpegChunk stores one MCU block into the row buffer.
 *              When the y coordinate changes, the completed row is flushed
 *              (dithered and drawn) before accepting the new block.
 *
 * @param       int16_t x         screen x of this block
 * @param       int16_t y         screen y of this block
 * @param       uint16_t w        block width
 * @param       uint16_t h        block height
 * @param       uint16_t *bitmap  RGB565 pixels
 * @param       bool dither
 * @param       bool invert
 */
bool Image::drawJpegChunk(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap, bool dither, bool invert)
{
    if (!_imagePtrJpeg || !_imagePtrJpeg->jpegRowBuffer)
        return false;

    // Record MCU height on the first block
    if (_imagePtrJpeg->jpegMcuH == 0)
        _imagePtrJpeg->jpegMcuH = h;

    // New row of MCU blocks — flush what we have before accepting new data
    if (_imagePtrJpeg->lastY == -1)
    {
        _imagePtrJpeg->lastY = y;
    }
    else if (y != _imagePtrJpeg->lastY)
    {
        _imagePtrJpeg->flushJpegRow(_imagePtrJpeg->lastY);
        _imagePtrJpeg->lastY = y;
    }

    // Copy pixels into the row buffer at the correct column offset
    int colOffset = x - _imagePtrJpeg->jpegDrawX;
    for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i)
            _imagePtrJpeg->jpegRowBuffer[j * _imagePtrJpeg->jpegImageWidth + colOffset + i] = bitmap[j * w + i];

    return 1;
}

/**
 * @brief       flushJpegRow dithers and draws one buffered MCU row.
 *              Called once per row of MCU blocks so dithering sees the full
 *              image width before advancing to the next row.
 *
 * @param       rowY  absolute screen y of the top of this MCU row
 */
void Image::flushJpegRow(int rowY)
{
    for (int j = 0; j < jpegMcuH; ++j)
    {
        for (int col = 0; col < jpegImageWidth; ++col)
        {
            uint32_t rgb = jpegRowBuffer[j * jpegImageWidth + col];
            uint8_t r = _RED(rgb), g = _GREEN(rgb), b = _BLUE(rgb);

            uint32_t val;
            if (jpegDither)
                val = ditherGetPixelBmp(RGB8BIT(r, g, b), col, rowY + j, jpegImageWidth, false);
            else
                val = RGB3BIT(r, g, b);

            if (jpegInvert)
                val = 7 - val;
            if (_inkplate->getDisplayMode() == INKPLATE_1BIT)
                val = (~val >> 2) & 1;

            _inkplate->drawPixel(jpegDrawX + col, rowY + j, val);
        }
    }
}
#endif
