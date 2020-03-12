#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "Inkplate.h"
Adafruit_MCP23017 mcp;

Inkplate::Inkplate(uint8_t _mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT) {
  _displayMode = _mode;
}

Inkplate::Inkplate() : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT) {
}

void Inkplate::begin(void) {
  Wire.begin();
  mcp.begin(0);
  mcp.pinMode(VCOM, OUTPUT);
  mcp.pinMode(PWRUP, OUTPUT);
  mcp.pinMode(WAKEUP, OUTPUT);
  mcp.pinMode(GPIO0_ENABLE, OUTPUT);
  mcp.digitalWrite(GPIO0_ENABLE, HIGH);

  //CONTROL PINS
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  mcp.pinMode(OE, OUTPUT);
  mcp.pinMode(GMOD, OUTPUT);
  mcp.pinMode(SPV, OUTPUT);

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
  mcp.pinMode(10, INPUT);
  mcp.pinMode(11, INPUT);
  mcp.pinMode(12, INPUT);
  
  //Battery voltage Switch MOSFET
  mcp.pinMode(9, OUTPUT);
  
  //1 bit per pixel mode (monochrome mode)
  if (_displayMode == 0) {
    D_memory_new = (uint8_t*)ps_malloc(600 * 100);
	_partial = (uint8_t*)ps_malloc(600*100);
	_pBuffer = (uint8_t*) ps_malloc(120000);
    if (D_memory_new == NULL || _partial == NULL || _pBuffer == NULL) {
      do {
        delay(100);
      } while (true);
    }
    memset(D_memory_new, 0, 60000);
	memset(_partial, 0, 60000);
  }

  //3 bit per pixel mode (8 level grayscale mode)
  if (_displayMode == 1) {
    D_memory4Bit = (uint8_t*)ps_malloc(240000);
    if (D_memory4Bit == NULL ) {
      do {
        delay(100);
      } while (true);
    }
    memset(D_memory4Bit, 255, 240000);
  }
}

void Inkplate::clearDisplay() {
  //Clear 1 bit per pixel display buffer
  if (_displayMode == 0) memset(_partial, 0, 60000);

  //Clear 3 bit per pixel display buffer
  if (_displayMode == 1) memset(D_memory4Bit, 255, 240000);
}

void Inkplate::draw_mode_on() {
  einkOn();
  SPH_SET;
  SPV_SET;
  delay(1);
  OE_SET;
  delay(1);
  GMOD_SET;
  CKV_SET;
  delay(5);
}

void Inkplate::draw_mode_off() {
  CKV_CLOCK;
  GMOD_CLEAR;
  CKV_CLOCK;
  GPIO.out &= ~DATA;
  CL_CLEAR;
  LE_CLEAR;
  OE_CLEAR;
  SPV_CLEAR;
  CKV_CLEAR;
  SPH_CLEAR;
  einkOff();
}

void Inkplate::advance_line() {
  CKV_CLEAR;
  usleep1();
  CKV_SET;
  usleep1();
}

void Inkplate::begin_frame() {
	
  SPV_SET;
  delayMicroseconds(100); //usleep(500);
  SPV_CLEAR;
  //usleep1();
  CKV_CLEAR;
  usleep1();
  //delayMicroseconds(20); //usleep(25);
  CKV_SET
  usleep1();
  SPV_SET;
  usleep1();
  //delayMicroseconds(20); //usleep(25);
  
  
  //SPV_SET; //STV=1
  //int loop = 2;
  //while(loop--)
  //{
  //  CKV_CLEAR; //CPV=0
  //  usleep1();
  //  CKV_SET; //CPV=1
  //  usleep1();
  //}
  //SPV_CLEAR; //STV=0
  //loop = 2;
  //while(loop--)
  //{
  //  CKV_CLEAR; //CPV=0
  //  usleep1();
  //  CKV_SET; //CPV=1
  //  usleep1();
  //}
  //SPV_SET; //STV=1
  //loop = 2;
  //while(loop--)
  //{
  //  CKV_CLEAR; //CPV=0
  //  usleep1();
  //  CKV_SET; //CPV=1
  //  usleep1();
  //}

  //Skip three lines to get to the start of the screen
  advance_line();
  advance_line();
  advance_line();
}

void Inkplate::end_frame() {

}

void Inkplate::begin_line() {
  SPH_CLEAR;
  usleep1();
//  LE_SET; //LE =1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//LE_CLEAR; //LE=0
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//EPD_OE_H; //OE =1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//SPH_CLEAR; //SPH=0
}

void Inkplate::end_line() {

  SPH_SET;
  //usleep1();

  CKV_CLEAR;
  //CL_SET;
  usleep1();
  CL_CLEAR;
  //usleep1();
  CKV_SET;
  usleep1();
  LE_SET;
  //usleep1();
  LE_CLEAR;
  //usleep1();
 
  
//  SPH_SET; //STH=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CKV_CLEAR; //CPV=0 – zegar taktujacy bufor GATE
//EPD_OE_L; //OE =0
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CL_CLEAR; //CL=0
//CL_SET; //CL=1
//CKV_SET; //CPV=1
}

void Inkplate::end_line_slow() {
  SPH_SET;
  //usleep1();
  CKV_CLEAR;
  CL_SET;
  delayMicroseconds(20);
  //usleep1();
  CL_CLEAR;
  CKV_SET;
  delayMicroseconds(4);
  LE_SET;
  //usleep1();
  LE_CLEAR;
  //usleep1();
}

//For precise 1uS timing, we cannot use delayMicroseconds(), instead we use ASM with nop command. Initial Z value will be difeerent on different CPU speeds! (for 240 MHz CPU Clock z = 25)
void usleep1() {
  int z = 27;
  while (z--) {
    asm("NOP");
  };
}

//Draw function, used by Adafruit GFX.
void Inkplate::drawPixel(int16_t x0, int16_t y0, uint16_t color) {
  if (x0 > 799 || y0 > 599 || x0 < 0 || y0 < 0) return;

  switch (_rotation) {
    case 1:
      _swap_int16_t(x0, y0);
      x0 = _width - x0 - 1;
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

  if (_displayMode == 0) {
    int x = x0 / 8;
    int x_sub = x0 % 8;
    uint8_t temp = *(_partial + 100 * y0 + x); //D_memory_new[99 * y0 + x];                     //-->> Doesn't work with index
    //D_memory_new[100 * y0 + x] = ~pixelMaskLUT[x_sub] & temp | (color ? pixelMaskLUT[x_sub] : 0); //-->> Doesn't work with index
    *(_partial + 100 * y0 + x) = ~pixelMaskLUT[x_sub] & temp | (color ? pixelMaskLUT[x_sub] : 0);
  } else {
    color &= 7;
    int x = x0 / 2;
    int x_sub = x0 % 2;
    uint8_t temp;
    temp = *(D_memory4Bit + 400 * y0 + x);
    *(D_memory4Bit + 400 * y0 + x) = pixelMaskGLUT[x_sub] & temp | (x_sub ? color : color << 4);
  }
}



//Function that displays content from RAM to screen
void Inkplate::display() {
  if (_displayMode == 0) display1b();
  if (_displayMode == 1) display3b();
}

//Display content from RAM to display (1 bit per pixel,. monochrome picture).
void Inkplate::display1b() {
  for(int i = 0; i<60000; i++) {
	  *(D_memory_new+i) &= *(_partial+i);
	  *(D_memory_new+i) |= (*(_partial+i));
  }
  uint16_t _pos;
  uint32_t _send;
  uint8_t data;
  draw_mode_on();
  SPV_SET;
  delayMicroseconds(500);

  //cleanFast(2);
  cleanFast(2);
  for(int i = 0; i<8; i++) {
	  cleanFast(0);
	  delayMicroseconds(500);
  }
  for(int i = 0; i<8; i++) {
	  cleanFast(1);
	  delayMicroseconds(500);
  }
  for(int i = 0; i<8; i++) {
	  cleanFast(0);
	  delayMicroseconds(500);
  }
  //for(int i = 0; i<2; i++) {
	  //cleanFast(2);
  //}
  //for (int i = 0; i < 6; i++) {
  //  cleanFast(1);
  //  delayMicroseconds(500);
  //}

  //for (int i = 0; i < 6; i++) {
  //  cleanFast(0);
  //  delayMicroseconds(500);
  //}

  //for (int i = 0; i < 1; i++) {
  //  cleanFast(1);
    //delayMicroseconds(500);
  //}
	
  for (int k = 0; k < 6; k++) {
    begin_frame();
	_pos = 59999;
    for (int i = 0; i < 600; i++) {
      //data = 10101010;
      //_send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
      GPIO.out_w1tc = DATA;
      //GPIO.out_w1ts = _send;
      //CL_SET;
      CL_CLEAR;
      begin_line();
      for (int j = 0; j < 100; j++) {
        data = LUTB[(*(D_memory_new + _pos) >> 4)];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
          GPIO.out_w1tc = DATA | CL;
          GPIO.out_w1ts = (_send) | CL;
        data = LUTB[*(D_memory_new + _pos) & 0x0F];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
          GPIO.out_w1tc = DATA | CL;
          GPIO.out_w1ts = (_send) | CL;
		_pos--;
      }
      end_line();
    }
    end_frame();
    delayMicroseconds(500);
  }
  cleanFast(2);
  cleanFast(2);
  delayMicroseconds(500);
  draw_mode_off();
}

void Inkplate::partialUpdate() {
  if(_displayMode == 1) return;
  uint16_t _pos = 59999;
  uint32_t _send;
  uint8_t data;
  uint8_t diffw, diffb;
  uint32_t n = 119999;
  
  draw_mode_on();
  SPV_SET;
  delayMicroseconds(500);
  
   for (int i = 0; i < 600; i++) {
      for (int j = 0; j < 100; j++) {
		diffw = ((*(D_memory_new+_pos))^(*(_partial+_pos)))&(~(*(_partial+_pos)));
		diffb = ((*(D_memory_new+_pos))^(*(_partial+_pos)))&((*(_partial+_pos)));
		_pos--;
		*(_pBuffer+n) = LUTW[diffw>>4] & (LUTB[diffb>>4]);
		n--;
		*(_pBuffer+n) = LUTW[diffw&0x0F] & (LUTB[diffb&0x0F]);
		n--;
	  }
   }	  
   
  for (int k = 0; k < 7; k++) {
    begin_frame();
   n = 119999;
    for (int i = 0; i < 600; i++) {
      //data = 10101010;
      //_send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
      GPIO.out_w1tc = DATA;
      //GPIO.out_w1ts = _send;
      //CL_SET;
      CL_CLEAR;
      begin_line();
      for (int j = 0; j < 200; j++) {
        //_pos = i * 200 + j;
		////diffw = ((*(D_memory_new+_pos))^(*(_partial+_pos)))&(~(*(_partial+_pos)));
		////diffb = ((*(D_memory_new+_pos))^(*(_partial+_pos)))&((*(_partial+_pos)));
		////data = LUTW[diffw>>4] & (LUTB[diffb>>4]);
        //data = LUT2[(*(D_memory_new + _pos) >> 4)];
		data = *(_pBuffer + n);
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
          GPIO.out_w1tc = DATA | CL;
          GPIO.out_w1ts = (_send) | CL;
        //data = LUT2[*(D_memory_new + _pos) & 0x0F];
		////data = LUTW[diffw&0x0F] & (LUTB[diffb&0x0F]);
        ////_send = ((_pBuffer & B00000011) << 4) | (((_pBuffer & B00001100) >> 2) << 18) | (((_pBuffer & B00010000) >> 4) << 23) | (((_pBuffer & B11100000) >> 5) << 25);
        ////GPIO.out_w1tc = DATA | CL;
        ////GPIO.out_w1ts = _send;
        ////GPIO.out_w1ts = CL;
		n--;
      }
      end_line();
    }
    end_frame();
    delayMicroseconds(500);
  }
  for(int i = 0; i<60000; i++) {
	  *(D_memory_new+i) &= *(_partial+i);
	  *(D_memory_new+i) |= (*(_partial+i));
  }
  
  cleanFast(2);
  delayMicroseconds(500);
  draw_mode_off();
}

//Display content from RAM to display (3 bit per pixel,. 8 level of grayscale, STILL IN PROGRESSS, we need correct wavefrom to get good picture, use it only for pictures not for GFX).
void Inkplate::display3b() {
  draw_mode_on();
  SPV_SET;
  delayMicroseconds(500);
  cleanFast(2);
  //cleanFast(2);
  //for(int i = 0; i<5; i++) {
  //	cleanFast(0);
  //}
  for(int i = 0; i<5; i++) {
	  cleanFast(0);
	  //delay(1);
  } 
  delay(5);
  for(int i = 0; i<5; i++) {
	  cleanFast(0);
	  //delay(1);
  } 
  delay(5);
  for(int i = 0; i<7; i++) {
	  cleanFast(1);
	  //delay(1);
  }
  delay(5);
  for(int i = 0; i<7; i++) {
	  cleanFast(0);
	  //delay(1);
  }
  delay(5);
  //cleanFast(1);
  cleanFast(2);
  //cleanFast(0);
  //for(int i = 0; i<3; i++) {
  //	  cleanFast(2);
  //}
  
  for (int k = 0; k < 12; k++) {
  //for (int k = 0; k < sz_contrast_cycles; ++k) {
    //for (int contrast_cnt = 0; contrast_cnt < contrast_cycles[k]; ++contrast_cnt) {
      begin_frame();
      uint8_t *dp = D_memory4Bit + 239999;
      uint32_t _send;
      uint8_t pix1;
      uint8_t pix2;
      uint8_t pix3;
      uint8_t pix4;

      for (int i = 0; i < 600; i++) {
		//CL_SET;
        //GPIO.out_w1tc = DATA;
        //CL_CLEAR;
        begin_line();
		//portDISABLE_INTERRUPTS();
        for (int j = 0; j < 100; j++) {

          uint8_t pixel = 0B00000000;
          uint8_t pixel2 = 0B00000000;

          //4 bit mode (non-reversed bits)
          //pix1 = (*(dp) >> k) & 1;  //4, 5, 6, 7
          //pix2 = (*(dp--) >> k + 4) & 1; //0, 1, 2, 3
          //pix3 = (*(dp) >> k) & 1;
          //pix4 = (*(dp--) >> k + 4) & 1;
		  
		  //pix1 = (*(dp)) & 0x07;  //4, 5, 6, 7
          //pix2 = (*(dp--) >> 4)& 0x07; //0, 1, 2, 3
          //pix3 = (*(dp)) & 0x07;
          //pix4 = (*(dp--) >> 4)& 0x07;
		  pix1 = *(dp--);
          pix2 = *(dp--);
		  pix3 = *(dp--);
          pix4 = *(dp--);
          //pixel |= ( pixel_to_epd_cmd[pix1] << 6) | ( pixel_to_epd_cmd[pix2] << 4) | ( pixel_to_epd_cmd[pix3] << 2) | ( pixel_to_epd_cmd[pix4] << 0);
		  //pixel |= ( waveform3Bit[pix1][k] << 6) | ( waveform3Bit[pix2][k] << 4) | ( waveform3Bit[pix3][k] << 2) | ( waveform3Bit[pix4][k] << 0);
		  pixel |= ( waveform3Bit[pix1&0x07][k] << 6) | ( waveform3Bit[(pix1>>4)&0x07][k] << 4) | ( waveform3Bit[pix2&0x07][k] << 2) | ( waveform3Bit[(pix2>>4)&0x07][k] << 0);
		  pixel2 |= ( waveform3Bit[pix3&0x07][k] << 6) | ( waveform3Bit[(pix3>>4)&0x07][k] << 4) | ( waveform3Bit[pix4&0x07][k] << 2) | ( waveform3Bit[(pix4>>4)&0x07][k] << 0);

          _send = ((pixel & B00000011) << 4) | (((pixel & B00001100) >> 2) << 18) | (((pixel & B00010000) >> 4) << 23) | (((pixel & B11100000) >> 5) << 25);
          GPIO.out_w1tc = DATA | CL;
          GPIO.out_w1ts = (_send) | CL;
          //GPIO.out_w1ts = CL;

          //4 bit mode (non-reversed bits)
          //pix1 = (*(dp) >> k) & 1;  //4, 5, 6, 7
          //pix2 = (*(dp--) >> k + 4) & 1; //0, 1, 2, 3
          //pix3 = (*(dp) >> k) & 1;
          //pix4 = (*(dp--) >> k + 4) & 1;

          //pixel2 |= ( pixel_to_epd_cmd[pix1] << 6);
          //pixel2 |= ( pixel_to_epd_cmd[pix2] << 4);
          //pixel2 |= ( pixel_to_epd_cmd[pix3] << 2);
          //pixel2 |= ( pixel_to_epd_cmd[pix4] << 0);
		  
		  //pix1 = (*(dp)) & 0x07;  //4, 5, 6, 7
          //pix2 = (*(dp--) >> 4)& 0x07; //0, 1, 2, 3
          //pix3 = (*(dp)) & 0x07;
          //pix4 = (*(dp--) >> 4)& 0x07;

          //pixel |= ( pixel_to_epd_cmd[pix1] << 6) | ( pixel_to_epd_cmd[pix2] << 4) | ( pixel_to_epd_cmd[pix3] << 2) | ( pixel_to_epd_cmd[pix4] << 0);
		  //pixel2 |= ( waveform3Bit[pix1][k] << 6) | ( waveform3Bit[pix2][k] << 4) | ( waveform3Bit[pix3][k] << 2) | ( waveform3Bit[pix4][k] << 0);

          _send = ((pixel2 & B00000011) << 4) | (((pixel2 & B00001100) >> 2) << 18) | (((pixel2 & B00010000) >> 4) << 23) | (((pixel2 & B11100000) >> 5) << 25);
          GPIO.out_w1tc = DATA | CL;
          GPIO.out_w1ts = (_send) | CL;
          //GPIO.out_w1ts = CL;
        }
		//portENABLE_INTERRUPTS();
        end_line();
      }
      end_frame();
    //}
  }
  
  cleanFast(2);
  delayMicroseconds(500);
  draw_mode_off();
}

void ckvClock() {
  CKV_CLEAR;
  usleep1();
  CKV_SET;
  usleep1();
}

void Inkplate::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char* _p, int16_t _w, int16_t _h) {
  if (_displayMode != INKPLATE_3BIT) return;
  uint8_t  _rem = _w % 2;
  int i, j;
  int xSize = _w / 2 + _rem;

  //if (_shiftX == 0) {
  //  for (int i = _y; i < _y + _h; i++) {
  //    memcpy(D_memory4Bit + (400 * i) + _x/2, _p + _w/2 * (i-_y), _w / 2);
  //  }
  //}

  for (i = 0; i < _h; i++) {
    for (j = 0; j < xSize - 1; j++) {
      drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4)>>1);
      drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff)>>1);
    }
    drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4)>>1);
    if (_rem == 0) drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff)>>1);
  }
}

//Clears contenst from display (slower, some epaper panels needs slower cleaning process from others).
void Inkplate::fillScreen(uint8_t c) {
  uint8_t data = c == 0 ? B10101010 : B01010101;
  uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
  draw_mode_on();
  SPV_SET;
  delayMicroseconds(500); //usleep(500);
  for (int k = 0; k < REF_RATE; k++) {
    begin_frame();
    for (int i = 0; i < 600; i++) {

      begin_line();
      for (int j = 0; j < 100; j++) {
        GPIO.out &= ~DATA;
        GPIO.out |= _send;
        CL_SET;
        CL_CLEAR;
        GPIO.out &= ~DATA;
        GPIO.out |= _send;
        CL_SET;
        CL_CLEAR;
      }
      end_line();
    }
    end_frame();
  }
  draw_mode_off();
}

//Clear screan before new screen update using waveform
void Inkplate::clean() {
  draw_mode_on();
  SPV_SET; //dspv_gpio::set::set(1 << DSPV_BIT);
  delayMicroseconds(500); //usleep(500);
  int m = 0;
  cleanFast(0); //white
  m++;
  for (int i = 0; i < 8; i++) {
    cleanFast((waveform[m] >> 30) & 3); //White to white
    m++;
	delay(1);
  }
  //cleanFast(0);
  cleanFast((waveform[m] >> 24) & 3); //white to black
  m++;
  for (int i = 0; i < 8; i++) {
    cleanFast((waveform[m]) & 3); //Black to black
    m++;
	delay(1);
  }
  cleanFast((waveform[m] >> 6) & 3); //Black to white
  m++;
  for (int i = 0; i < 8; i++) {
    cleanFast((waveform[m] >> 30) & 3); //White to white
    m++;
	delay(1);
  }
  //cleanFast(2);
  //delay(1);
  draw_mode_off();
}

//Clears content from epaper diplay as fast as ESP32 can.
void Inkplate::cleanFast(uint8_t c) {
  uint8_t data;
  if (c == 0) {
    data = B10101010;     //White
  } else if (c == 1) {
    data = B01010101;     //Black
  } else if (c == 2) {
    data = B00000000;     //Discharge
  }
  uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
  begin_frame();
  for (int i = 0; i < 600; i++) {

    begin_line();
    for (int j = 0; j < 100; j++) {
      GPIO.out_w1tc = DATA | CL;
      GPIO.out_w1ts = (_send) | CL;
      //GPIO.out_w1ts = CL;
      GPIO.out_w1tc = DATA | CL;
      GPIO.out_w1ts = (_send) | CL;
      //GPIO.out_w1ts = CL;
    }
    end_line();
  }
  end_frame();
}

//Turn on supply for epaper display (TPS65186) [+15 VDC, -15VDC, +22VDC, -20VDC, +3.3VDC, VCOM]
void Inkplate::einkOn() {
    _panelOn = 1;
  pinsAsOutputs();
  WAKEUP_SET;
  PWRUP_SET;
  //Enable all rails
  Wire.beginTransmission(0x48);
  Wire.write(0x01);
  Wire.write(B00111111);
  Wire.endTransmission();
  //Set out voltage on LDO outputs
  Wire.beginTransmission(0x48);
  Wire.write(0x02);
  Wire.write(B00100011);
  Wire.endTransmission();
  //Set VCOM Voltage
  Wire.beginTransmission(0x48);
  Wire.write(0x03);
  Wire.write(150);
  Wire.endTransmission();
  //Set power up times (all on 3mS)
  Wire.beginTransmission(0x48);
  Wire.write(0x0A);
  Wire.write(0);
  Wire.endTransmission();
  //Set Power Down Seq.
  Wire.beginTransmission(0x48);
  Wire.write(0x0B);
  Wire.write(B00011011);
  Wire.endTransmission();
  //Set Power Down Times (all on 6mS)
  Wire.beginTransmission(0x48);
  Wire.write(0x0C);
  Wire.write(0);
  Wire.endTransmission();
  
  delay(20);
  
  VCOM_SET;
  
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


}

//Turn off epapewr supply and put all digital IO pins in high Z state
void Inkplate::einkOff() {
  _panelOn = 0;
  GPIO.out &= ~(DATA | CL | LE);
  SPH_CLEAR;
  OE_CLEAR;
  GMOD_CLEAR;
  SPV_CLEAR;

  PWRUP_CLEAR;
  //Enable all rails
  Wire.beginTransmission(0x48);
  Wire.write(0x01);
  Wire.write(B00000000);
  Wire.endTransmission();
  //delay(250);
  WAKEUP_CLEAR;
  VCOM_CLEAR;

  pinsZstate();
}

void Inkplate::setRotation(uint8_t r) {
  _rotation = r % 4;
  switch (rotation) {
    case 0:
      _width  = E_INK_WIDTH;
      _height = E_INK_HEIGHT;
      break;
    case 1:
      _width  = E_INK_HEIGHT;
      _height = E_INK_WIDTH;
      break;
    case 2:
      _width  = E_INK_WIDTH;
      _height = E_INK_HEIGHT;
      break;
    case 3:
      _width  = E_INK_HEIGHT;
      _height = E_INK_WIDTH;
      break;
  }
}

uint8_t Inkplate::getPanelState() {
  return _panelOn;
}

void Inkplate::pinsZstate() {
  //CONTROL PINS
  pinMode(0, INPUT);
  pinMode(2, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  mcp.pinMode(OE, INPUT);
  mcp.pinMode(GMOD, INPUT);
  mcp.pinMode(SPV, INPUT);

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

void Inkplate::pinsAsOutputs() {
  //CONTROL PINS
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  mcp.pinMode(OE, OUTPUT);
  mcp.pinMode(GMOD, OUTPUT);
  mcp.pinMode(SPV, OUTPUT);

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

void Inkplate::selectDisplayMode(uint8_t _mode) {
  if (_mode == INKPLATE_1BIT) {
	if (D_memory_new != NULL) return;
    if (D_memory4Bit != NULL) free(D_memory4Bit);
    //1 bit per pixel mode (monochrome mode)
    D_memory_new = (uint8_t*)ps_malloc(600 * 100);
	_partial = (uint8_t*)ps_malloc(600 * 100);
	_pBuffer = (uint8_t*) ps_malloc(120000);
    if (D_memory_new == NULL || _partial == NULL || _pBuffer == NULL) {
      do {
        delay(100);
      } while (true);
    }
    memset(D_memory_new, 0, 60000);
    _displayMode = INKPLATE_1BIT;
  }

  if (_mode == INKPLATE_3BIT) {
	if (D_memory4Bit != NULL) return;
    if (D_memory_new != NULL) {
		free(D_memory_new);
		free(_partial);
		free(_pBuffer);
	}
    //3 bit per pixel mode (8 level grayscale mode)
    D_memory4Bit = (uint8_t*)ps_malloc(240000);
    if (D_memory4Bit == NULL ) {
      do {
        delay(100);
      } while (true);
    }
    memset(D_memory4Bit, 255, 240000);
    _displayMode = INKPLATE_3BIT;
  }
}

uint8_t Inkplate::getDisplayMode() {
  return _displayMode;
}

uint8_t Inkplate::readTouchpad(uint8_t _pad) {
  return mcp.digitalRead((_pad&3)+10);
}

int8_t Inkplate::readTemperature() {
  return _temperature;
}

double Inkplate::readBattery() {
  mcp.digitalWrite(9, HIGH);
  delay(1);
  int adc = analogRead(35);
  mcp.digitalWrite(9, LOW);
  return (double(adc) / 4095 * 3.3 * 2);
}