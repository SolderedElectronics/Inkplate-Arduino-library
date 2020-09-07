#include "Font.h"

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c)
{
    return gfxFont->glyph + c;
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont)
{
    return gfxFont->bitmap;
}

void Font::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void Font::setTextColor(uint16_t c)
{
    textcolor = textbgcolor = c;
}

void Font::setTextColor(uint16_t c, uint16_t bg)
{
    textcolor = c;
    textbgcolor = bg;
}

void Font::setTextWrap(bool w)
{
    wrap = w;
}

void Font::cp437(bool x = true)
{
    _cp437 = x;
}

int16_t Font::getCursorX()
{
    return cursor_x;
}

int16_t Font::getCursorY()
{
    return cursor_y;
}

void Font::setTextSize(uint8_t s)
{
    setTextSize(s, s);
}

void Font::setTextSize(uint8_t s_x, uint8_t s_y)
{
    textsize_x = (s_x > 0) ? s_x : 1;
    textsize_y = (s_y > 0) ? s_y : 1;
}

void Font::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
    drawChar(x, y, c, color, bg, size, size);
}

void Font::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y)
{
    if (!gfxFont)
    {
        if ((x >= width()) ||             // Clip right
            (y >= height()) ||            // Clip bottom
            ((x + 6 * size_x - 1) < 0) || // Clip left
            ((y + 8 * size_y - 1) < 0))   // Clip top
            return;

        if (!_cp437 && (c >= 176))
            c++;

        startWrite();
        for (int8_t i = 0; i < 5; i++)
        {
            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
            for (int8_t j = 0; j < 8; j++, line >>= 1)
            {
                if (line & 1)
                {
                    if (size_x == 1 && size_y == 1)
                        writePixel(x + i, y + j, color);
                    else
                        writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y, color);
                }
                else if (bg != color)
                {
                    if (size_x == 1 && size_y == 1)
                        writePixel(x + i, y + j, bg);
                    else
                        writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
                }
            }
        }
        if (bg != color)
        {
            if (size_x == 1 && size_y == 1)
                writeFastVLine(x + 5, y, 8, bg);
            else
                writeFillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
        }
        endWrite();
    }
    else
    {
        c -= (uint8_t)pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
        uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

        uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
        uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
        int8_t xo = pgm_read_byte(&glyph->xOffset), yo = pgm_read_byte(&glyph->yOffset);
        uint8_t xx, yy, bits = 0, bit = 0;
        int16_t xo16 = 0, yo16 = 0;

        if (size_x > 1 || size_y > 1)
        {
            xo16 = xo;
            yo16 = yo;
        }

        startWrite();
        for (yy = 0; yy < h; yy++)
        {
            for (xx = 0; xx < w; xx++)
            {
                if (!(bit++ & 7))
                {
                    bits = pgm_read_byte(&bitmap[bo++]);
                }
                if (bits & 0x80)
                {
                    if (size_x == 1 && size_y == 1)
                    {
                        writePixel(x + xo + xx, y + yo + yy, color);
                    }
                    else
                    {
                        writeFillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y, size_x, size_y, color);
                    }
                }
                bits <<= 1;
            }
        }
        endWrite();
    }
}

size_t Font::write(uint8_t c)
{
    if (!gfxFont)
    { // 'Classic' built-in font

        if (c == '\n')
        {                               // Newline?
            cursor_x = 0;               // Reset x to zero,
            cursor_y += textsize_y * 8; // advance y one line
        }
        else if (c != '\r')
        { // Ignore carriage returns
            if (wrap && ((cursor_x + textsize_x * 6) > width()))
            {                               // Off right?
                cursor_x = 0;               // Reset x to zero,
                cursor_y += textsize_y * 8; // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x, textsize_y);
            cursor_x += textsize_x * 6; // Advance x one char
        }
    }
    else
    { // Custom font

        if (c == '\n')
        {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        else if (c != '\r')
        {
            uint8_t first = pgm_read_byte(&gfxFont->first);
            if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last)))
            {
                GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
                uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
                if ((w > 0) && (h > 0))
                {                                                        // Is there an associated bitmap?
                    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                    if (wrap && ((cursor_x + textsize_x * (xo + w)) > width()))
                    {
                        cursor_x = 0;
                        cursor_y += (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x, textsize_y);
                }
                cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
            }
        }
    }
    return 1;
}

void Font::setFont(const GFXfont *f)
{
    if (f)
    {
        if (!gfxFont)
        {
            cursor_y += 6;
        }
    }
    else if (gfxFont)
    {
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

void Font::charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy)
{
    if (gfxFont)
    {
        if (c == '\n')
        {
            *x = 0;
            *y += textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        else if (c != '\r')
        {
            uint8_t first = pgm_read_byte(&gfxFont->first), last = pgm_read_byte(&gfxFont->last);
            if ((c >= first) && (c <= last))
            {
                GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
                uint8_t gw = pgm_read_byte(&glyph->width), gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t xo = pgm_read_byte(&glyph->xOffset), yo = pgm_read_byte(&glyph->yOffset);
                if (wrap && ((*x + (((int16_t)xo + gw) * textsize_x)) > width()))
                {
                    *x = 0;
                    *y += textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                }
                int16_t tsx = (int16_t)textsize_x, tsy = (int16_t)textsize_y, x1 = *x + xo * tsx, y1 = *y + yo * tsy,
                        x2 = x1 + gw * tsx - 1, y2 = y1 + gh * tsy - 1;
                if (x1 < *minx)
                    *minx = x1;
                if (y1 < *miny)
                    *miny = y1;
                if (x2 > *maxx)
                    *maxx = x2;
                if (y2 > *maxy)
                    *maxy = y2;
                *x += xa * tsx;
            }
        }
    }
    else
    {
        if (c == '\n')
        {
            *x = 0;
            *y += textsize_y * 8;
        }
        else if (c != '\r')
        {
            if (wrap && ((*x + textsize_x * 6) > width()))
            {
                *x = 0;
                *y += textsize_y * 8;
            }
            int x2 = *x + textsize_x * 6 - 1, y2 = *y + textsize_y * 8 - 1;
            if (x2 > *maxx)
                *maxx = x2; // Track max x, y
            if (y2 > *maxy)
                *maxy = y2;
            if (*x < *minx)
                *minx = *x; // Track min x, y
            if (*y < *miny)
                *miny = *y;
            *x += textsize_x * 6; // Advance x one char
        }
    }
}

void Font::getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    uint8_t c;

    *x1 = x;
    *y1 = y;
    *w = *h = 0;

    int16_t minx = width(), miny = height(), maxx = -1, maxy = -1;

    while ((c = *str++))
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if (maxx >= minx)
    {
        *x1 = minx;
        *w = maxx - minx + 1;
    }
    if (maxy >= miny)
    {
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}

void Font::getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    if (str.length() != 0)
    {
        getTextBounds(const_cast<char *>(str.c_str()), x, y, x1, y1, w, h);
    }
}

void Font::getTextBounds(const __FlashStringHelper *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w,
                         uint16_t *h)
{
    uint8_t *s = (uint8_t *)str, c;

    *x1 = x;
    *y1 = y;
    *w = *h = 0;

    int16_t minx = width(), miny = height(), maxx = -1, maxy = -1;

    while ((c = pgm_read_byte(s++)))
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if (maxx >= minx)
    {
        *x1 = minx;
        *w = maxx - minx + 1;
    }
    if (maxy >= miny)
    {
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}