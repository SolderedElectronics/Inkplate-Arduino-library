#ifndef __FONT_H__
#define __FONT_H__

#include "Arduino.h"
#include "Print.h"

#include "../libs/GFXFont/gfxfont.h"
#include "../libs/GFXFont/glcdfont.c"


class Font : public Print
{
  public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

    virtual int16_t width() = 0;
    virtual int16_t height() = 0;

    virtual size_t write(uint8_t);

    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);

    void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w,
                       uint16_t *h);
    void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

    void setTextSize(uint8_t s);
    void setTextSize(uint8_t sx, uint8_t sy);

    void setFont(const GFXfont *f = NULL);

    int16_t getCursorX();
    int16_t getCursorY();
    void setCursor(int16_t x, int16_t y);

    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);

    void setTextWrap(bool w);
    void cp437(bool x);

  private:
    int16_t cursor_x = 0;
    int16_t cursor_y = 0;
    uint16_t textcolor, textbgcolor;
    uint8_t textsize_x, textsize_y;
    bool wrap, _cp437;

    virtual void startWrite(void) = 0;
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) = 0;
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void endWrite(void) = 0;

  protected:
    GFXfont *gfxFont;
    void charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
};

#endif