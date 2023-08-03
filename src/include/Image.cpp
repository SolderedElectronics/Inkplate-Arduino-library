/**
 **************************************************
 * @file        Image.cpp
 * @brief       Basic functionalities to work with images
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
 * @authors     Soldered
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
    // Try to get the file extension.
    char _fileExtension[5];
    if (!getFileExtension((char *)path, _fileExtension))
        return false;

    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        if (strstr(_fileExtension, "bmp") != NULL || strstr(_fileExtension, "dib") != NULL)
            return drawBitmapFromWeb(path, x, y, dither, invert);
        if (strstr(_fileExtension, "jpg") != NULL || strstr(_fileExtension, "jpeg") != NULL)
            return drawJpegFromWeb(path, x, y, dither, invert);
        if (strstr(_fileExtension, "png") != NULL)
            return drawPngFromWeb(path, x, y, dither, invert);
    }
    else
    {
        if (strstr(_fileExtension, "bmp") != NULL || strstr(_fileExtension, "dib") != NULL)
            return drawBitmapFromSd(path, x, y, dither, invert);
        if (strstr(_fileExtension, "jpg") != NULL || strstr(_fileExtension, "jpeg") != NULL)
            return drawJpegFromSd(path, x, y, dither, invert);
        if (strstr(_fileExtension, "png") != NULL)
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
#if defined(ARDUINO_INKPLATECOLOR)
    drawBitmap3Bit(x, y, buf, w, h);
    return 1;
#elif defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
    uint16_t scaled_w = ceil(w / 4.0);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < scaled_w; j++)
        {
            writePixel(4 * j + x, i + y, (buf[scaled_w * i + j] & 0xC0) >> 6);
            writePixel(4 * j + x + 1, i + y, (buf[scaled_w * i + j] & 0x30) >> 4);
            writePixel(4 * j + x + 2, i + y, (buf[scaled_w * i + j] & 0x0C) >> 2);
            writePixel(4 * j + x + 3, i + y, (buf[scaled_w * i + j] & 0x03));
        }
    }
    return 1;
#else
    if (getDisplayMode() == INKPLATE_1BIT && bg == 0xFF)
        drawBitmap(x, y, buf, w, h, c);
    else if (getDisplayMode() == INKPLATE_1BIT && bg != 0xFF)
        drawBitmap(x, y, buf, w, h, c, bg);
    else
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
 *              Image position (center, topLeft, bottomLeft, topRight,
 * bottomRight, _npos)
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
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATE4) && !defined(ARDUINO_INKPLATE2) &&                   \
    !defined(ARDUINO_INKPLATE7)
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
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0x0f) >> 1);
        }
        writePixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
        if (_rem == 0)
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0x0f) >> 1);
    }
    endWrite();
}

/**
 * @brief       Get file extension from the filename and convert it into lowercase letters.
 *
 * @param       char *_filename
 *              Pointer to the array that holds filename (with file extension).
 * @param       char *_extension
 *              Pointer to the array where file extension needs to be saved.
 *
 * @return      True if parsing file extension is successfull, false if not.
 */
bool Image::getFileExtension(char *_filename, char *_extension)
{
    // Check if the _extension or _filename pointers are not NULL.
    if (_extension == NULL || _filename == NULL)
        return false;

    // Find the end of the string
    int _len = strlen(_filename);

    // Check the lenth. It must be greater then 4 (one char filename + dot + three chars for the extension).
    if (_len < 5)
        return false;

    // Go from the back to the start and try to extract file extension.
    // Try to find the index where file extension starts.
    int _startIndex;
    for (_startIndex = _len - 1; (_len >= 0) && (_filename[_startIndex] != '.'); _startIndex--)
        ;

    // Move by one index to the right.
    _startIndex++;

    // Check if the extension index is valid.
    if ((_len - _startIndex) > 4)
        return false;

    // Copy extension into _extension array and convert it into lowercase.
    for (int i = 0; i < (_len - _startIndex); i++)
    {
        _extension[i] = tolower(_filename[_startIndex + i]);
    }

    // Add null terminating char.
    _extension[_len - _startIndex] = '\0';

    // If everything went successfull, return true.
    return true;
}