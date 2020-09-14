#include "Image.h"

#include "../libs/pngle/pngle.h"

bool Image::drawPngFromSd(const char *fileName, int x, int y, bool dither, bool invert)
{
    return 0;
}

bool Image::drawPngFromSd(SdFile *p, int x, int y, bool dither, bool invert)
{
    return 0;
}

bool Image::drawPngFromWeb(const char *url, int x, int y, bool dither, bool invert)
{
    return 0;
}

bool Image::drawPngFromWeb(WiFiClient *s, int x, int y, int32_t len, bool dither, bool invert)
{
    return 0;
}