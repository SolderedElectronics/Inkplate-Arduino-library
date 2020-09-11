#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Arduino.h"

class Triangulate
{
  private:
    int tx[100];
    int ty[100];
    int tc = 0;

    float innerAngle[100];
    bool earTip[100];

    float area(int x1, int y1, int x2, int y2, int x3, int y3);
    bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y);
    void preProcess(int *x, int *y, int n);
    void updateVertex(int p, int *x, int *y, int n);
    bool isConvex(int *x, int *y, int n);
    void trivialTriangles(int *x, int *y, int n);
    void findEars(int *x, int *y, int n);
    int smallestEar(int *x, int *y, int n);
    void nonTrivialTriangles(int *x, int *y, int n);

  public:
    void triangulate(int *x, int *y, int n, int *_tx, int *_ty);
};

#endif