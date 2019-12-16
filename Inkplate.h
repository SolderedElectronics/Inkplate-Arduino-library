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

#include <Adafruit_GFX.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#define E_INK_WIDTH 800
#define E_INK_HEIGHT 600
extern Adafruit_MCP23017 mcp;

#define DATA    0x0E8C0030   //D0-D7 = GPIO4 GPIO5 GPIO18 GPIO19 GPIO23 GPIO25 GPIO26 GPIO27

#define CL        0x01    //GPIO0
#define CL_SET    {GPIO.out_w1ts = CL;}
#define CL_CLEAR  {GPIO.out_w1tc = CL;}

#define LE        0x04     //GPIO2
#define LE_SET    {GPIO.out_w1ts = LE;}
#define LE_CLEAR  {GPIO.out_w1tc = LE;}

#define CKV       0x01   //GPIO32
#define CKV_SET   {GPIO.out1_w1ts.val = CKV;}
#define CKV_CLEAR {GPIO.out1_w1tc.val = CKV;}

#define SPH         0x02   //GPIO33
#define SPH_SET     {GPIO.out1_w1ts.val = SPH;}
#define SPH_CLEAR   {GPIO.out1_w1tc.val = SPH;}

//I/O Expander - A Channel
#define GMOD       1    //GPIOA1
#define GMOD_SET    {mcp.digitalWrite(GMOD, HIGH);}
#define GMOD_CLEAR  {mcp.digitalWrite(GMOD, HIGH);}

#define OE          0    //GPIOA0
#define OE_SET      {mcp.digitalWrite(OE, HIGH);}
#define OE_CLEAR    {mcp.digitalWrite(OE, LOW);}

#define SPV         2   //GPIOA5
#define SPV_SET     {mcp.digitalWrite(SPV, HIGH);}
#define SPV_CLEAR   {mcp.digitalWrite(SPV, LOW);}

#define WAKEUP         3   //GPIOA3
#define WAKEUP_SET     {mcp.digitalWrite(WAKEUP, HIGH);}
#define WAKEUP_CLEAR   {mcp.digitalWrite(WAKEUP, LOW);}

#define PWRUP         4   //GPIOA4
#define PWRUP_SET     {mcp.digitalWrite(PWRUP, HIGH);}
#define PWRUP_CLEAR   {mcp.digitalWrite(PWRUP, LOW);}

#define VCOM         5   //GPIOA6
#define VCOM_SET     {mcp.digitalWrite(VCOM, HIGH);}
#define VCOM_CLEAR   {mcp.digitalWrite(VCOM, LOW);}

#define GDISP_SCREEN_HEIGHT 600
#define CKV_CLOCK ckvClock();
#define SCLOCK        {  CL_SET; CL_CLEAR; }

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define BLACK 1
#define WHITE 0
#define GPIO0_ENABLE 8
#define REF_RATE 6
#define INKPLATE_1BIT 0
#define INKPLATE_3BIT 1
#define PAD1 0
#define PAD2 1
#define PAD3 2

static void ckvClock();
static void usleep1();

class Inkplate : public Adafruit_GFX {
  public:
    Inkplate(uint8_t _mode);
    Inkplate();
    uint8_t* D_memory_new;
    uint8_t* D_memory4Bit;
    const uint8_t LUT2[16] = {B10101010, B10101001, B10100110, B10100101, B10011010, B10011001, B10010110, B10010101, B01101010, B01101001, B01100110, B01100101, B01011010, B01011001, B01010110, B01010101};
    const uint8_t pixelMaskLUT[8] = {B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000};
    const uint8_t pixelMaskGLUT[2] = {B00001111, B11110000};
    const uint8_t pixel_to_epd_cmd[3] = {B00000001, B00000010, B00000011};

    //PVI waveform for cleaning screen, not sure if it is correct, but it cleans screen properly.
    const uint32_t waveform[50] = {0x00000008, 0x00000008, 0x00200408, 0x80281888, 0x60a81898, 0x60a8a8a8, 0x60a8a8a8, 0x6068a868, 0x6868a868, 0x6868a868, 0x68686868, 0x6a686868, 0x5a686868, 0x5a686868, 0x5a586a68, 0x5a5a6a68, 0x5a5a6a68, 0x55566a68, 0x55565a64, 0x55555654, 0x55555556, 0x55555556, 0x55555556, 0x55555516, 0x55555596, 0x15555595, 0x95955595, 0x95959595, 0x95949495, 0x94949495, 0x94949495, 0xa4949494, 0x9494a4a4, 0x84a49494, 0x84948484, 0x84848484, 0x84848484, 0x84848484, 0xa5a48484, 0xa9a4a4a8, 0xa9a8a8a8, 0xa5a9a9a4, 0xa5a5a5a4, 0xa1a5a5a1, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0x15151515, 0x11111111};

    //Settings for contrast. Basicly, each element in array describes how many times each color is written to display (starting form darkest to lightest).
    //This is for 3 bit mode, but you can expant to 8 bit mode if you want, by adding more wariables, changing display seqence in display4Bit() and expanding the memory buffer size to double the current size.
    const uint8_t contrast_cycles[3] = {2, 2, 2};
    const uint8_t sz_contrast_cycles = sizeof(contrast_cycles) / sizeof(uint8_t);

    void drawPixel(int16_t x0, int16_t y0, uint16_t color);
    void begin(void);
    void clearDisplay();
    void display();
    void draw_mode_off();
    void draw_mode_on();
    void advance_line();
    void begin_frame();
    void end_frame();
    void begin_line();
    void end_line();
    void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char* _p, int16_t _w, int16_t _h);
    void fillScreen(uint8_t c);
    void cleanFast(uint8_t c);
    void clean();
    void setRotation(uint8_t);
    void einkOff(void);
    void einkOn(void);
    void selectDisplayMode(uint8_t _mode);
    uint8_t getDisplayMode();
    void end_line_slow();
    void pinsZstate();
    void pinsAsOutputs();
    uint8_t getPanelState();
	uint8_t readTouchpad(uint8_t);
	int8_t readTemperature();
	double readBattery();

  private:
	int8_t _temperature;
    void display1b();
    void display3b();
    uint8_t _panelOn=0;
    uint8_t _rotation = 0;
    uint16_t _tempRotation;
    uint8_t _displayMode = 0; //By default, 1 bit mode is used
};

#endif
