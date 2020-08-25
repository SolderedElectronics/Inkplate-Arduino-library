#include "Arduino.h"
#include "math.h"
#include "Triangulate.h"

float Triangulate::area(int x1, int y1, int x2, int y2, int x3, int y3)
{
    return fabs((float)((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2))) / 2.0);
}

bool Triangulate::isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{
    float A = area(x1, y1, x2, y2, x3, y3);
    float A1 = area(x, y, x2, y2, x3, y3);
    float A2 = area(x1, y1, x, y, x3, y3);
    float A3 = area(x1, y1, x2, y2, x, y);
    return fabs(-A + A1 + A2 + A3) < 1e-3;
}

void Triangulate::preProcess(int *x, int *y, int n)
{
    for (int i = 0; i < n; ++i)
    {
        int prev = (i - 1 + n) % n;
        int next = (i + 1 + n) % n;
        float deg = atan2(y[prev] - y[i], x[prev] - x[i]) - atan2(y[next] - y[i], x[next] - x[i]);
        if (deg < 0.0)
            deg += 2 * M_PI;
        innerAngle[i] = deg;
    }
}

void Triangulate::updateVertex(int p, int *x, int *y, int n)
{
    int prev = (p - 1 + n) % n;
    int next = (p + 1 + n) % n;
    float deg = atan2(y[prev] - y[p], x[prev] - x[p]) - atan2(y[next] - y[p], x[next] - x[p]);
    if (deg < 0.0)
        deg += 2 * M_PI;
    innerAngle[p] = deg;
    bool f = 0;
    for (int j = 0; j < n; ++j)
    {
        if (prev != j && p != j && next != j &&
            innerAngle[p] > M_PI &&
            isInside(x[prev], y[prev], x[p], y[p], x[next], y[next], x[j], y[j]))
            f = 1;
    }
    earTip[p] = !f;
}

bool Triangulate::isConvex(int *x, int *y, int n)
{
    for (int i = 0; i < n; ++i)
        if (innerAngle[i] > M_PI)
            return 0;
    return 1;
}

void Triangulate::trivialTriangles(int *x, int *y, int n)
{
    for (int i = 0; i < n - 2; ++i)
    {
        tx[tc] = x[0];
        ty[tc] = y[0];
        ++tc;
        tx[tc] = x[i + 1];
        ty[tc] = y[i + 1];
        ++tc;
        tx[tc] = x[i + 2];
        ty[tc] = y[i + 2];
        ++tc;
    }
}

void Triangulate::findEars(int *x, int *y, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (innerAngle[i] > M_PI)
            continue;
        int prev = (i - 1 + n) % n;
        int next = (i + 1 + n) % n;
        bool f = 0;
        for (int j = 0; j < n; ++j)
        {
            if (prev != j && i != j && next != j &&
                innerAngle[i] > M_PI &&
                isInside(x[prev], y[prev], x[i], y[i], x[next], y[next], x[j], y[j]))
                f = 1;
        }
        earTip[i] = !f;
    }
}

int Triangulate::smallestEar(int *x, int *y, int n)
{
    int mn = 0;
    for (int i = 1; i < n; ++i)
        if (earTip[i] && innerAngle[i] < innerAngle[mn])
            mn = i;
    return mn;
}

void Triangulate::nonTrivialTriangles(int *x, int *y, int n)
{
    findEars(x, y, n);
    int initialN = n;
    while (tc / 3 < initialN - 2)
    {
        int pos = smallestEar(x, y, n);
        int prev = (pos - 1 + n) % n;
        int next = (pos + 1 + n) % n;
        tx[tc] = x[prev];
        ty[tc] = y[prev];
        ++tc;
        tx[tc] = x[pos];
        ty[tc] = y[pos];
        ++tc;
        tx[tc] = x[next];
        ty[tc] = y[next];
        ++tc;
        for (int i = pos; i < n - 1; i++)
        {
            x[i] = x[i + 1];
            y[i] = y[i + 1];
            innerAngle[i] = innerAngle[i + 1];
            earTip[i] = earTip[i + 1];
        }
        --n;
        updateVertex(prev, x, y, n);
        updateVertex(prev + 1, x, y, n);
    }
}

void Triangulate::triangulate(int *x, int *y, int n, int *_tx, int *_ty)
{
    tc = 0;
    preProcess(x, y, n);
    if (isConvex(x, y, n))
        trivialTriangles(x, y, n);
    else
        nonTrivialTriangles(x, y, n);
    memcpy(_tx, tx, 100);
    memcpy(_ty, ty, 100);
}