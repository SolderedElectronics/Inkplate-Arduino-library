#include "Image.h"
#include "ImageDitherColorKernels.h"

//#ifdef ARDUINO_INKPLATECOLOR
int kernelWidth = sizeof kernel[0] / sizeof kernel[0][0];
int kernelHeight = sizeof kernel / sizeof kernel[0];

unsigned int pallete[] = {
    0x000000, 0xFFFFFF, 0x00FF00, 0x0000FF, 0xFF0000, 0xFFFF00, 0xFF8000,
};

unsigned int width = E_INK_WIDTH, height = E_INK_HEIGHT;

#endif