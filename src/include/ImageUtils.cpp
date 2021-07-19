/**
 **************************************************
 * @file        ImageUtils.cpp
 * @brief       Image utilities for all formats
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
 * @authors     e-radionica.com
 ***************************************************/

#include "Image.h"

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
void Image::getPointsForPosition(const Position &position, const uint16_t imageWidth, const uint16_t imageHeight,
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
