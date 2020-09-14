#include "Shapes.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) \
    {                       \
        int16_t t = a;      \
        a = b;              \
        b = t;              \
    }
#endif

void Shapes::drawElipse(int rx, int ry, int xc, int yc, int c)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    while (dx < dy)
    {
        drawPixel(x + xc, y + yc, c);
        drawPixel(-x + xc, y + yc, c);
        drawPixel(x + xc, -y + yc, c);
        drawPixel(-x + xc, -y + yc, c);

        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else
        {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + ((rx * rx) * ((y - 1) * (y - 1))) - (rx * rx * ry * ry);
    while (y >= 0)
    {
        drawPixel(x + xc, y + yc, c);
        drawPixel(-x + xc, y + yc, c);
        drawPixel(x + xc, -y + yc, c);
        drawPixel(-x + xc, -y + yc, c);

        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else
        {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void Shapes::fillElipse(int rx, int ry, int xc, int yc, int c)
{
    int hh = ry * ry;
    int ww = rx * rx;
    int hhww = hh * ww;
    int x0 = rx;
    int dx = 0;

    for (int x = -rx; x <= rx; x++)
        drawPixel(xc + x, yc, c);

    for (int y = 1; y <= ry; y++)
    {
        int x1 = x0 - (dx - 1);
        for (; x1 > 0; x1--)
            if (x1 * x1 * hh + y * y * ww <= hhww)
                break;
        dx = x0 - x1;
        x0 = x1;

        for (int x = -x0; x <= x0; x++)
        {
            drawPixel(xc + x, yc - y, c);
            drawPixel(xc + x, yc + y, c);
        }
    }
}

void Shapes::drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness)
{
    float deg = atan2f((float)(y2 - y1), (float)(x2 - x1));

    float l1 = tan(deg);

    float degShift = (l1 < 0 ? M_PI_2 : -M_PI_2);

    int x3 = (int)round((float)x1 + thickness / 2.0 * cos(deg + degShift));
    int y3 = (int)round((float)y1 + thickness / 2.0 * sin(deg + degShift));

    int x4 = (int)round((float)x2 + thickness / 2.0 * cos(deg + degShift));
    int y4 = (int)round((float)y2 + thickness / 2.0 * sin(deg + degShift));

    x1 = (int)round((float)x1 + thickness / 2.0 * cos(deg - degShift));
    y1 = (int)round((float)y1 + thickness / 2.0 * sin(deg - degShift));

    x2 = (int)round((float)x2 + thickness / 2.0 * cos(deg - degShift));
    y2 = (int)round((float)y2 + thickness / 2.0 * sin(deg - degShift));

    fillTriangle(x1, y1, x2, y2, x3, y3, color);
    fillTriangle(x2, y2, x4, y4, x3, y3, color);
}

void Shapes::drawGradientLine(int x1, int y1, int x2, int y2, int color1, int color2, float thickness)
{
    int n = color2 - color1;

    float px = (float)(x2 - x1) / (float)n;
    float py = (float)(y2 - y1) / (float)n;

    for (int i = 0; i < n; ++i)
    {
        if (abs(thickness + 1) < 0.1)
            drawLine((int)((float)x1 + (float)i * px), (int)((float)y1 + (float)i * py),
                     (int)((float)x1 + (float)(i + 1) * px), (int)((float)y1 + (float)(i + 1) * py), color1 + i);
        else
            drawThickLine((int)((float)x1 + (float)i * px), (int)((float)y1 + (float)i * py),
                          (int)((float)x1 + (float)(i + 1) * px), (int)((float)y1 + (float)(i + 1) * py), color1 + i,
                          thickness);
    }
}
