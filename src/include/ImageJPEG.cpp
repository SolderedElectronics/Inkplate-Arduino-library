#include "Image.h"

#define RED(a)   ((((a)&0xf800) >> 11) << 3)
#define GREEN(a) ((((a)&0x07e0) >> 5) << 2)
#define BLUE(a)  (((a)&0x001f) << 3)
