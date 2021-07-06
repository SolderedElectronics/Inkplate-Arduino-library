/**
 **************************************************
 * @file        Image.cpp
 * @brief       Basic functionalities to work with images
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
#include "pgmspace.h"

Image *_imagePtrJpeg = nullptr;
Image *_imagePtrPng = nullptr;

/**
 * @brief       Image sets pointers to selected image
 */
Image::Image(int16_t w, int16_t h) : Adafruit_GFX(w, h)
{
    _imagePtrJpeg = this;
    _imagePtrPng = this;
}

/**
 * @brief       drawImage function draws image from string path
 * 
 * @param       String path
 *              path to image file
 * @param       int x
 *              x position for upper left image corner
 * @param       int y
 *              y position for upper left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if inverting, 0 if not
 *
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const String path, int x, int y, bool dither, bool invert)
{
    return drawImage(path.c_str(), x, y, dither, invert);
};

/**
 * @brief       drawImage function draws image from char path
 * 
 * @param       char *path
 *              char pointer path to image file
 * @param       int x
 *              x position for upper left image corner
 * @param       int y
 *              y position for upper left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if inverting, 0 if not
 *
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const char *path, int x, int y, bool dither, bool invert)
{
    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        if (strstr(path, ".bmp") != NULL || strstr(path, ".dib") != NULL)
            return drawBitmapFromWeb(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromWeb(path, x, y, dither, invert);
        if (strstr(path, ".png") != NULL)
            return drawPngFromWeb(path, x, y, dither, invert);
    }
    else
    {
        if (strstr(path, ".bmp") != NULL || strstr(path, ".dib") != NULL)
            return drawBitmapFromSd(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromSd(path, x, y, dither, invert);
        if (strstr(path, ".png") != NULL)
            return drawPngFromSd(path, x, y, dither, invert);
    }
    return 0;
};

/**
 * @brief       drawImage function draws image from buffer
 * 
 * @param       uint8_t *buf
 *              char pointer path to image file
 * @param       int x
 *              x position for upper left image corner
 * @param       int y
 *              y position for upper left image corner
 * @param       bool w
 *              image width
 * @param       bool h
 *              image height
 * @param       bool c
 *              image color
 * @param       bool bg
 *              16 bit color to draw background with
 * 
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c, uint8_t bg)
{
#ifdef ARDUINO_INKPLATECOLOR
    drawBitmap3Bit(x, y, buf, w, h);
    return 1;
#else
    if (getDisplayMode() == INKPLATE_1BIT && bg == 0xFF)
        drawBitmap(x, y, buf, w, h, c);
    else if (getDisplayMode() == INKPLATE_1BIT && bg != 0xFF)
        drawBitmap(x, y, buf, w, h, c, bg);
    else if (getDisplayMode() == INKPLATE_3BIT)
        drawBitmap3Bit(x, y, buf, w, h);
    return 1;
#endif
}

/**
 * @brief       drawImage function draws image from string path
 * 
 * @param       String path
 *              path to image file
 * @param       Format &format
 *              Image format (bmp, jpg, png)
 * @param       int x
 *              x position for upper left image corner
 * @param       int y
 *              y position for upper left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if inverting, 0 if not
 *
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const String path, const Format &format, const int x, const int y, const bool dither,
                      const bool invert)
{
    return drawImage(path.c_str(), format, x, y, dither, invert);
};

/**
 * @brief       drawImage function draws image from string path
 * 
 * @param       char *path
 *              path to image file
 * @param       Format &format
 *              Image format (bmp, jpg, png)
 * @param       int x
 *              x position for upper left image corner
 * @param       int y
 *              y position for upper left image corner
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if inverting, 0 if not
 *
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const char *path, const Format &format, const int x, const int y, const bool dither,
                      const bool invert)
{
    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        if (format == BMP)
            return drawBitmapFromWeb(path, x, y, dither, invert);
        if (format == JPG)
            return drawJpegFromWeb(path, x, y, dither, invert);
        if (format == PNG)
            return drawPngFromWeb(path, x, y, dither, invert);
    }
    else
    {
        if (format == BMP)
            return drawBitmapFromSd(path, x, y, dither, invert);
        if (format == JPG)
            return drawJpegFromSd(path, x, y, dither, invert);
        if (format == PNG)
            return drawPngFromSd(path, x, y, dither, invert);
    }
    return 0;
}

/**
 * @brief       drawImage function draws image from string path
 * 
 * @param       char *path
 *              path to image file
 * @param       Format &format
 *              Image format (bmp, jpg, png)
 * @param       Position position
 *              Image position (center, topLeft, bottomLeft, topRight, bottomRight, _npos)
 * @param       bool dither
 *              1 if using dither, 0 if not
 * @param       bool invert
 *              1 if inverting, 0 if not
 *
 * @return      1 if succesfuly drawn, 0 if not             
 */
bool Image::drawImage(const char *path, const Format &format, const Position &position, const bool dither,
                      const bool invert)
{
    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        if (format == JPG)
            return drawJpegFromWebAtPosition(path, position, dither, invert);
        else if (format == PNG)
            return drawPngFromWebAtPosition(path, position, dither, invert);
        else if (format == BMP)
            return drawBmpFromWebAtPosition(path, position, dither, invert);
        return false;
    }
    else
    {
        if (format == JPG)
            return drawJpegFromSdAtPosition(path, position, dither, invert);
        else if (format == PNG)
            return drawPngFromSdAtPosition(path, position, dither, invert);
        else if (format == BMP)
            return drawBmpFromSdAtPosition(path, position, dither, invert);
        return false;
    }
    return false;
}

/**
 * @brief       drawBitmap3Bit function draws bitmap 3 bit image
 * 
 * @param       int16_t _x
 *              x position for image top left cornere
 * @param       int16_t _y
 *              x position for image top left cornere
 * @param       int16_t _w
 *              Image width
 * @param       int16_t _h
 *              Image height
 */
void Image::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h)
{
#ifndef ARDUINO_INKPLATECOLOR
    if (getDisplayMode() != INKPLATE_3BIT)
        return;
#endif

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
