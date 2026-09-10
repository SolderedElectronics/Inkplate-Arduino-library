/**
 **************************************************
 *
 * @file        Inkplate.cpp
 * @brief       Basic funtions for controling inkplate
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Josip Šimun Kuči @ Soldered
 ***************************************************/

#include "Inkplate.h"

//--------------------------USER FUNCTIONS--------------------------------------------
#ifdef MULTIPLE_DISPLAY_MODES
/**
 * @brief       Constructor for boards with multiple display modes.
 *
 * @param       uint8_t mode
 *              Initial display mode: INKPLATE_1BIT or INKPLATE_3BIT.
 */
Inkplate::Inkplate(uint8_t mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
{
    _mode = mode;
}

/**
 * @brief       preloadScreen function copies data from partial to data buffer
 */
void Inkplate::preloadScreen()
{
    memcpy(DMemoryNew, _partial, E_INK_WIDTH * E_INK_HEIGHT / 8);
}
#else
/**
 * @brief       Constructor for boards with a single fixed display mode.
 */
Inkplate::Inkplate() : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT), Graphics(E_INK_WIDTH, E_INK_HEIGHT)
{
}
#endif

/**
 * @brief       Initializes the Inkplate library, peripherals, and framebuffers.
 *
 * @note        Must be called once in setup(). Subsequent calls are ignored.
 */
void Inkplate::begin()
{

    // Check if the initializaton of the library already done.
    // In the case of already initialized library, return form the begin() funtion to
    // avoiid any memory leaks, multiple initializaton of the peripherals etc.
    if (_beginDone == 1)
        return;

    inkplateMutexInit();

    Wire.begin();

    // Init low level driver for EPD.
    initDriver(this);

// Forward the display mode to the EPD driver
#ifdef MULTIPLE_DISPLAY_MODES
    selectDisplayMode(_mode);
#endif

    // Clean frame buffers.
    clearDisplay();

    // Block multiple inits.
    _beginDone = 1;
}

/**
 * @brief       Draws a single pixel; Adafruit_GFX entry point.
 *
 * @param       int16_t x
 *              Pixel x coordinate.
 * @param       int16_t y
 *              Pixel y coordinate.
 * @param       uint16_t color
 *              Pixel color value.
 */
void Inkplate::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    writePixel(x, y, color);
}

/**
 * @brief       Writes a single pixel into the framebuffer with rotation applied.
 *
 * @param       int16_t x
 *              Pixel x coordinate.
 * @param       int16_t y
 *              Pixel y coordinate.
 * @param       uint16_t color
 *              Pixel color value.
 */
void Inkplate::writePixel(int16_t x, int16_t y, uint16_t color)
{
    writePixelInternal(x, y, color);
}

/**
 * @brief       Sets the display rotation and updates the reported width/height.
 *
 * @param       uint8_t r
 *              Rotation index: 0 = normal, 1 = 90° left, 2 = 180°, 3 = 90° right.
 */
void Inkplate::setRotation(uint8_t r)
{
    rotation = (r & 3);
    switch (rotation)
    {
    case 0:
    case 2:
        _width = E_INK_WIDTH;
        _height = E_INK_HEIGHT;
        break;
    case 1:
    case 3:
        _width = E_INK_HEIGHT;
        _height = E_INK_WIDTH;
        break;
    }
}
