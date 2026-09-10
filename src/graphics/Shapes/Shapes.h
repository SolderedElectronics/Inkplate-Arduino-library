/**
 **************************************************
 * @file        Shapes.h
 * @brief       Basic shapes
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "../../system/InkplateBoards.h"
#include "../../system/defines.h"
#include "../Adafruit_GFX/Adafruit_GFX.h"
#include "Arduino.h"

#define maxVer 100
#define maxHt  E_INK_HEIGHT

/**
 * @brief       Shapes class is used to generate basic shapes
 */
class Shapes : virtual public Adafruit_GFX
{
  public:
    Shapes(int16_t w, int16_t h) : Adafruit_GFX(w, h){};

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;


    void drawElipse(int rx, int ry, int xc, int yc, int c);
    void fillElipse(int rx, int ry, int xc, int yc, int c);
    void drawPolygon(int *x, int *y, int n, int color);
    void fillPolygon(int *x, int *y, int n, int color);
    void drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness);
    void drawGradientLine(int x1, int y1, int x2, int y2, int color1, int color2, float thickness = -1);

#if defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
    void drawTextWithShadow(int x, int y, const char *_text, uint8_t _colorText, uint8_t _colorShadow);
#endif

  private:
    struct EdgeBucket
    {
        int ymax;
        float xofymin;
        float slopeinverse;
    };

    struct edgeTableTuple
    {
        int countEdgeBucket;
        EdgeBucket buckets[maxVer];
    };

    void initedgeTable();
    void insertionSort(edgeTableTuple *ett);
    void storeEdgeInTuple(edgeTableTuple *receiver, int ym, int xm, float slopInv);
    void storeEdgeInTable(int x1, int y1, int x2, int y2);
    void removeEdgeByYmax(edgeTableTuple *tup, int yy);
    void updatexbyslopeinv(edgeTableTuple *tup);
    void scanlineFill(uint8_t c);

    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;

    edgeTableTuple *edgeTable, activeEdgeTuple;
};

#endif