#ifndef __INKPLATE_H__
#define __INKPLATE_H__
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef ARDUINO_ESP32_DEV
#error "Wrong board selected! Select ESP32 Wrover from board menu!"
#endif

#include "Adafruit_GFX.h"
#include "Wire.h"
#include "SPI.h"
#include "Adafruit_MCP23017.h"
#include "SdFat.h"
#include "WiFiClient.h"

#define INKPLATE_GAMMA 1.45
#define E_INK_WIDTH 800
#define E_INK_HEIGHT 600
#define BLACK 1
#define WHITE 0
#define GPIO0_ENABLE 8
#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define PAD1 0
#define PAD2 1
#define PAD3 2

#define DATA 0x0E8C0030 //D0-D7 = GPIO4 GPIO5 GPIO18 GPIO19 GPIO23 GPIO25 GPIO26 GPIO27

#define CL 0x01 //GPIO0
#define CL_SET              \
    {                       \
        GPIO.out_w1ts = CL; \
    }
#define CL_CLEAR            \
    {                       \
        GPIO.out_w1tc = CL; \
    }

#define LE 0x04 //GPIO2
#define LE_SET              \
    {                       \
        GPIO.out_w1ts = LE; \
    }
#define LE_CLEAR            \
    {                       \
        GPIO.out_w1tc = LE; \
    }

#define CKV 0x01 //GPIO32
#define CKV_SET                   \
    {                             \
        GPIO.out1_w1ts.val = CKV; \
    }
#define CKV_CLEAR                 \
    {                             \
        GPIO.out1_w1tc.val = CKV; \
    }

#define SPH 0x02 //GPIO33
#define SPH_SET                   \
    {                             \
        GPIO.out1_w1ts.val = SPH; \
    }
#define SPH_CLEAR                 \
    {                             \
        GPIO.out1_w1tc.val = SPH; \
    }

//#define SPV         	15   //GPIO15
//#define SPV_SET     	{digitalWrite(SPV, HIGH);}
//#define SPV_CLEAR   	{digitalWrite(SPV, LOW);}

//I/O Expander - A Channel
#define GMOD 1 //GPIOA1
#define GMOD_SET                      \
    {                                 \
        mcp.digitalWrite(GMOD, HIGH); \
    }
#define GMOD_CLEAR                   \
    {                                \
        mcp.digitalWrite(GMOD, LOW); \
    }

#define OE 0 //GPIOA0
#define OE_SET                      \
    {                               \
        mcp.digitalWrite(OE, HIGH); \
    }
#define OE_CLEAR                   \
    {                              \
        mcp.digitalWrite(OE, LOW); \
    }

#define SPV 2 //GPIOA5
#define SPV_SET                      \
    {                                \
        mcp.digitalWrite(SPV, HIGH); \
    }
#define SPV_CLEAR                   \
    {                               \
        mcp.digitalWrite(SPV, LOW); \
    }

#define WAKEUP 3 //GPIOA3
#define WAKEUP_SET                      \
    {                                   \
        mcp.digitalWrite(WAKEUP, HIGH); \
    }
#define WAKEUP_CLEAR                   \
    {                                  \
        mcp.digitalWrite(WAKEUP, LOW); \
    }

#define PWRUP 4 //GPIOA4
#define PWRUP_SET                      \
    {                                  \
        mcp.digitalWrite(PWRUP, HIGH); \
    }
#define PWRUP_CLEAR                   \
    {                                 \
        mcp.digitalWrite(PWRUP, LOW); \
    }

#define VCOM 5 //GPIOA6
#define VCOM_SET                      \
    {                                 \
        mcp.digitalWrite(VCOM, HIGH); \
    }
#define VCOM_CLEAR                   \
    {                                \
        mcp.digitalWrite(VCOM, LOW); \
    }

#define CKV_CLOCK ckvClock();

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) \
    {                       \
        int16_t t = a;      \
        a = b;              \
        b = t;              \
    }
#endif

extern Adafruit_MCP23017 mcp;
extern SPIClass spi2;
extern SdFat sd;
static void ckvClock();
static void usleep1();

class Inkplate : public Adafruit_GFX
{
public:
    uint8_t *D_memory_new;
    uint8_t *_partial;
    uint8_t *D_memory4Bit;
    uint8_t *_pBuffer;
    const uint8_t LUT2[16] ={ B10101010, B10101001, B10100110, B10100101, B10011010, B10011001, B10010110, B10010101, B01101010, B01101001, B01100110, B01100101, B01011010, B01011001, B01010110, B01010101 };
    const uint8_t LUTW[16] ={ B11111111, B11111110, B11111011, B11111010, B11101111, B11101110, B11101011, B11101010, B10111111, B10111110, B10111011, B10111010, B10101111, B10101110, B10101011, B10101010 };
    const uint8_t LUTB[16] ={ B11111111, B11111101, B11110111, B11110101, B11011111, B11011101, B11010111, B11010101, B01111111, B01111101, B01110111, B01110101, B01011111, B01011101, B01010111, B01010101 };
    const uint8_t pixelMaskLUT[8] ={ B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };
    const uint8_t pixelMaskGLUT[2] ={ B00001111, B11110000 };
    const uint8_t discharge[16] ={ B11111111, B11111100, B11110011, B11110000, B11001111, B11001100, B11000011, B11000000, B00111111, B00111100, B00110011, B00110000, B00001111, B00001100, B00000011, B00000000 };
    //BLACK->WHITE
    //THIS IS OKAYISH WAVEFORM FOR GRAYSCALE. IT CAN BE MUCH BETTER.
    const uint8_t waveform3Bit[8][7] ={ { 0, 0, 0, 0, 1, 1, 1 }, { 0, 0, 1, 1, 1, 2, 1 }, { 0, 1, 1, 2, 1, 2, 1 }, { 0, 0, 1, 1, 2, 1, 2 }, { 1, 1, 1, 2, 2, 1, 2 }, { 0, 0, 1, 1, 1, 2, 2 }, { 0, 1, 1, 2, 1, 2, 2 }, { 0, 0, 0, 0, 0, 0, 2 } };
    //const uint8_t waveform3Bit[8][12] = {{3,3,3,1,1,1,1,1,1,1,2,0}, {3,3,3,3,1,1,1,1,1,1,2,0}, {3,3,3,3,3,1,1,1,1,1,2,0}, {3,3,3,3,3,3,1,1,1,1,2,0}, {3,3,3,3,3,3,3,1,1,1,2,0}, {3,3,3,3,3,3,3,2,1,1,2,0}, {3,3,3,3,3,3,3,3,3,1,2,0}, {3,3,3,3,3,3,3,3,3,3,2,0}};
    //const uint8_t waveform3Bit[16][12] = {{0,0,0,0,0,0,1,2,1,1,0,3},{0,0,1,1,1,2,2,2,1,1,0,3},{0,0,0,1,1,2,2,2,1,1,0,3},  {0,0,0,1,2,1,2,1,2,1,3}, {0,0,2,1,2,1,2,1,2,1,3}, {0,0,1,2,2,1,1,1,1,2,0,3}, {0,0,0,2,1,1,1,1,0,2,0,3}, {0,0,2,1,2,2,1,1,1,2,0,3}, {0,0,0,2,2,2,1,1,1,2,0,3}, {0,0,0,0,0,0,2,1,1,2,0,3}, {0,0,0,0,0,2,2,1,1,2,0,3}, {0,0,0,0,0,1,1,1,2,2,0,3}, {0,0,0,0,1,2,1,2,1,2,0,3}, {0,0,0,0,1,1,2,2,1,2,0,3},{0,0,0,0,1,1,1,2,2,2,0,3}, {0,0,0,0,0,0,0,0,0,2,0,3}};
    //PVI waveform for cleaning screen, not sure if it is correct, but it cleans screen properly.
    const uint32_t waveform[50] ={ 0x00000008, 0x00000008, 0x00200408, 0x80281888, 0x60a81898, 0x60a8a8a8, 0x60a8a8a8, 0x6068a868, 0x6868a868, 0x6868a868, 0x68686868, 0x6a686868, 0x5a686868, 0x5a686868, 0x5a586a68, 0x5a5a6a68, 0x5a5a6a68, 0x55566a68, 0x55565a64, 0x55555654, 0x55555556, 0x55555556, 0x55555556, 0x55555516, 0x55555596, 0x15555595, 0x95955595, 0x95959595, 0x95949495, 0x94949495, 0x94949495, 0xa4949494, 0x9494a4a4, 0x84a49494, 0x84948484, 0x84848484, 0x84848484, 0x84848484, 0xa5a48484, 0xa9a4a4a8, 0xa9a8a8a8, 0xa5a9a9a4, 0xa5a5a5a4, 0xa1a5a5a1, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0x15151515, 0x11111111 };

    struct bitmapHeader
    {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t startRAW;
        uint32_t dibHeaderSize;
        uint32_t width;
        uint32_t height;
        uint16_t color;
        uint32_t compression;
    };

    Inkplate(uint8_t _mode);
    void begin(void);
    uint16_t getPixel(int16_t x0, int16_t y0);
    void drawPixel(int16_t x0, int16_t y0, uint16_t color);
    void clearDisplay();
    void display();
    void partialUpdate();
    void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h);
    void setRotation(uint8_t);
    void einkOff(void);
    void einkOn(void);
    void selectDisplayMode(uint8_t _mode);
    uint8_t getDisplayMode();
    int drawBitmapFromSD(SdFile *p, int x, int y, bool dither = false, bool invert = false);
    int drawBitmapFromSD(char *fileName, int x, int y, bool dither = false, bool invert = false);
    int drawBitmapFromWeb(WiFiClient *s, int x, int y, int len, bool invert = false);
    int drawBitmapFromWeb(char *url, int x, int y, bool invert = false);
    void drawThickLine(int x1, int y1, int x2, int y2, int color, float thickness);
    void drawGradientLine(int x1, int y1, int x2, int y2, int color1, int color2, float thickness = -1);
    int sdCardInit();
    SdFat getSdFat();
    SPIClass getSPI();
    uint8_t getPanelState();
    uint8_t readTouchpad(uint8_t);
    int8_t readTemperature();
    double readBattery();
    void vscan_start();
    void vscan_write();
    void hscan_start(uint32_t _d = 0);
    void vscan_end();
    void clean();
    void cleanFast(uint8_t c, uint8_t rep);
    void cleanFast2(uint8_t c, uint8_t n, uint16_t d);
    void pinsZstate();
    void pinsAsOutputs();

private:
    uint8_t gammaLUT[256];
    uint8_t pixelBuffer[800 * 3 + 5];
    uint8_t ditherBuffer[800 * 3 + 5][2];
    int8_t _temperature;
    uint8_t _panelOn = 0;
    uint8_t _rotation = 0;
    uint8_t _displayMode = 0; //By default, 1 bit mode is used
    int sdCardOk = 0;
    uint8_t _blockPartial = 1;
    uint8_t _beginDone = 0;

    void display1b();
    void display3b();
    uint32_t read32(uint8_t *c);
    uint16_t read16(uint8_t *c);
    void ditherStart(uint8_t *pixelBuffer, uint8_t* bufferPtr, int w, bool invert, uint8_t bits);
    void ditherLoadNextLine(uint8_t *pixelBuffer, uint8_t* bufferPtr, int w, bool invert, uint8_t bits);
    uint8_t ditherGetPixel(int i, int j, int w, int h);
    uint8_t ditherSwap(int w);
    void readBmpHeaderSd(SdFile *_f, struct bitmapHeader *_h);
    void readBmpHeaderWeb(WiFiClient *_s, struct bitmapHeader *_h);
    int drawMonochromeBitmapSd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool invert);
    int drawGrayscaleBitmap4Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool invert);
    int drawGrayscaleBitmap8Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool dither, bool invert);
    int drawGrayscaleBitmap24Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y, bool dither, bool invert);
    int drawMonochromeBitmapWeb(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert);
    int drawGrayscaleBitmap4Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert);
    int drawGrayscaleBitmap8Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert);
    int drawGrayscaleBitmap24Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert);
    void precalculateGamma(uint8_t *c, float gamma);
};

#endif
