/**
 **************************************************
 * @file        Image.cpp
 * @brief       Basic functionalities to work with images
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
#pragma once
#if defined(ARDUINO_INKPLATE13SPECTRA) || defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2)
#include "Inkplate.h"
#include "ImageColor.h"
#include "../Tjpeg/TJpg_Decoder.h"
#include "pgmspace.h"


ImageColor *_imagePtrJpeg = nullptr;
ImageColor *_imagePtrPng = nullptr;

int16_t (*ImageColor::ditherBuffer)[ImageColor::ditherRowCount][ImageColor::ditherBufferWidth] = nullptr;
uint8_t *ImageColor::pixelBuffer = nullptr;
uint32_t *ImageColor::ditherPalette = nullptr;
uint8_t *ImageColor::palette = nullptr;


/**
 * @brief       begin initialises the ImageColor subsystem.
 *              Must be called once before any draw functions are used.
 *
 *              Stores the Inkplate pointer, registers the JPEG/PNG callback
 *              pointers, allocates all PSRAM buffers needed for dithering and
 *              pixel staging, and sets the default dither kernel.
 *
 * @param       Inkplate *inkplateptr
 *              Pointer to the parent Inkplate instance.
 */
void ImageColor::begin(Inkplate *inkplateptr)
{
    _inkplate = inkplateptr;

    // Register this instance as the target for JPEG and PNG decode callbacks.
    _imagePtrJpeg = this;
    _imagePtrPng = this;

    setDitherKernel(ReducedDiffusion);

    // Allocate PSRAM buffers. All four are required; a NULL result is reported
    // via Serial and the caller should not attempt to draw images.

    // ditherBuffer size is board-independent (same formula for all color boards).
    ditherBuffer =
        (int16_t(*)[ditherRowCount][ditherBufferWidth])heap_caps_calloc(1, ditherBufferSizeBytes, MALLOC_CAP_SPIRAM);

    // pixelBuffer holds one decoded image row. Inkplate2 and Inkplate13SPECTRA
    // scan along the longer (height) axis; Inkplate6COLOR scans along the width.
#if defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE13SPECTRA)
    pixelBuffer = (uint8_t *)heap_caps_calloc(1, (E_INK_HEIGHT * 4 + 5), MALLOC_CAP_SPIRAM);
#else
    pixelBuffer = (uint8_t *)heap_caps_calloc(1, (E_INK_WIDTH * 4 + 5), MALLOC_CAP_SPIRAM);
#endif

    ditherPalette = (uint32_t *)heap_caps_calloc(256, sizeof(uint32_t), MALLOC_CAP_SPIRAM);
    palette = (uint8_t *)heap_caps_calloc(128, sizeof(uint8_t), MALLOC_CAP_SPIRAM);

    if (!ditherBuffer || !pixelBuffer || !ditherPalette || !palette)
    {
        Serial.println(" Failed to allocate one or more buffers (SRAM/PSRAM)");
    }
}

/**
 * @brief       Sets the active error-diffusion dither kernel.
 *
 * @param       DitherKernel kernel
 *              Enum value selecting the kernel (e.g. FloydSteinberg, Atkinson).
 *              Out-of-range values are silently ignored.
 */
void ImageColor::setDitherKernel(const DitherKernel kernel)
{
    const uint8_t kernelIndex = static_cast<uint8_t>(kernel);
    if (kernelIndex >= DITHER_KERNEL_COUNT)
        return;
    currentKernel = &DITHER_KERNELS[kernelIndex];
}

/**
 * @brief       draw function draws image from string path
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
bool ImageColor::draw(const String path, int x, int y, bool dither, bool invert)
{
    return draw(path.c_str(), x, y, dither, invert);
};

/**
 * @brief       draw function draws image from char path
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
bool ImageColor::draw(const char *path, int x, int y, bool dither, bool invert)
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
 * @brief       draw function draws image from buffer
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
bool ImageColor::draw(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c, uint8_t bg)
{
#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE13SPECTRA)
    drawBitmap3Bit(x, y, buf, w, h);
    return 1;
#else
    uint16_t scaled_w = ceil(w / 4.0);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < scaled_w; j++)
        {
            _inkplate->drawPixel(4 * j + x, i + y, (buf[scaled_w * i + j] & 0xC0) >> 6);
            _inkplate->drawPixel(4 * j + x + 1, i + y, (buf[scaled_w * i + j] & 0x30) >> 4);
            _inkplate->drawPixel(4 * j + x + 2, i + y, (buf[scaled_w * i + j] & 0x0C) >> 2);
            _inkplate->drawPixel(4 * j + x + 3, i + y, (buf[scaled_w * i + j] & 0x03));
        }
    }
    return 1;
#endif
}

/**
 * @brief       draw function draws image from string path
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
bool ImageColor::draw(const String path, const Format &format, const int x, const int y, const bool dither,
                      const bool invert)
{
    return draw(path.c_str(), format, x, y, dither, invert);
};

/**
 * @brief       draw function draws image from string path
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
bool ImageColor::draw(const char *path, const Format &format, const int x, const int y, const bool dither,
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
 * @brief       draw function draws image from string path
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
bool ImageColor::draw(const char *path, const Format &format, const Position &position, const bool dither,
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
void ImageColor::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h)
{
    uint8_t _rem = _w & 1;
    int i, j;
    int xSize = (_w >> 1) + _rem;

    for (i = 0; i < _h; i++)
    {
        for (j = 0; j < xSize - 1; j++)
        {
            _inkplate->drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
            _inkplate->drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0x0f) >> 1);
        }
        _inkplate->drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
        if (_rem == 0)
            _inkplate->drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0x0f) >> 1);
    }
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
bool ImageColor::getFileExtension(char *_filename, char *_extension)
{
    // Check if the _extension or _filename pointers are not NULL.
    if (_extension == NULL || _filename == NULL)
        return false;

    // Ignore any URL query string or fragment (e.g. "?color=white&time=19217398") when
    // locating the extension - the actual file type sits before it.
    const char *_queryMark = strpbrk(_filename, "?#");
    int _len = _queryMark != NULL ? (_queryMark - _filename) : strlen(_filename);

    // Check the lenth. It must be greater then 4 (one char filename + dot + three chars for the extension).
    if (_len < 5)
        return false;

    // Go from the back to the start and try to extract file extension.
    // Try to find the index where file extension starts.
    int _startIndex;
    for (_startIndex = _len - 1; (_startIndex >= 0) && (_filename[_startIndex] != '.'); _startIndex--)
        ;

    // Move by one index to the right.
    _startIndex++;

    // Check if the extension index is valid.
    if (_startIndex <= 0 || (_len - _startIndex) > 4)
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

/**
 * @brief       getPointsForPosition function calculates image position from
 * position parameters
 *
 * @param       Position &position
 *              Image position (center, topLeft, bottomLeft, topRight,
 * bottomRight, _npos)
 * @param       uint16_t imageWidth
 *              Image width
 * @param       uint16_t imageHeight
 *              Image height
 * @param       uint16_t screenWidth
 *              screen width size
 * @param       uint16_t screenHeight
 *              screen height size
 * @param       uint16_t *posX
 *              new x position
 * @param       uint16_t *posY
 *              new y position
 */
void ImageColor::getPointsForPosition(const Position &position, const uint16_t imageWidth, const uint16_t imageHeight,
                                      const uint16_t screenWidth, const uint16_t screenHeight, uint16_t *posX,
                                      uint16_t *posY)
{
    *posX = 0;
    *posY = 0;

    switch (position)
    {
    case TopLeft:
        break;
    case Center:
        if (imageWidth < screenWidth)
            *posX = (screenWidth - imageWidth) >> 1;

        if (imageHeight < screenHeight)
            *posY = (screenHeight - imageHeight) >> 1;
        break;
    case BottomLeft:
        if (imageHeight < screenHeight)
            *posY = screenHeight - imageHeight;
        break;
    case TopRight:
        if (imageWidth < screenWidth)
            *posX = screenWidth - imageWidth;
        break;
    case BottomRight:
        if (imageWidth < screenWidth)
            *posX = screenWidth - imageWidth;
        if (imageHeight < screenHeight)
            *posY = screenHeight - imageHeight;
        break;
    default:
        break;
    }
}

#endif