#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "TJpg_Decoder.h"

#define RED(a)    ((((a) & 0xf800) >> 11) << 3)
#define GREEN(a)  ((((a) & 0x07e0) >> 5) << 2)
#define BLUE(a)   (((a) & 0x001f) << 3)

SPIClass spi2(HSPI);
SdFat sd(&spi2);

//--------------------------STATIC FUNCTIONS--------------------------------------------
//For precise 1uS timing, we cannot use delayMicroseconds(), instead we use ASM with nop command. Initial Z value will be difeerent on different CPU speeds! (for 240 MHz CPU Clock z = 25)
void usleep1()
{
    int z = 25;
    while (z--)
    {
        asm("NOP");
    };
}

void ckvClock()
{
    CKV_CLEAR;
    usleep1();
    CKV_SET;
    usleep1();
}

bool jpegCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* data, void* _display) {
    Inkplate *display = static_cast<Inkplate *>(_display);

    int i, j;
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            uint16_t rgb = data[j*w + i];
            uint8_t px = (RED(rgb) * 2126 / 10000) + (GREEN(rgb) * 7152 / 10000) + (BLUE(rgb) * 722 / 10000);
            display->drawPixel(i + x, j + y, px >> 5);
        }
    }

    return 1;
}

//--------------------------USER FUNCTIONS--------------------------------------------
Inkplate::Inkplate(uint8_t _mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT)
{
    _displayMode = _mode;
}

void Inkplate::begin(void)
{
    if (_beginDone == 1)
        return;
    Wire.begin();
	mcpBegin(MCP23017_ADDR, mcpRegsInt);
    pinModeMCP(VCOM, OUTPUT);
    pinModeMCP(PWRUP, OUTPUT);
    pinModeMCP(WAKEUP, OUTPUT);
    pinModeMCP(GPIO0_ENABLE, OUTPUT);
    digitalWriteMCP(GPIO0_ENABLE, HIGH);

    //CONTROL PINS
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeMCP(OE, OUTPUT);
    pinModeMCP(GMOD, OUTPUT);
    pinModeMCP(SPV, OUTPUT);

    //DATA PINS
    pinMode(4, OUTPUT); //D0
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT); //D7

    //TOUCHPAD PINS
    pinModeMCP(10, INPUT);
    pinModeMCP(11, INPUT);
    pinModeMCP(12, INPUT);

    //Battery voltage Switch MOSFET
    pinModeMCP(9, OUTPUT);

    D_memory_new = (uint8_t *)ps_malloc(600 * 100);
    _partial = (uint8_t *)ps_malloc(600 * 100);
    _pBuffer = (uint8_t *)ps_malloc(120000);
    D_memory4Bit = (uint8_t *)ps_malloc(240000);
    if (D_memory_new == NULL || _partial == NULL || _pBuffer == NULL || D_memory4Bit == NULL)
    {
        do
        {
            delay(100);
        } while (true);
    }
    memset(D_memory_new, 0, 60000);
    memset(_partial, 0, 60000);
    memset(_pBuffer, 0, 120000);
    memset(D_memory4Bit, 255, 240000);

    //precalculateGamma(gammaLUT, INKPLATE_GAMMA);
    precalculateGamma(gammaLUT, 1);
    _beginDone = 1;
}

//Draw function, used by Adafruit GFX.
void Inkplate::drawPixel(int16_t x0, int16_t y0, uint16_t color)
{
    if (x0 > _width - 1 || y0 > _height - 1 || x0 < 0 || y0 < 0)
        return;

    switch (_rotation)
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = _height - x0 - 1;
        break;
    case 2:
        x0 = _width - x0 - 1;
        y0 = _height - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = _width - y0 - 1;
        break;
    }

    if (_displayMode == 0)
    {
        int x = x0 / 8;
        int x_sub = x0 % 8;
        uint8_t temp = *(_partial + 100 * y0 + x); //D_memory_new[99 * y0 + x];
        *(_partial + 100 * y0 + x) = ~pixelMaskLUT[x_sub] & temp | (color ? pixelMaskLUT[x_sub] : 0);
    }
    else
    {
        color &= 7;
        int x = x0 / 2;
        int x_sub = x0 % 2;
        uint8_t temp;
        temp = *(D_memory4Bit + 400 * y0 + x);
        *(D_memory4Bit + 400 * y0 + x) = pixelMaskGLUT[x_sub] & temp | (x_sub ? color : color << 4);
    }
}

//Get pixel color function.
uint16_t Inkplate::getPixel(int16_t x0, int16_t y0)
{
    if (x0 > _width - 1 || y0 > _height - 1 || x0 < 0 || y0 < 0)
        return 0;

    switch (_rotation)
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = _height - x0 - 1;
        break;
    case 2:
        x0 = _width - x0 - 1;
        y0 = _height - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = _width - y0 - 1;
        break;
    }

    if (_displayMode == 0)
    {
        int x = x0 / 8;
        int x_sub = x0 % 8;
        return *(_partial + 100 * y0 + x) & pixelMaskLUT[x_sub] * 7;
    }
    else
    {
        int x = x0 >> 1;
        int x_sub = x0 & 1;
        uint8_t temp;
        return (*(D_memory4Bit + 400 * y0 + x) & pixelMaskGLUT[x_sub]) >> (x_sub ? 4 : 0);
    }
}

void Inkplate::clearDisplay()
{
    //Clear 1 bit per pixel display buffer
    if (_displayMode == 0)
        memset(_partial, 0, 60000);

    //Clear 3 bit per pixel display buffer
    if (_displayMode == 1)
        memset(D_memory4Bit, 255, 240000);
}

//Function that displays content from RAM to screen
void Inkplate::display()
{
    if (_displayMode == 0)
        display1b();
    if (_displayMode == 1)
        display3b();
}

void Inkplate::partialUpdate()
{
    if (_displayMode == 1)
        return;
    if (_blockPartial == 1)
        display1b();
    uint16_t _pos = 59999;
    uint32_t _send;
    uint8_t data;
    uint8_t diffw, diffb;
    uint32_t n = 119999;
    uint8_t dram;

    for (int i = 0; i < 600; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            diffw = ((*(D_memory_new + _pos)) ^ (*(_partial + _pos))) & (~(*(_partial + _pos)));
            diffb = ((*(D_memory_new + _pos)) ^ (*(_partial + _pos))) & ((*(_partial + _pos)));
            _pos--;
            *(_pBuffer + n) = LUTW[diffw >> 4] & (LUTB[diffb >> 4]);
            n--;
            *(_pBuffer + n) = LUTW[diffw & 0x0F] & (LUTB[diffb & 0x0F]);
            n--;
        }
    }

    einkOn();
    for (int k = 0; k < 5; k++)
    {
        vscan_start();
        n = 119999;
        for (int i = 0; i < 600; i++)
        {
            data = *(_pBuffer + n);
            _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            hscan_start(_send);
            n--;
            for (int j = 0; j < 199; j++)
            {
                data = *(_pBuffer + n);
                _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
                n--;
            }
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    /*
      for (int k = 0; k < 1; k++) {
      vscan_start();
    _pos = 59999;
      for (int i = 0; i < 600; i++) {
      data = discharge[(*(D_memory_new + _pos) >> 4)];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
      hscan_start(_send);
      data = discharge[*(D_memory_new + _pos) & 0x0F];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
      _pos--;
        for (int j = 0; j < 99; j++) {
          data = discharge[(*(D_memory_new + _pos) >> 4)];
          _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
          data = discharge[*(D_memory_new + _pos) & 0x0F];
          _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
      _pos--;
        }
      GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
      vscan_end();
      }
      delayMicroseconds(230);
    }
    */
    cleanFast(2, 2);
    cleanFast(3, 1);
    vscan_start();
    einkOff();
    einkOff();
    for (int i = 0; i < 60000; i++)
    {
        *(D_memory_new + i) &= *(_partial + i);
        *(D_memory_new + i) |= (*(_partial + i));
    }
}

void Inkplate::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h)
{
    if (_displayMode != INKPLATE_3BIT)
        return;
    uint8_t _rem = _w % 2;
    int i, j;
    int xSize = _w / 2 + _rem;

    for (i = 0; i < _h; i++)
    {
        for (j = 0; j < xSize - 1; j++)
        {
            drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
            drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
        }
        drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
        if (_rem == 0)
            drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
    }
}

void Inkplate::setRotation(uint8_t r)
{
    _rotation = r % 4;
    switch (_rotation)
    {
    case 0:
        _width = E_INK_WIDTH;
        _height = E_INK_HEIGHT;
        break;
    case 1:
        _width = E_INK_HEIGHT;
        _height = E_INK_WIDTH;
        break;
    case 2:
        _width = E_INK_WIDTH;
        _height = E_INK_HEIGHT;
        break;
    case 3:
        _width = E_INK_HEIGHT;
        _height = E_INK_WIDTH;
        break;
    }
}

//Turn off epapewr supply and put all digital IO pins in high Z state
void Inkplate::einkOff()
{
    if (_panelOn == 0)
        return;
    _panelOn = 0;
    GMOD_CLEAR;
    OE_CLEAR;
    GPIO.out &= ~(DATA | CL | LE);
    SPH_CLEAR;
    SPV_CLEAR;

    PWRUP_CLEAR;
    WAKEUP_CLEAR;
    VCOM_CLEAR;

    pinsZstate();
}

//Turn on supply for epaper display (TPS65186) [+15 VDC, -15VDC, +22VDC, -20VDC, +3.3VDC, VCOM]
void Inkplate::einkOn()
{
    if (_panelOn == 1)
        return;
    _panelOn = 1;
    pinsAsOutputs();
    WAKEUP_SET;
    PWRUP_SET;
    VCOM_SET;
    //Enable all rails
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(B00111111);
    Wire.endTransmission();

    delay(40);

    Wire.beginTransmission(0x48);
    Wire.write(0x0D);
    Wire.write(B10000000);
    Wire.endTransmission();

    delay(2);

    Wire.beginTransmission(0x48);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(0x48, 1);
    _temperature = Wire.read();

    LE_CLEAR;  //setpin_le(FALSE);
    OE_CLEAR;  //setpin_oe(FALSE);
    CL_CLEAR;  //setpin_cl(FALSE);
    SPH_SET;   //setpin_sph(FALSE);
    GMOD_SET;  //setpin_gmode(FALSE);
    SPV_SET;   //setpin_spv(FALSE);
    CKV_CLEAR; //setpin_ckv(FALSE);
    OE_SET;
}

void Inkplate::selectDisplayMode(uint8_t _mode)
{
    if (_mode != _displayMode)
    {
        _displayMode = _mode & 1;
        memset(D_memory_new, 0, 60000);
        memset(_partial, 0, 60000);
        memset(_pBuffer, 0, 120000);
        memset(D_memory4Bit, 255, 240000);
        _blockPartial = 1;
    }
}

uint8_t Inkplate::getDisplayMode()
{
    return _displayMode;
}

int Inkplate::drawBitmapFromSD(SdFile *p, int x, int y, bool dither, bool invert)
{
    if (sdCardOk == 0)
        return 0;
    struct bitmapHeader bmpHeader;
    readBmpHeaderSd(p, &bmpHeader);
    if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 || !(bmpHeader.color == 1 || bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 24))
        return 0;

    if ((bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 24 || bmpHeader.color == 32) && getDisplayMode() != INKPLATE_3BIT)
    {
        selectDisplayMode(INKPLATE_3BIT);
    }

    if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT)
    {
        selectDisplayMode(INKPLATE_1BIT);
    }

    if (bmpHeader.color == 1)
        drawMonochromeBitmapSd(p, bmpHeader, x, y, invert);
    if (bmpHeader.color == 4)
        drawGrayscaleBitmap4Sd(p, bmpHeader, x, y, dither, invert);
    if (bmpHeader.color == 8)
        drawGrayscaleBitmap8Sd(p, bmpHeader, x, y, dither, invert);
    if (bmpHeader.color == 24)
        drawGrayscaleBitmap24Sd(p, bmpHeader, x, y, dither, invert);

    return 1;
}

int Inkplate::drawBitmapFromSD(char *fileName, int x, int y, bool dither, bool invert)
{
    if (sdCardOk == 0)
        return 0;
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
    {
        return drawBitmapFromSD(&dat, x, y, dither, invert);
    }
    else
    {
        return 0;
    }
}

int Inkplate::drawBitmapFromWeb(WiFiClient *s, int x, int y, int len, bool dither, bool invert)
{
    struct bitmapHeader bmpHeader;
    readBmpHeaderWeb(s, &bmpHeader);
    if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 || !(bmpHeader.color == 1 || bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 24))
        return 0;

    if ((bmpHeader.color == 4 || bmpHeader.color == 8 || bmpHeader.color == 24 || bmpHeader.color == 32) && getDisplayMode() != INKPLATE_3BIT)
    {
        selectDisplayMode(INKPLATE_3BIT);
    }

    if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT)
    {
        selectDisplayMode(INKPLATE_1BIT);
    }

    if (bmpHeader.color == 1)
        drawMonochromeBitmapWeb(s, bmpHeader, x, y, len, invert);
    if (bmpHeader.color == 4)
        drawGrayscaleBitmap4Web(s, bmpHeader, x, y, len, dither, invert);
    if (bmpHeader.color == 8)
        drawGrayscaleBitmap8Web(s, bmpHeader, x, y, len, dither, invert);
    if (bmpHeader.color == 24)
        drawGrayscaleBitmap24Web(s, bmpHeader, x, y, len, dither, invert);

    return 1;
}

int Inkplate::drawBitmapFromWeb(char *url, int x, int y, bool dither, bool invert)
{
    if (WiFi.status() != WL_CONNECTED)
        return 0;
    int ret = 0;

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode == 200)
    {
        int32_t len = http.getSize();
        if (len > 0)
        {
            WiFiClient *dat = http.getStreamPtr();
            ret = drawBitmapFromWeb(dat, x, y, len, dither, invert);
        }
    }

    http.end();
    WiFi.setSleep(sleep);
    return ret;
}

int Inkplate::drawJpegFromSD(Inkplate *display, SdFile *p, int x, int y, bool dither, bool invert)
{
    uint16_t w = 0, h = 0;

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(jpegCallback);

    uint32_t pnt = 0;
    uint32_t total = p->fileSize();
    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    while (pnt < total) {
        uint32_t toread = p->available();
        if (toread > 0) {
            int read = p->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }
    p->close();

    //TJpgDec.getJpgSize(&w, &h, buf, total);
    //Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);
    selectDisplayMode(INKPLATE_3BIT);

    TJpgDec.drawJpg(x, y, buf, total, display);

    free(buf);
}

int Inkplate::drawJpegFromSD(Inkplate *display, char *fileName, int x, int y, bool dither, bool invert)
{
    if (sdCardOk == 0)
        return 0;
    SdFile dat;
    if (dat.open(fileName, O_RDONLY))
    {
        return drawJpegFromSD(display, &dat, x, y, dither, invert);
    }
    else
    {
        return 0;
    }
}

void Inkplate::drawElipse(int rx, int ry,
                          int xc, int yc,
                          int c)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    d1 = (ry * ry) - (rx * rx * ry) +
         (0.25 * rx * rx);
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

    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
         ((rx * rx) * ((y - 1) * (y - 1))) -
         (rx * rx * ry * ry);
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

void Inkplate::fillElipse(int rx, int ry,
                          int xc, int yc,
                          int c)
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

void Inkplate::fillPolygon(int *x, int *y, int n, int color)
{
    int tx[100], ty[100];
    triangulate.triangulate(x, y, n, tx, ty);

    for (int i = 0; i < n - 2; ++i)
    {
        fillTriangle(
            tx[i * 3 + 0], ty[i * 3 + 0],
            tx[i * 3 + 1], ty[i * 3 + 1],
            tx[i * 3 + 2], ty[i * 3 + 2],
            color);
    }
}

void Inkplate::drawPolygon(int *x, int *y, int n, int color)
{
    for (int i = 0; i < n; ++i)
        drawLine(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n], color);
}

void Inkplate::drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness)
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

void Inkplate::drawGradientLine(int x1, int y1, int x2, int y2, int color1, int color2, float thickness)
{
    int n = color2 - color1;

    float px = (float)(x2 - x1) / (float)n;
    float py = (float)(y2 - y1) / (float)n;

    for (int i = 0; i < n; ++i)
    {
        if (abs(thickness + 1) < 0.1)
            drawLine((int)((float)x1 + (float)i * px), (int)((float)y1 + (float)i * py),
                     (int)((float)x1 + (float)(i + 1) * px), (int)((float)y1 + (float)(i + 1) * py),
                     color1 + i);
        else
            drawThickLine((int)((float)x1 + (float)i * px), (int)((float)y1 + (float)i * py),
                          (int)((float)x1 + (float)(i + 1) * px), (int)((float)y1 + (float)(i + 1) * py),
                          color1 + i,
                          thickness);
    }
}

int Inkplate::sdCardInit()
{
    spi2.begin(14, 12, 13, 15);
    sdCardOk = sd.begin(15, SD_SCK_MHZ(25));
    return sdCardOk;
}

SdFat Inkplate::getSdFat()
{
    return sd;
}

SPIClass Inkplate::getSPI()
{
    return spi2;
}

uint8_t Inkplate::getPanelState()
{
    return _panelOn;
}

uint8_t Inkplate::readTouchpad(uint8_t _pad)
{
    return digitalReadMCP((_pad & 3) + 10);
}

int8_t Inkplate::readTemperature()
{
    return _temperature;
}

double Inkplate::readBattery()
{
    digitalWriteMCP(9, HIGH);
    delay(1);
    int adc = analogRead(35);
    digitalWriteMCP(9, LOW);
    return (double(adc) / 4095 * 3.3 * 2);
}

//--------------------------LOW LEVEL STUFF--------------------------------------------
void Inkplate::vscan_start()
{
    CKV_SET;
    delayMicroseconds(7);
    SPV_CLEAR;
    delayMicroseconds(10);
    CKV_CLEAR;
    delayMicroseconds(0); //usleep1();
    CKV_SET;
    delayMicroseconds(8);
    SPV_SET;
    delayMicroseconds(10);
    CKV_CLEAR;
    delayMicroseconds(0); //usleep1();
    CKV_SET;
    delayMicroseconds(18);
    CKV_CLEAR;
    delayMicroseconds(0); //usleep1();
    CKV_SET;
    delayMicroseconds(18);
    CKV_CLEAR;
    delayMicroseconds(0); //usleep1();
    CKV_SET;
    //delayMicroseconds(18);
}

void Inkplate::vscan_write()
{
    CKV_CLEAR;
    LE_SET;
    LE_CLEAR;
    delayMicroseconds(0);
    SPH_CLEAR;
    CL_SET;
    CL_CLEAR;
    SPH_SET;
    CKV_SET;
}

void Inkplate::hscan_start(uint32_t _d)
{
    SPH_CLEAR;
    GPIO.out_w1ts = (_d) | CL;
    GPIO.out_w1tc = DATA | CL;
    //CL_SET;
    //CL_CLEAR;
    SPH_SET;
}

void Inkplate::vscan_end()
{
    CKV_CLEAR;
    LE_SET;
    LE_CLEAR;
    delayMicroseconds(1);
    CKV_SET;
}

//Clear screan before new screen update using waveform
void Inkplate::clean()
{
    einkOn();
    int m = 0;
    cleanFast(0, 1); //white
    m++;
    cleanFast((waveform[m] >> 30) & 3, 8); //White to white
    m++;
    cleanFast((waveform[m] >> 24) & 3, 1); //white to black
    m++;
    cleanFast((waveform[m]) & 3, 8); //Black to black
    m++;
    cleanFast((waveform[m] >> 6) & 3, 1); //Black to white
    m++;
    cleanFast((waveform[m] >> 30) & 3, 10); //White to white
}

//Clears content from epaper diplay as fast as ESP32 can.
void Inkplate::cleanFast(uint8_t c, uint8_t rep)
{
    einkOn();
    uint8_t data;
    if (c == 0)
    {
        data = B10101010; //White
    }
    else if (c == 1)
    {
        data = B01010101; //Black
    }
    else if (c == 2)
    {
        data = B00000000; //Discharge
    }
    else if (c == 3)
    {
        data = B11111111; //Skip
    }

	uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);;
	for (int k = 0; k < rep; k++) {
    vscan_start();
    for (int i = 0; i < 600; i++)
	{
		hscan_start(_send);
		GPIO.out_w1ts = (_send) | CL;
		GPIO.out_w1tc = DATA | CL;
		for (int j = 0; j < 99; j++)
		{
			GPIO.out_w1ts = (_send) | CL;
			GPIO.out_w1tc = DATA | CL;
			GPIO.out_w1ts = (_send) | CL;
			GPIO.out_w1tc = DATA | CL;
		}
		GPIO.out_w1ts = (_send) | CL;
		GPIO.out_w1tc = DATA | CL;
		vscan_end();
    }
    delayMicroseconds(230);
  }
}

void Inkplate::cleanFast2(uint8_t c, uint8_t n, uint16_t d)
{
    /*
    uint8_t data;
    if (c == 0) {
      data = B10101010;     //White
    } else if (c == 1) {
      data = B01010101;     //Black
    } else if (c == 2) {
      data = B00000000;     //Discharge
    } else if (c == 3) {
      data = B11111111;   //Skip
    }
    uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
    begin_frame();
    for (int k = 0; k < n; k++) {

      for (int i = 0; i < 600; i++) {
        begin_line();
        if (k == 0) {

          for (int j = 0; j < 100; j++) {
            GPIO.out_w1tc = DATA | CL;
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            GPIO.out_w1ts = (_send) | CL;
          }
        }
        end_line();
      }
      end_frame();
    delayMicroseconds(d);
    }
    */
}

void Inkplate::pinsZstate()
{
    //CONTROL PINS
    pinMode(0, INPUT);
    pinMode(2, INPUT);
    pinMode(32, INPUT);
    pinMode(33, INPUT);
    pinModeMCP(OE, INPUT);
    pinModeMCP(GMOD, INPUT);
    pinModeMCP(SPV, INPUT);

    //DATA PINS
    pinMode(4, INPUT); //D0
    pinMode(5, INPUT);
    pinMode(18, INPUT);
    pinMode(19, INPUT);
    pinMode(23, INPUT);
    pinMode(25, INPUT);
    pinMode(26, INPUT);
    pinMode(27, INPUT); //D7
}

void Inkplate::pinsAsOutputs()
{
    //CONTROL PINS
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    pinModeMCP(OE, OUTPUT);
    pinModeMCP(GMOD, OUTPUT);
    pinModeMCP(SPV, OUTPUT);

    //DATA PINS
    pinMode(4, OUTPUT); //D0
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT); //D7
}

//----------------------------MCP23017 Functions----------------------------
void Inkplate::pinModeMCP(uint8_t _pin, uint8_t _mode) 
{
	uint8_t _port = (_pin / 8) & 1;
	uint8_t _p = _pin % 8;

	switch (_mode)
	{
		case INPUT:
			mcpRegsInt[MCP23017_IODIRA + _port] |= 1 << _p;   //Set it to input
			mcpRegsInt[MCP23017_GPPUA + _port] &= ~(1 << _p); //Disable pullup on that pin
			updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
			updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
			break;

		case INPUT_PULLUP:
			mcpRegsInt[MCP23017_IODIRA + _port] |= 1 << _p;   //Set it to input
			mcpRegsInt[MCP23017_GPPUA + _port] |= 1 << _p;    //Enable pullup on that pin
			updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
			updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
			break;

		case OUTPUT:
			mcpRegsInt[MCP23017_IODIRA + _port] &= ~(1 << _p); //Set it to output
			mcpRegsInt[MCP23017_GPPUA + _port] &= ~(1 << _p); //Disable pullup on that pin
			updateRegister(MCP23017_ADDR, MCP23017_IODIRA + _port, mcpRegsInt[MCP23017_IODIRA + _port]);
			updateRegister(MCP23017_ADDR, MCP23017_GPPUA + _port, mcpRegsInt[MCP23017_GPPUA + _port]);
			break;
	}
}

void Inkplate::digitalWriteMCP(uint8_t _pin, uint8_t _state)
{
	uint8_t _port = (_pin / 8) & 1;
	uint8_t _p = _pin % 8;

	if (mcpRegsInt[MCP23017_IODIRA + _port] & (1 << _p)) return; //Check if the pin is set as an output
	_state ? (mcpRegsInt[MCP23017_GPIOA + _port] |= (1 << _p)) : (mcpRegsInt[MCP23017_GPIOA + _port] &= ~(1 << _p));
	updateRegister(MCP23017_ADDR, MCP23017_GPIOA + _port, mcpRegsInt[MCP23017_GPIOA + _port]);
}

uint8_t Inkplate::digitalReadMCP(uint8_t _pin)
{
	uint8_t _port = (_pin / 8) & 1;
	uint8_t _p = _pin % 8;
	readMCPRegister(MCP23017_ADDR, MCP23017_GPIOA + _port, mcpRegsInt);
	return (mcpRegsInt[MCP23017_GPIOA + _port] & (1 << _p)) ? HIGH : LOW;
}

void Inkplate::setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity)
{
	intPort &= 1;
	mirroring &= 1;
	openDrain &= 1;
	polarity &= 1;
	mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << mirroring);
	mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << openDrain);
	mcpRegsInt[MCP23017_IOCONA + intPort] = (mcpRegsInt[MCP23017_IOCONA + intPort] & ~(1 << 6)) | (1 << polarity);
	updateRegister(MCP23017_ADDR, MCP23017_IOCONA + intPort, mcpRegsInt[MCP23017_IOCONA + intPort]);
}

void Inkplate::setIntPin(uint8_t _pin, uint8_t _mode)
{
	uint8_t _port = (_pin / 8) & 1;
	uint8_t _p = _pin % 8;

	switch (_mode)
	{
		case CHANGE:
			mcpRegsInt[MCP23017_INTCONA + _port] &= ~(1 << _p);
			break;

		case FALLING:
			mcpRegsInt[MCP23017_INTCONA + _port] |= (1 << _p);
			mcpRegsInt[MCP23017_DEFVALA + _port] |= (1 << _p);
			break;

		case RISING:
			mcpRegsInt[MCP23017_INTCONA + _port] |= (1 << _p);
			mcpRegsInt[MCP23017_DEFVALA + _port] &= ~(1 << _p);
			break;
  }
  mcpRegsInt[MCP23017_GPINTENA + _port] |= (1 << _p);
  updateRegister(MCP23017_ADDR, MCP23017_GPINTENA, mcpRegsInt, 6);
}

void Inkplate::removeIntPin(uint8_t _pin)
{
	uint8_t _port = (_pin / 8) & 1;
	uint8_t _p = _pin % 8;
	mcpRegsInt[MCP23017_GPINTENA + _port] &= ~(1 << _p);
	updateRegister(MCP23017_ADDR, MCP23017_GPINTENA, mcpRegsInt, 2);
}

uint16_t Inkplate::getINT()
{
	readMCPRegisters(MCP23017_ADDR, MCP23017_INTFA, mcpRegsInt, 2);
	return ((mcpRegsInt[MCP23017_INTFB] << 8) | mcpRegsInt[MCP23017_INTFA]);
}

uint16_t Inkplate::getINTstate()
{
	readMCPRegisters(MCP23017_ADDR, MCP23017_INTCAPA, mcpRegsInt, 2);
	return ((mcpRegsInt[MCP23017_INTCAPB] << 8) | mcpRegsInt[MCP23017_INTCAPA]);
}

void Inkplate::setPorts(uint16_t _d)
{
	mcpRegsInt[MCP23017_GPIOA] = _d & 0xff;
	mcpRegsInt[MCP23017_GPIOB] = (_d >> 8) & 0xff;
	updateRegister(MCP23017_ADDR, MCP23017_GPIOA, mcpRegsInt, 2);
}

uint16_t Inkplate::getPorts()
{
	readMCPRegisters(MCP23017_ADDR, MCP23017_GPIOA, mcpRegsInt, 2);
	return ((mcpRegsInt[MCP23017_GPIOB] << 8) | (mcpRegsInt[MCP23017_GPIOA]));
}

//--------------------------PRIVATE FUNCTIONS--------------------------------------------
//Display content from RAM to display (1 bit per pixel,. monochrome picture).
void Inkplate::display1b()
{
    for (int i = 0; i < 60000; i++)
    {
        *(D_memory_new + i) &= *(_partial + i);
        *(D_memory_new + i) |= (*(_partial + i));
    }
    uint16_t _pos;
    uint32_t _send;
    uint8_t data;
    uint8_t dram;
    einkOn();
    //clean();
    cleanFast(0, 1);
    cleanFast(1, 12);
    cleanFast(2, 1);
    cleanFast(0, 11);
    cleanFast(2, 1);
    cleanFast(1, 12);
    cleanFast(2, 1);
    cleanFast(0, 11);
    for (int k = 0; k < 3; k++)
    {
        _pos = 59999;
        vscan_start();
        for (int i = 0; i < 600; i++)
        {
            dram = *(D_memory_new + _pos);
            data = LUTB[(dram >> 4) & 0x0F];
            _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            hscan_start(_send);
            data = LUTB[dram & 0x0F];
            _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
            for (int j = 0; j < 99; j++)
            {
                dram = *(D_memory_new + _pos);
                data = LUTB[(dram >> 4) & 0x0F];
                _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
                data = LUTB[dram & 0x0F];
                _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
                _pos--;
            }
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }

    _pos = 59999;
    vscan_start();
    for (int i = 0; i < 600; i++)
    {
        dram = *(D_memory_new + _pos);
        data = LUT2[(dram >> 4) & 0x0F];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
        hscan_start(_send);
        data = LUT2[dram & 0x0F];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        _pos--;
        for (int j = 0; j < 99; j++)
        {
            dram = *(D_memory_new + _pos);
            data = LUT2[(dram >> 4) & 0x0F];
            _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            data = LUT2[dram & 0x0F];
            _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            _pos--;
        }
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
        vscan_end();
    }
    delayMicroseconds(230);

	vscan_start();
    for (int i = 0; i < 600; i++)
	{
		dram = *(D_memory_new + _pos);
		data = 0b00000000;;
		_send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
		hscan_start(_send);
		data = 0b00000000;
		GPIO.out_w1ts = (_send) | CL;
		GPIO.out_w1tc = DATA | CL;
		for (int j = 0; j < 99; j++)
		{
			GPIO.out_w1ts = (_send) | CL;
			GPIO.out_w1tc = DATA | CL;
			GPIO.out_w1ts = (_send) | CL;
			GPIO.out_w1tc = DATA | CL;
		}
		GPIO.out_w1ts = (_send) | CL;
		GPIO.out_w1tc = DATA | CL;
		vscan_end();
	}
	delayMicroseconds(230);  
		
	vscan_start();
	einkOff();
	_blockPartial = 0;
}

//Display content from RAM to display (3 bit per pixel,. 8 level of grayscale, STILL IN PROGRESSS, we need correct wavefrom to get good picture, use it only for pictures not for GFX).
void Inkplate::display3b()
{
    einkOn();
    cleanFast(0, 1);
    cleanFast(1, 12);
    cleanFast(2, 1);
    cleanFast(0, 11);
    cleanFast(2, 1);
    cleanFast(1, 12);
    cleanFast(2, 1);
    cleanFast(0, 11);

    for (int k = 0; k < 8; k++)
    {
        uint8_t *dp = D_memory4Bit + 239999;
        uint32_t _send;
        uint8_t pix1;
        uint8_t pix2;
        uint8_t pix3;
        uint8_t pix4;
        uint8_t pixel;
        uint8_t pixel2;

        vscan_start();
        for (int i = 0; i < 600; i++)
        {
            pixel = 0B00000000;
            pixel2 = 0B00000000;
            pix1 = *(dp--);
            pix2 = *(dp--);
            pix3 = *(dp--);
            pix4 = *(dp--);
            pixel |= (waveform3Bit[pix1 & 0x07][k] << 6) | (waveform3Bit[(pix1 >> 4) & 0x07][k] << 4) | (waveform3Bit[pix2 & 0x07][k] << 2) | (waveform3Bit[(pix2 >> 4) & 0x07][k] << 0);
            pixel2 |= (waveform3Bit[pix3 & 0x07][k] << 6) | (waveform3Bit[(pix3 >> 4) & 0x07][k] << 4) | (waveform3Bit[pix4 & 0x07][k] << 2) | (waveform3Bit[(pix4 >> 4) & 0x07][k] << 0);

            _send = ((pixel & B00000011) << 4) | (((pixel & B00001100) >> 2) << 18) | (((pixel & B00010000) >> 4) << 23) | (((pixel & B11100000) >> 5) << 25);
            hscan_start(_send);
            _send = ((pixel2 & B00000011) << 4) | (((pixel2 & B00001100) >> 2) << 18) | (((pixel2 & B00010000) >> 4) << 23) | (((pixel2 & B11100000) >> 5) << 25);
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;

            for (int j = 0; j < 99; j++)
            {

                pixel = 0B00000000;
                pixel2 = 0B00000000;
                pix1 = *(dp--);
                pix2 = *(dp--);
                pix3 = *(dp--);
                pix4 = *(dp--);
                pixel |= (waveform3Bit[pix1 & 0x07][k] << 6) | (waveform3Bit[(pix1 >> 4) & 0x07][k] << 4) | (waveform3Bit[pix2 & 0x07][k] << 2) | (waveform3Bit[(pix2 >> 4) & 0x07][k] << 0);
                pixel2 |= (waveform3Bit[pix3 & 0x07][k] << 6) | (waveform3Bit[(pix3 >> 4) & 0x07][k] << 4) | (waveform3Bit[pix4 & 0x07][k] << 2) | (waveform3Bit[(pix4 >> 4) & 0x07][k] << 0);

                _send = ((pixel & B00000011) << 4) | (((pixel & B00001100) >> 2) << 18) | (((pixel & B00010000) >> 4) << 23) | (((pixel & B11100000) >> 5) << 25);
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;

                _send = ((pixel2 & B00000011) << 4) | (((pixel2 & B00001100) >> 2) << 18) | (((pixel2 & B00010000) >> 4) << 23) | (((pixel2 & B11100000) >> 5) << 25);
                GPIO.out_w1ts = (_send) | CL;
                GPIO.out_w1tc = DATA | CL;
            }
            GPIO.out_w1ts = (_send) | CL;
            GPIO.out_w1tc = DATA | CL;
            vscan_end();
        }
        delayMicroseconds(230);
    }
    cleanFast(2, 1);
    cleanFast(3, 1);
    vscan_start();
    einkOff();
}

uint32_t Inkplate::read32(uint8_t *c)
{
    return (*(c) | (*(c + 1) << 8) | (*(c + 2) << 16) | (*(c + 3) << 24));
}

uint16_t Inkplate::read16(uint8_t *c)
{
    return (*(c) | (*(c + 1) << 8));
}

void Inkplate::readBmpHeaderSd(SdFile *_f, struct bitmapHeader *_h)
{
    uint8_t header[100];
    _f->rewind();
    _f->read(header, 100);
    _h->signature = read16(header + 0);
    _h->fileSize = read32(header + 2);
    _h->startRAW = read32(header + 10);
    _h->dibHeaderSize = read32(header + 14);
    _h->width = read32(header + 18);
    _h->height = read32(header + 22);
    _h->color = read16(header + 28);
    _h->compression = read32(header + 30);
    return;
}

void Inkplate::readBmpHeaderWeb(WiFiClient *_s, struct bitmapHeader *_h)
{
    uint8_t header[34];
    _s->read(header, 34);
    _h->signature = read16(header + 0);
    _h->fileSize = read32(header + 2);
    _h->startRAW = read32(header + 10);
    _h->dibHeaderSize = read32(header + 14);
    _h->width = read32(header + 18);
    _h->height = read32(header + 22);
    _h->color = read16(header + 28);
    _h->compression = read32(header + 30);
    return;
}

int Inkplate::drawMonochromeBitmapSd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    uint8_t paddingBits = w % 32;
    w /= 32;

    f->seekSet(bmpHeader.startRAW);
    int i, j;
    for (j = 0; j < h; j++)
    {
        uint8_t *bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w * 4);
        for (i = 0; i < w; i++)
        {
            uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < 32; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = f->read() << 24 | f->read() << 16 | f->read() << 8 | f->read();
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
    }
    f->close();
    return 1;
}

int Inkplate::drawGrayscaleBitmap4Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    uint8_t paddingBits = w % 8;
    w /= 8;

    f->seekSet(bmpHeader.startRAW);
    int i, j;

    uint8_t *bufferPtr;

    if (dither)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w * 4 + (paddingBits ? 4 : 0));

        ditherStart(pixelBuffer, bufferPtr, w * 8 + (paddingBits ? 4 : 0), invert, 4);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w * 4 + (paddingBits ? 4 : 0));

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(pixelBuffer, bufferPtr, w * 8 + (paddingBits ? 4 : 0), invert, 4);
        }

        for (i = 0; i < w; i++)
        {
            if (dither)
            {

                for (int n = 0; n < 8; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
                }
            }
            else
            {
                uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
                if (invert)
                    pixelRow = ~pixelRow;
                for (int n = 0; n < 8; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, (pixelRow & (0xFULL << (28 - n * 4))) >> (28 - n * 4 + 1));
                }
            }
        }
        if (paddingBits)
        {
            if (dither)
            {
                for (int n = 0; n < paddingBits; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
                }
            }
            else
            {
                uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
                if (invert)
                    pixelRow = ~pixelRow;
                for (int n = 0; n < paddingBits; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ((pixelRow & (0xFULL << (28 - n * 4)))) >> (28 - n * 4 + 1));
                }
            }
        }
        if (dither)
            ditherSwap(w * 8 + paddingBits);
    }
    f->close();
    return 1;
}

int Inkplate::drawGrayscaleBitmap8Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    char padding = w & 3;
    f->seekSet(bmpHeader.startRAW);
    int i, j;

    uint8_t *bufferPtr;

    if (dither)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w);

        ditherStart(pixelBuffer, bufferPtr, w, invert, 8);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w);

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(pixelBuffer, bufferPtr, w, invert, 8);
        }

        for (i = 0; i < w; i++)
        {
            if (dither)
                drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
            else
            {
                uint8_t px = 0;
                if (invert)
                    px = 255 - *(bufferPtr++);
                else
                    px = *(bufferPtr++);
                drawPixel(i + x, h - 1 - j + y, px >> 5);
            }
        }
    }

    if (dither)
        ditherSwap(w);

    if (padding)
    {
        for (int p = 0; p < 4 - padding; p++)
        {
            f->read();
        }
    }
    f->close();
    return 1;
}

int Inkplate::drawGrayscaleBitmap24Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    char padding = w & 3;
    f->seekSet(bmpHeader.startRAW);
    int i, j;

    uint8_t *bufferPtr;

    if (dither)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w * 3);

        ditherStart(pixelBuffer, bufferPtr, w, invert, 24);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        f->read(pixelBuffer, w * 3);

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(pixelBuffer, bufferPtr, w, invert, 24);
        }

        for (i = 0; i < w; i++)
        {
            //This is the proper way of converting True Color (24 Bit RGB) bitmap file into grayscale, but it takes waaay too much time (full size picture takes about 17s to decode!)
            //float px = (0.2126 * (readByteFromSD(&file) / 255.0)) + (0.7152 * (readByteFromSD(&file) / 255.0)) + (0.0722 * (readByteFromSD(&file) / 255.0));
            //px = pow(px, 1.5);
            //display.drawPixel(i + x, h - j + y, (uint8_t)(px*7));

            if (dither)
                drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
            else
            {
                uint8_t px = 0;
                //So then, we are convertng it to grayscale using good old average and gamma correction (from LUT). With this metod, it is still slow (full size image takes 4 seconds), but much beter than prev mentioned method.
                if (invert)
                    px = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) + ((255 - *(bufferPtr++)) * 722 / 10000);
                else
                    px = (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
                drawPixel(i + x, h - 1 - j + y, px >> 5);
            }

            //drawPixel(i + x, h - j + y, gammaLUT[px]);
            //drawPixel(i + x, h - j + y, px/32);
        }

        if (dither)
            ditherSwap(w);

        if (padding)
        {
            for (int p = 0; p < padding; p++)
            {
                f->read();
            }
        }
    }
    f->close();
    return 1;
}

//Loads first line in current dither buffer
void Inkplate::ditherStart(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits)
{
    for (int i = 0; i < w; ++i)
        if (bits == 24)
        {
            if (invert)
                ditherBuffer[0][i] = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) + ((255 - *(bufferPtr++)) * 722 / 10000);
            else
                ditherBuffer[0][i] = (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
        }
        else if (bits == 8)
        {
            if (invert)
                ditherBuffer[0][i] = 255 - *(bufferPtr++);
            else
                ditherBuffer[0][i] = *(bufferPtr++);
        }
    if (bits == 4)
    {
        int _w = w / 8;
        int paddingBits = w % 8;

        for (int i = 0; i < _w; ++i)
        {
            for (int n = 0; n < 4; n++)
            {
                uint8_t temp = *(bufferPtr++);
                ditherBuffer[0][i * 8 + n * 2] = temp & 0xF0;
                ditherBuffer[0][i * 8 + n * 2 + 1] = (temp & 0x0F) << 4;

                if (invert)
                {
                    ditherBuffer[0][i * 8 + n * 2] = ~ditherBuffer[0][i * 8 + n * 2];
                    ditherBuffer[0][i * 8 + n * 2 + 1] = ~ditherBuffer[0][i * 8 + n * 2 + 1];
                }
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                ditherBuffer[0][_w * 8 + n] = (pixelRow & (0xFULL << ((7 - n) * 4))) >> ((7 - n) * 4 - 4);
            }
        }
    }
}

//Loads next line, after this ditherGetPixel can be called and alters values in next line
void Inkplate::ditherLoadNextLine(uint8_t *pixelBuffer, uint8_t *bufferPtr, int w, bool invert, uint8_t bits)
{
    for (int i = 0; i < w; ++i)
    {
        if (bits == 24)
        {
            if (invert)
                ditherBuffer[1][i] = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) + ((255 - *(bufferPtr++)) * 722 / 10000);
            else
                ditherBuffer[1][i] = (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
        }
        else if (bits == 8)
        {
            if (invert)
                ditherBuffer[1][i] = 255 - *(bufferPtr++);
            else
                ditherBuffer[1][i] = *(bufferPtr++);
        }
    }
    if (bits == 4)
    {
        int _w = w / 8;
        int paddingBits = w % 8;

        for (int i = 0; i < _w; ++i)
        {
            for (int n = 0; n < 4; n++)
            {
                uint8_t temp = *(bufferPtr++);
                ditherBuffer[0][i * 8 + n * 2] = temp & 0xF0;
                ditherBuffer[0][i * 8 + n * 2 + 1] = (temp & 0x0F) << 4;

                if (invert)
                {
                    ditherBuffer[0][i * 8 + n * 2] = ~ditherBuffer[0][i * 8 + n * 2];
                    ditherBuffer[0][i * 8 + n * 2 + 1] = ~ditherBuffer[0][i * 8 + n * 2 + 1];
                }
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                ditherBuffer[1][_w * 8 + n] = (pixelRow & (0xFULL << (28 - n * 4))) >> (28 - n * 4 - 4);
            }
        }
    }
}

//Gets specific pixel, mainly at i, j is just used for bound checking when changing next line values
uint8_t Inkplate::ditherGetPixel(int i, int j, int w, int h)
{
    uint8_t oldpixel = ditherBuffer[0][i];
    uint8_t newpixel = (oldpixel & B11100000);

    ditherBuffer[0][i] = newpixel;

    uint8_t quant_error = oldpixel - newpixel;

    if (i + 1 < w)
        ditherBuffer[0][i + 1] = min(255, (int)ditherBuffer[0][i + 1] + (((int)quant_error * 7) >> 4));
    if (j + 1 < h && 0 <= i - 1)
        ditherBuffer[1][i - 1] = min(255, (int)ditherBuffer[1][i - 1] + (((int)quant_error * 3) >> 4));
    if (j + 1 < h)
        ditherBuffer[1][i + 0] = min(255, (int)ditherBuffer[1][i + 0] + (((int)quant_error * 5) >> 4));
    if (j + 1 < h && i + 1 < w)
        ditherBuffer[1][i + 1] = min(255, (int)ditherBuffer[1][i + 1] + (((int)quant_error * 1) >> 4));

    return newpixel;
}

//Swaps current and next line, for next one to be overwritten
uint8_t Inkplate::ditherSwap(int w)
{
    for (int i = 0; i < w; ++i)
        ditherBuffer[0][i] = ditherBuffer[1][i];
}

int Inkplate::drawMonochromeBitmapWeb(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    uint8_t paddingBits = w % 32;
    int total = len - 34;
    w /= 32;

    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    int pnt = 0;
    while (pnt < total)
    {
        int toread = s->available();
        if (toread > 0)
        {
            int read = s->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }

    int i, j, k = bmpHeader.startRAW - 34;
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            uint32_t pixelRow = buf[k++] << 24 | buf[k++] << 16 | buf[k++] << 8 | buf[k++];
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < 32; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
        if (paddingBits)
        {
            uint32_t pixelRow = buf[k++] << 24 | buf[k++] << 16 | buf[k++] << 8 | buf[k++];
            if (invert)
                pixelRow = ~pixelRow;
            for (int n = 0; n < paddingBits; n++)
            {
                drawPixel((i * 32) + n + x, h - 1 - j + y, !(pixelRow & (1ULL << (31 - n))));
            }
        }
    }

    free(buf);

    return 1;
}

int Inkplate::drawGrayscaleBitmap4Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    char paddingBits = w % 8;
    int total = len - 34;
    w /= 8;

    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    int pnt = 0;
    while (pnt < total)
    {
        int toread = s->available();
        if (toread > 0)
        {
            int read = s->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }

    int i, j;

    uint8_t *bufferPtr;
    uint8_t *f_pointer = buf + (bmpHeader.startRAW - 34);

    if (dither)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w * 4 + (paddingBits ? 1 : 0); i++)
            pixelBuffer[i] = *(f_pointer++);

        ditherStart(pixelBuffer, bufferPtr, w, invert, 4);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w * 4 + (paddingBits ? 1 : 0); i++)
            pixelBuffer[i] = *(f_pointer++);

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(pixelBuffer, bufferPtr, w * 8 + (paddingBits ? 4 : 0), invert, 4);
        }

        for (i = 0; i < w; i++)
        {
            if (dither)
            {

                for (int n = 0; n < 8; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
                }
            }
            else
            {
                uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
                if (invert)
                    pixelRow = ~pixelRow;
                for (int n = 0; n < 8; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, (pixelRow & (0xFULL << (28 - n * 4))) >> (28 - n * 4 + 1));
                }
            }
        }
        if (paddingBits)
        {
            if (dither)
            {
                for (int n = 0; n < paddingBits; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ditherGetPixel((i * 8) + n, h - 1 - j, w * 8 + (paddingBits ? 4 : 0), h) >> 5);
                }
            }
            else
            {
                uint32_t pixelRow = *(bufferPtr++) << 24 | *(bufferPtr++) << 16 | *(bufferPtr++) << 8 | *(bufferPtr++);
                if (invert)
                    pixelRow = ~pixelRow;
                for (int n = 0; n < paddingBits; n++)
                {
                    drawPixel((i * 8) + n + x, h - 1 - j + y, ((pixelRow & (0xFULL << (28 - n * 4)))) >> (28 - n * 4 + 1));
                }
            }
        }
        if (dither)
            ditherSwap(w * 8 + paddingBits);
    }

    free(buf);

    return 1;
}

int Inkplate::drawGrayscaleBitmap8Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    char padding = w % 4;
    int total = len - 34;

    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    uint32_t pnt = 0;
    while (pnt < total)
    {
        int toread = s->available();
        if (toread > 0)
        {
            int read = s->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }

    uint8_t *bufferPtr;
    uint8_t *f_pointer = buf + (bmpHeader.startRAW - 34);

    int i, j;

    if (dither)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w; i++)
            pixelBuffer[i] = *(f_pointer++);

        ditherStart(buf, bufferPtr, w, invert, 8);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w; i++)
            pixelBuffer[i] = *(f_pointer++);

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(buf, bufferPtr, w, invert, 8);
        }

        for (i = 0; i < w; i++)
        {
            if (dither)
                drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
            else
            {
                uint8_t px = 0;
                if (invert)
                    px = 255 - *(bufferPtr++);
                else
                    px = *(bufferPtr++);
                drawPixel(i + x, h - 1 - j + y, px >> 5);
            }
        }

        if (dither)
            ditherSwap(w);

        if (padding)
        {
            for (int p = 0; p < 4 - padding; p++)
            {
                ++bufferPtr;
            }
        }
    }

    free(buf);

    return 1;
}

int Inkplate::drawGrayscaleBitmap24Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool dither, bool invert)
{
    int w = bmpHeader.width;
    int h = bmpHeader.height;
    char padding = w % 4;
    int total = len - 34;

    uint8_t *buf = (uint8_t *)ps_malloc(total);
    if (buf == NULL)
        return 0;

    int pnt = 0;
    while (pnt < total)
    {
        int toread = s->available();
        if (toread > 0)
        {
            int read = s->read(buf + pnt, toread);
            if (read > 0)
                pnt += read;
        }
    }

    uint8_t *bufferPtr;
    uint8_t *f_pointer = buf + (bmpHeader.startRAW - 34);

    int i, j;

    if (dither)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w * 3; i++)
            pixelBuffer[i] = *(f_pointer++);

        ditherStart(buf, bufferPtr, w, invert, 24);
    }

    for (j = 0; j < h; j++)
    {
        bufferPtr = pixelBuffer;
        for (i = 0; i < w * 3; i++)
            pixelBuffer[i] = *(f_pointer++);

        if (dither && j != h - 1)
        {
            ditherLoadNextLine(buf, bufferPtr, w, invert, 24);
        }

        for (i = 0; i < w; i++)
        {
            //This is the proper way of converting True Color (24 Bit RGB) bitmap file into grayscale, but it takes waaay too much time (full size picture takes about 17s to decode!)
            //float px = (0.2126 * (readByteFromSD(&file) / 255.0)) + (0.7152 * (readByteFromSD(&file) / 255.0)) + (0.0722 * (readByteFromSD(&file) / 255.0));
            //px = pow(px, 1.5);
            //display.drawPixel(i + x, h - j + y, (uint8_t)(px*7));

            //So then, we are convertng it to grayscale using good old average and gamma correction (from LUT). With this metod, it is still slow (full size image takes 4 seconds), but much beter than prev mentioned method.
            if (dither)
                drawPixel(i + x, h - 1 - j + y, ditherGetPixel(i, j, w, h) >> 5);
            else
            {
                uint8_t px = 0;
                if (invert)
                    px = ((255 - *(bufferPtr++)) * 2126 / 10000) + ((255 - *(bufferPtr++)) * 7152 / 10000) + ((255 - *(bufferPtr++)) * 722 / 10000);
                else
                    px = (*(bufferPtr++) * 2126 / 10000) + (*(bufferPtr++) * 7152 / 10000) + (*(bufferPtr++) * 722 / 10000);
                //drawPixel(i + x, h - j + y, gammaLUT[px]);
                drawPixel(i + x, h - 1 - j + y, px >> 5);
                //drawPixel(i + x, h - j + y, px/32);
            }
        }

        if (dither)
            ditherSwap(w);

        if (padding)
        {
            for (int p = 0; p < padding; p++)
            {
                ++bufferPtr;
            }
        }
    }

    free(buf);

    return 1;
}

void Inkplate::precalculateGamma(uint8_t *c, float gamma)
{
    for (int i = 0; i < 256; i++)
    {
        c[i] = int(round((pow(i / 255.0, gamma)) * 15));
    }
}

//----------------------------MCP23017 Low Level Functions----------------------------
bool Inkplate::mcpBegin(uint8_t _addr, uint8_t* _r)
{
  Wire.beginTransmission(_addr);
  int error = Wire.endTransmission();
  if (error) return false;
  readMCPRegisters(_addr, _r);
  _r[0] = 0xff;
  _r[1] = 0xff;
  updateAllRegisters(_addr, _r);
  return true;
}

void Inkplate::readMCPRegisters(uint8_t _addr, uint8_t *k)
{
	Wire.beginTransmission(_addr);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(_addr, (uint8_t)22);
	for (int i = 0; i < 22; i++)
	{
		k[i] = Wire.read();
	}
}

void Inkplate::readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
	Wire.beginTransmission(_addr);
	Wire.write(_regName);
	Wire.endTransmission();

	Wire.requestFrom(_addr, _n);
	for (int i = 0; i < _n; i++)
	{
		k[_regName + i] = Wire.read();
	}
}

void Inkplate::readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k)
{
	Wire.beginTransmission(_addr);
	Wire.write(_regName);
	Wire.endTransmission();
	Wire.requestFrom(_addr, (uint8_t)1);
	k[_regName] = Wire.read();
}

void Inkplate::updateAllRegisters(uint8_t _addr, uint8_t *k)
{
	Wire.beginTransmission(_addr);
	Wire.write(0x00);
	for (int i = 0; i < 22; i++)
	{
		Wire.write(k[i]);
	}
	Wire.endTransmission();
}

void Inkplate::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d)
{
	Wire.beginTransmission(_addr);
	Wire.write(_regName);
	Wire.write(_d);
	Wire.endTransmission();
}

void Inkplate::updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n)
{
	Wire.beginTransmission(_addr);
	Wire.write(_regName);
	for (int i = 0; i < _n; i++)
	{
		Wire.write(k[_regName + i]);
	}
	Wire.endTransmission();
}