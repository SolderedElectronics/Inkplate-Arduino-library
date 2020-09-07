#include "Shapes.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

void Shapes::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    startWrite();
    writeFastVLine(x, y, h, color);
    endWrite();
}

void Shapes::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    startWrite();
    writeFastHLine(x, y, w, color);
    endWrite();
}

void Shapes::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    startWrite();
    writeFillRect(x, y, w, h, color);
    endWrite();
}

void Shapes::fillScreen(uint16_t color)
{
    startWrite();
    fillRect(0, 0, width(), height(), color);
    endWrite();
}

void Shapes::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if (x0 == x1)
    {
        if (y0 > y1)
        {
            _swap_int16_t(y0, y1);
        }
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    }
    else if (y0 == y1)
    {
        if (x0 > x1)
        {
            _swap_int16_t(x0, x1);
        }
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    }
    else
    {
        startWrite();
        writeLine(x0, y0, x1, y1, color);
        endWrite();
    }
}

void Shapes::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    startWrite();
    writePixel(x0, y0 + r, color);
    writePixel(x0, y0 - r, color);
    writePixel(x0 + r, y0, color);
    writePixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}

void Shapes::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    startWrite();
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4)
        {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2)
        {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8)
        {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1)
        {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
    endWrite();
}

void Shapes::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    startWrite();
    writeFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
    endWrite();
}

void Shapes::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color)
{
    startWrite();
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    int16_t px = x;
    int16_t py = y;

    ++delta;

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (x < (y + 1))
        {
            if (corners & 1)
                writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
            if (corners & 2)
                writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
        }
        if (y != py)
        {
            if (corners & 1)
                writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
            if (corners & 2)
                writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
            py = y;
        }
        px = x;
    }
    endWrite();
}

void Shapes::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    startWrite();
    writeFastHLine(x, y, w, color);
    writeFastHLine(x, y + h - 1, w, color);
    writeFastVLine(x, y, h, color);
    writeFastVLine(x + w - 1, y, h, color);
    endWrite();
}

void Shapes::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2;
    if (r > max_radius)
        r = max_radius;

    startWrite();
    writeFastHLine(x + r, y, w - 2 * r, color);
    writeFastHLine(x + r, y + h - 1, w - 2 * r, color);
    writeFastVLine(x, y + r, h - 2 * r, color);
    writeFastVLine(x + w - 1, y + r, h - 2 * r, color);
    endWrite();

    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void Shapes::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2;
    if (r > max_radius)
        r = max_radius;

    startWrite();
    writeFillRect(x + r, y, w - 2 * r, h, color);
    endWrite();

    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void Shapes::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    startWrite();
    writeLine(x0, y0, x1, y1, color);
    writeLine(x1, y1, x2, y2, color);
    writeLine(x2, y2, x0, y0, color);
    endWrite();
}


void Shapes::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t a, b, y, last;

    if (y0 > y1)
    {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2)
    {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1)
    {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    startWrite();
    if (y0 == y2)
    {
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;

        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;

        writeFastHLine(a, y0, b - a + 1, color);
        endWrite();
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    if (y1 == y2)
        last = y1;
    else
        last = y1 - 1;

    for (y = y0; y <= last; y++)
    {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }

    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++)
    {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }
    endWrite();
}


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

void Shapes::fillPolygon(int *x, int *y, int n, int color)
{
    int tx[100], ty[100];
    triangulate.triangulate(x, y, n, tx, ty);

    for (int i = 0; i < n - 2; ++i)
    {
        fillTriangle(tx[i * 3 + 0], ty[i * 3 + 0], tx[i * 3 + 1], ty[i * 3 + 1], tx[i * 3 + 2], ty[i * 3 + 2], color);
    }
}

void Shapes::drawPolygon(int *x, int *y, int n, int color)
{
    for (int i = 0; i < n; ++i)
        drawLine(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n], color);
}

void Shapes::drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness)
{
    float deg = atan2f((float)(y2 - y1), (float)(x2 - x1));

    float l1 = tan(deg);
    float k1 = (float)y1 - l1 * (float)x1;

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
