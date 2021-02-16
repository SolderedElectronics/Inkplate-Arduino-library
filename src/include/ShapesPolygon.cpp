/*
ShapesPolygon.cpp
Inkplate Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
February 12, 2021
https://github.com/e-radionicacom/Inkplate-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Shapes.h"

void Shapes::initedgeTable()
{
    int i;
    for (i = 0; i < maxHt; i++)
        edgeTable[i].countEdgeBucket = 0;
    activeEdgeTuple.countEdgeBucket = 0;
}

void Shapes::insertionSort(edgeTableTuple *ett)
{
    int i, j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++)
    {
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

        while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
        {
            ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
            ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
            ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
            j = j - 1;
        }
        ett->buckets[j + 1].ymax = temp.ymax;
        ett->buckets[j + 1].xofymin = temp.xofymin;
        ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}

void Shapes::storeEdgeInTuple(edgeTableTuple *receiver, int ym, int xm, float slopInv)
{
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

    insertionSort(receiver);

    (receiver->countEdgeBucket)++;
}

void Shapes::storeEdgeInTable(int x1, int y1, int x2, int y2)
{
    float m, minv;
    int ymaxTS, xwithyminTS, scanline; // ts stands for to store

    if (x2 == x1)
    {
        minv = 0.000000;
    }
    else
    {
        m = ((float)(y2 - y1)) / ((float)(x2 - x1));

        if (y2 == y1)
            return;

        minv = (float)1.0 / m;
    }

    if (y1 > y2)
    {
        scanline = y2;
        ymaxTS = y1;
        xwithyminTS = x2;
    }
    else
    {
        scanline = y1;
        ymaxTS = y2;
        xwithyminTS = x1;
    }
    storeEdgeInTuple(&edgeTable[scanline], ymaxTS, xwithyminTS, minv);
}

void Shapes::removeEdgeByYmax(edgeTableTuple *tup, int yy)
{
    int i, j;
    for (i = 0; i < tup->countEdgeBucket; i++)
    {
        if (tup->buckets[i].ymax == yy)
        {
            for (j = i; j < tup->countEdgeBucket - 1; j++)
            {
                tup->buckets[j].ymax = tup->buckets[j + 1].ymax;
                tup->buckets[j].xofymin = tup->buckets[j + 1].xofymin;
                tup->buckets[j].slopeinverse = tup->buckets[j + 1].slopeinverse;
            }
            tup->countEdgeBucket--;
            i--;
        }
    }
}

void Shapes::updatexbyslopeinv(edgeTableTuple *tup)
{
    int i;

    for (i = 0; i < tup->countEdgeBucket; i++)
    {
        (tup->buckets[i]).xofymin = (tup->buckets[i]).xofymin + (tup->buckets[i]).slopeinverse;
    }
}

void Shapes::scanlineFill(uint8_t c)
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

    for (i = 0; i < maxHt; i++)
    {
        for (j = 0; j < edgeTable[i].countEdgeBucket; j++)
            storeEdgeInTuple(&activeEdgeTuple, edgeTable[i].buckets[j].ymax, edgeTable[i].buckets[j].xofymin,
                             edgeTable[i].buckets[j].slopeinverse);

        removeEdgeByYmax(&activeEdgeTuple, i);
        insertionSort(&activeEdgeTuple);

        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        while (j < activeEdgeTuple.countEdgeBucket)
        {
            if (coordCount % 2 == 0)
            {
                x1 = (int)(activeEdgeTuple.buckets[j].xofymin);
                ymax1 = activeEdgeTuple.buckets[j].ymax;
                if (x1 == x2)
                {
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x2 = x1;
                        ymax2 = ymax1;
                    }

                    else
                    {
                        coordCount++;
                    }
                }
                else
                {
                    coordCount++;
                }
            }
            else
            {
                x2 = (int)activeEdgeTuple.buckets[j].xofymin;
                ymax2 = activeEdgeTuple.buckets[j].ymax;

                FillFlag = 0;
                if (x1 == x2)
                {
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                    coordCount++;
                    FillFlag = 1;
                }

                if (FillFlag)
                {
                    drawLine(x1, i, x2, i, c);
                }
            }

            j++;
        }
        updatexbyslopeinv(&activeEdgeTuple);
    }
}

void Shapes::drawPolygon(int *x, int *y, int n, int color)
{
    for (int i = 0; i < n; ++i)
        drawLine(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n], color);
}

void Shapes::fillPolygon(int *x, int *y, int n, int color)
{
    edgeTable = (edgeTableTuple *)ps_malloc(maxHt * sizeof(edgeTableTuple));
    initedgeTable();

    int count = 0, x1, y1, x2, y2;

    for (int i = 0; i < n + 1; ++i)
    {
        count++;
        if (count > 2)
        {
            x1 = x2;
            y1 = y2;
            count = 2;
        }
        if (count == 1)
        {
            x1 = x[i % n];
            y1 = y[i % n];
        }
        else
        {
            x2 = x[i % n];
            y2 = y[i % n];
            drawLine(x1, y1, x2, y2, color);
            storeEdgeInTable(x1, y1, x2, y2);
        }
    }
    scanlineFill(color);
    free(edgeTable);
}