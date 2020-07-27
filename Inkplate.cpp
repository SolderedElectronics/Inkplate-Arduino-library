#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "Inkplate.h"
Adafruit_MCP23017 mcp;
SPIClass spi2(HSPI);
SdFat sd(&spi2);

//--------------------------STATIC FUNCTIONS--------------------------------------------
//For precise 1uS timing, we cannot use delayMicroseconds(), instead we use ASM with nop command. Initial Z value will be difeerent on different CPU speeds! (for 240 MHz CPU Clock z = 25)
void usleep1() {
  int z = 25;
  while (z--) {
    asm("NOP");
  };
}

void ckvClock() {
  CKV_CLEAR;
  usleep1();
  CKV_SET;
  usleep1();
}

//--------------------------USER FUNCTIONS--------------------------------------------
Inkplate::Inkplate(uint8_t _mode) : Adafruit_GFX(E_INK_WIDTH, E_INK_HEIGHT) {
  _displayMode = _mode;
}

void Inkplate::begin(void) {
  if(_beginDone == 1) return;
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
  //pinMode(SPV, OUTPUT);

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
  
  D_memory_new = (uint8_t*)ps_malloc(600 * 100);
  _partial = (uint8_t*)ps_malloc(600*100);
  _pBuffer = (uint8_t*) ps_malloc(120000);
  D_memory4Bit = (uint8_t*)ps_malloc(240000);
  if (D_memory_new == NULL || _partial == NULL || _pBuffer == NULL || D_memory4Bit == NULL) {
    do {
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
    uint8_t temp = *(_partial + 100 * y0 + x); //D_memory_new[99 * y0 + x];
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

void Inkplate::clearDisplay() {
  //Clear 1 bit per pixel display buffer
  if (_displayMode == 0) memset(_partial, 0, 60000);

  //Clear 3 bit per pixel display buffer
  if (_displayMode == 1) memset(D_memory4Bit, 255, 240000);
}

//Function that displays content from RAM to screen
void Inkplate::display() {
  if (_displayMode == 0) display1b();
  if (_displayMode == 1) display3b();
}

void Inkplate::partialUpdate() {
  if(_displayMode == 1) return;
  if(_blockPartial == 1) display1b();
  uint16_t _pos = 59999;
  uint32_t _send;
  uint8_t data;
  uint8_t diffw, diffb;
  uint32_t n = 119999;
  uint8_t dram;
  
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
   
  einkOn();
  for (int k = 0; k < 5; k++) {
    vscan_start();
    n = 119999;
    for (int i = 0; i < 600; i++) {
		data = *(_pBuffer + n);
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
		hscan_start(_send);
		n--;
      for (int j = 0; j < 199; j++) {
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
  for(int i = 0; i<60000; i++) {
	  *(D_memory_new+i) &= *(_partial+i);
	  *(D_memory_new+i) |= (*(_partial+i));
  }
}

void Inkplate::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char* _p, int16_t _w, int16_t _h) {
  if (_displayMode != INKPLATE_3BIT) return;
  uint8_t  _rem = _w % 2;
  int i, j;
  int xSize = _w / 2 + _rem;

  for (i = 0; i < _h; i++) {
    for (j = 0; j < xSize - 1; j++) {
      drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4)>>1);
      drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff)>>1);
    }
    drawPixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4)>>1);
    if (_rem == 0) drawPixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff)>>1);
  }
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

//Turn off epapewr supply and put all digital IO pins in high Z state
void Inkplate::einkOff() {
  if(_panelOn == 0) return;
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
void Inkplate::einkOn() {
  if(_panelOn == 1) return;
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

  LE_CLEAR; //setpin_le(FALSE);
  OE_CLEAR; //setpin_oe(FALSE);
  CL_CLEAR;   //setpin_cl(FALSE);
  SPH_SET;   //setpin_sph(FALSE);
  GMOD_SET;   //setpin_gmode(FALSE);
  SPV_SET;   //setpin_spv(FALSE);
  CKV_CLEAR;   //setpin_ckv(FALSE);
  OE_SET;
}

void Inkplate::selectDisplayMode(uint8_t _mode) {
	if(_mode != _displayMode) {
		_displayMode = _mode&1;
		memset(D_memory_new, 0, 60000);
		memset(_partial, 0, 60000);
		memset(_pBuffer, 0, 120000);
		memset(D_memory4Bit, 255, 240000);
		_blockPartial = 1;
	}
}

uint8_t Inkplate::getDisplayMode() {
  return _displayMode;
}

int Inkplate::drawBitmapFromSD(SdFile* p, int x, int y) {
	if(sdCardOk == 0) return 0;
	struct bitmapHeader bmpHeader;
	readBmpHeaderSd(p, &bmpHeader);
	if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 || !(bmpHeader.color == 1 || bmpHeader.color == 24)) return 0;

	if ((bmpHeader.color == 24 || bmpHeader.color == 32) && getDisplayMode() != INKPLATE_3BIT) {
		selectDisplayMode(INKPLATE_3BIT);
	}

	if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT) {
		selectDisplayMode(INKPLATE_1BIT);
	}
  
	if (bmpHeader.color == 1) drawMonochromeBitmapSd(p, bmpHeader, x, y);
	if (bmpHeader.color == 24) drawGrayscaleBitmap24Sd(p, bmpHeader, x, y);

  return 1;
}

int Inkplate::drawBitmapFromSD(char* fileName, int x, int y) {
  if(sdCardOk == 0) return 0;
  SdFile dat;
  if (dat.open(fileName, O_RDONLY)) {
    return drawBitmapFromSD(&dat, x, y);
  } else {
    return 0;
  }
}

int Inkplate::drawBitmapFromWeb(WiFiClient* s, int x, int y, int len, bool invert) {
  struct bitmapHeader bmpHeader;
  readBmpHeaderWeb(s, &bmpHeader);
  if (bmpHeader.signature != 0x4D42 || bmpHeader.compression != 0 || !(bmpHeader.color == 1 || bmpHeader.color == 24)) return 0;

  if ((bmpHeader.color == 24 || bmpHeader.color == 32) && getDisplayMode() != INKPLATE_3BIT) {
    selectDisplayMode(INKPLATE_3BIT);
  }

  if (bmpHeader.color == 1 && getDisplayMode() != INKPLATE_1BIT) {
    selectDisplayMode(INKPLATE_1BIT);
  }
  
  if (bmpHeader.color == 1) drawMonochromeBitmapWeb(s, bmpHeader, x, y, len, invert);
  if (bmpHeader.color == 24) drawGrayscaleBitmap24Web(s, bmpHeader, x, y, len, invert);

  return 1;
}

int Inkplate::drawBitmapFromWeb(char* url, int x, int y, bool invert) {
  if (WiFi.status() != WL_CONNECTED) return 0;
  int ret = 0;

  bool sleep = WiFi.getSleep();
  WiFi.setSleep(false);

  HTTPClient http;
  http.getStream().setNoDelay(true);
  http.getStream().setTimeout(1);
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode == 200) {
    int32_t len = http.getSize();
    if (len > 0) {
      WiFiClient * dat = http.getStreamPtr();
      ret = drawBitmapFromWeb(dat, x, y, len, invert);
    }
  }
  
  http.end();
  WiFi.setSleep(sleep);
  return ret;
}

int Inkplate::sdCardInit() {
	spi2.begin(14, 12, 13, 15);
	sdCardOk = sd.begin(15, SD_SCK_MHZ(25));
	return sdCardOk;
}

SdFat Inkplate::getSdFat() {
	return sd;
}

SPIClass Inkplate::getSPI() {
	return spi2;
}

uint8_t Inkplate::getPanelState() {
  return _panelOn;
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

void Inkplate::vscan_end() {
  CKV_CLEAR;
  LE_SET;
  LE_CLEAR;
  delayMicroseconds(1);
  CKV_SET;
}

//Clear screan before new screen update using waveform
void Inkplate::clean() {
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
void Inkplate::cleanFast(uint8_t c, uint8_t rep) {
  einkOn();
  uint8_t data;
  if (c == 0) {
    data = B10101010;     //White
  } else if (c == 1) {
    data = B01010101;     //Black
  } else if (c == 2) {
    data = B00000000;     //Discharge
  } else if (c == 3) {
	data = B11111111;	  //Skip
  }

  uint32_t _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
  for (int i = 0; i < rep; i++) {
    unsigned int x, y;
    vscan_start();
    for (y = 0; y < 600; y++) {
	  hscan_start(_send);
      for (x = 0; x < 100; x++) {
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

void Inkplate::cleanFast2(uint8_t c, uint8_t n, uint16_t d) {
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

//--------------------------PRIVATE FUNCTIONS--------------------------------------------
//Display content from RAM to display (1 bit per pixel,. monochrome picture).
void Inkplate::display1b() {
  for(int i = 0; i<60000; i++) {
	  *(D_memory_new+i) &= *(_partial+i);
	  *(D_memory_new+i) |= (*(_partial+i));
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
  for (int k = 0; k < 5; k++) {
	_pos = 59999;
    vscan_start();
    for (int i = 0; i < 600; i++) {
	  dram = *(D_memory_new + _pos);
      data = LUTB[(dram >> 4)&0x0F];
      _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
	  hscan_start(_send);
	  data = LUTB[dram & 0x0F];
      _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
	  GPIO.out_w1ts = (_send) | CL;
      GPIO.out_w1tc = DATA | CL;
	  _pos--;
      for (int j = 0; j < 99; j++) {
		dram = *(D_memory_new + _pos);
        data = LUTB[(dram >> 4)&0x0F];
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
    for (int i = 0; i < 600; i++) {
	  dram = *(D_memory_new + _pos);
      data = LUT2[(dram >> 4)&0x0F];
      _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
	  hscan_start(_send);
	  data = LUT2[dram & 0x0F];
      _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
	  GPIO.out_w1ts = (_send) | CL;
      GPIO.out_w1tc = DATA | CL;
	  _pos--;
      for (int j = 0; j < 99; j++) {
		dram = *(D_memory_new + _pos);
        data = LUT2[(dram >> 4)&0x0F];
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
  /*
  for (int k = 0; k < 1; k++) {
    vscan_start();
    hscan_start();
	_pos = 59999;
    for (int i = 0; i < 600; i++) {
      for (int j = 0; j < 100; j++) {
        data = discharge[(*(D_memory_new + _pos) >> 4)];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
        GPIO.out_w1tc = DATA | CL;
        GPIO.out_w1ts = (_send) | CL;
        data = discharge[*(D_memory_new + _pos) & 0x0F];
        _send = ((data & B00000011) << 4) | (((data & B00001100) >> 2) << 18) | (((data & B00010000) >> 4) << 23) | (((data & B11100000) >> 5) << 25);
        GPIO.out_w1tc = DATA | CL;
        GPIO.out_w1ts = (_send) | CL;
		_pos--;
      }
      vscan_write();
    }
    CKV_CLEAR;
    delayMicroseconds(230);
  }
  */
  cleanFast(2, 2);
  cleanFast(3, 1);
  vscan_start();
  einkOff();
  _blockPartial = 0;
}

//Display content from RAM to display (3 bit per pixel,. 8 level of grayscale, STILL IN PROGRESSS, we need correct wavefrom to get good picture, use it only for pictures not for GFX).
void Inkplate::display3b() {
  einkOn();
  cleanFast(0, 1);
  cleanFast(1, 12);
  cleanFast(2, 1);
  cleanFast(0, 11);
  cleanFast(2, 1);
  cleanFast(1, 12);
  cleanFast(2, 1);
  cleanFast(0, 11);
  
  for (int k = 0; k < 7; k++) {
      uint8_t *dp = D_memory4Bit + 239999;
      uint32_t _send;
      uint8_t pix1;
      uint8_t pix2;
      uint8_t pix3;
      uint8_t pix4;
	  uint8_t pixel;
      uint8_t pixel2;
	  
      vscan_start();
      for (int i = 0; i < 600; i++) {
		pixel = 0B00000000;
        pixel2 = 0B00000000;
		pix1 = *(dp--);
        pix2 = *(dp--);
		pix3 = *(dp--);
        pix4 = *(dp--);
		pixel |= ( waveform3Bit[pix1&0x07][k] << 6) | ( waveform3Bit[(pix1>>4)&0x07][k] << 4) | ( waveform3Bit[pix2&0x07][k] << 2) | ( waveform3Bit[(pix2>>4)&0x07][k] << 0);
		pixel2 |= ( waveform3Bit[pix3&0x07][k] << 6) | ( waveform3Bit[(pix3>>4)&0x07][k] << 4) | ( waveform3Bit[pix4&0x07][k] << 2) | ( waveform3Bit[(pix4>>4)&0x07][k] << 0);

        _send = ((pixel & B00000011) << 4) | (((pixel & B00001100) >> 2) << 18) | (((pixel & B00010000) >> 4) << 23) | (((pixel & B11100000) >> 5) << 25);
		hscan_start(_send);
        _send = ((pixel2 & B00000011) << 4) | (((pixel2 & B00001100) >> 2) << 18) | (((pixel2 & B00010000) >> 4) << 23) | (((pixel2 & B11100000) >> 5) << 25);
        GPIO.out_w1ts = (_send) | CL;
        GPIO.out_w1tc = DATA | CL;
		
        for (int j = 0; j < 99; j++) {

          pixel = 0B00000000;
          pixel2 = 0B00000000;
		  pix1 = *(dp--);
          pix2 = *(dp--);
		  pix3 = *(dp--);
          pix4 = *(dp--);
		  pixel |= ( waveform3Bit[pix1&0x07][k] << 6) | ( waveform3Bit[(pix1>>4)&0x07][k] << 4) | ( waveform3Bit[pix2&0x07][k] << 2) | ( waveform3Bit[(pix2>>4)&0x07][k] << 0);
		  pixel2 |= ( waveform3Bit[pix3&0x07][k] << 6) | ( waveform3Bit[(pix3>>4)&0x07][k] << 4) | ( waveform3Bit[pix4&0x07][k] << 2) | ( waveform3Bit[(pix4>>4)&0x07][k] << 0);

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

uint32_t Inkplate::read32(uint8_t* c) {
  return (*(c) | (*(c + 1) << 8) | (*(c + 2) << 16) | (*(c + 3) << 24));
}

uint16_t Inkplate::read16(uint8_t* c) {
  return (*(c) | (*(c + 1) << 8));
}

void Inkplate::readBmpHeaderSd(SdFile *_f, struct bitmapHeader *_h) {
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

void Inkplate::readBmpHeaderWeb(WiFiClient *_s, struct bitmapHeader *_h) {
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

int Inkplate::drawMonochromeBitmapSd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y) {
  int w = bmpHeader.width;
  int h = bmpHeader.height;
  uint8_t paddingBits = w % 32;
  w /= 32;

  f->seekSet(bmpHeader.startRAW);
  int i, j;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      uint32_t pixelRow = f->read() << 24 | f->read() << 16 | f->read() << 8 | f->read();
      for (int n = 0; n < 32; n++) {
        drawPixel((i * 32) + n + x, h - j + y, !(pixelRow & (1ULL << (31 - n))));
      }
    }
    if (paddingBits) {
      uint32_t pixelRow = f->read() << 24 | f->read() << 16 | f->read() << 8 | f->read();
      for (int n = 0; n < paddingBits; n++) {
        drawPixel((i * 32) + n + x, h - j + y, !(pixelRow & (1ULL << (31 - n))));
      }
    }
  }
  f->close();
  return 1;
}

int Inkplate::drawGrayscaleBitmap24Sd(SdFile *f, struct bitmapHeader bmpHeader, int x, int y) {
  int w = bmpHeader.width;
  int h = bmpHeader.height;
  char padding = w % 4;
  f->seekSet(bmpHeader.startRAW);
  int i, j;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      //This is the proper way of converting True Color (24 Bit RGB) bitmap file into grayscale, but it takes waaay too much time (full size picture takes about 17s to decode!)
      //float px = (0.2126 * (readByteFromSD(&file) / 255.0)) + (0.7152 * (readByteFromSD(&file) / 255.0)) + (0.0722 * (readByteFromSD(&file) / 255.0));
      //px = pow(px, 1.5);
      //display.drawPixel(i + x, h - j + y, (uint8_t)(px*7));

      //So then, we are convertng it to grayscale using good old average and gamma correction (from LUT). With this metod, it is still slow (full size image takes 4 seconds), but much beter than prev mentioned method.
      uint8_t px = (f->read() * 2126 / 10000) + (f->read() * 7152 / 10000) + (f->read() * 722 / 10000);
      //drawPixel(i + x, h - j + y, gammaLUT[px]);
	  drawPixel(i + x, h - j + y, px>>5);
	  //drawPixel(i + x, h - j + y, px/32);
    }
    if (padding) {
      for (int p = 0; p < padding; p++) {
        f->read();
      }
    }
  }
  f->close();
  return 1;
}

int Inkplate::drawMonochromeBitmapWeb(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert) {
  int w = bmpHeader.width;
  int h = bmpHeader.height;
  uint8_t paddingBits = w % 32;
  int total = len - 34;
  w /= 32;

  uint8_t* buf = (uint8_t*) ps_malloc(total);
  if (buf == NULL)
    return 0;

  int pnt = 0;
  while (pnt < total) {
    int toread = s->available();
    if (toread > 0) {
      int read = s->read(buf+pnt, toread);
      if (read > 0)
        pnt += read;
    }
  }

  int i, j, k = bmpHeader.startRAW - 34;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      uint8_t a = buf[k++];
      uint8_t b = buf[k++];
      uint8_t c = buf[k++];
      uint8_t d = buf[k++];
      uint32_t pixelRow = a << 24 | b << 16 | c << 8 | d;
      if (invert)
        pixelRow = ~pixelRow;
      for (int n = 0; n < 32; n++) {
        drawPixel((i * 32) + n + x, h - j + y, !(pixelRow & (1ULL << (31 - n))));
      }
    }
    if (paddingBits) {
      uint8_t a = buf[k++];
      uint8_t b = buf[k++];
      uint8_t c = buf[k++];
      uint8_t d = buf[k++];
      uint32_t pixelRow = a << 24 | b << 16 | c << 8 | d;
      if (invert)
        pixelRow = ~pixelRow;
      for (int n = 0; n < paddingBits; n++) {
        drawPixel((i * 32) + n + x, h - j + y, !(pixelRow & (1ULL << (31 - n))));
      }
    }
  }

  free(buf);

  return 1;
}

int Inkplate::drawGrayscaleBitmap24Web(WiFiClient *s, struct bitmapHeader bmpHeader, int x, int y, int len, bool invert) {
  int w = bmpHeader.width;
  int h = bmpHeader.height;
  char padding = w % 4;
  int total = len - 34;

  uint8_t* buf = (uint8_t*) ps_malloc(total);
  if (buf == NULL)
    return 0;

  int pnt = 0;
  while (pnt < total) {
    int toread = s->available();
    if (toread > 0) {
      int read = s->read(buf+pnt, toread);
      if (read > 0)
        pnt += read;
    }
  }

  int i, j, k = bmpHeader.startRAW - 34;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      uint8_t r = buf[k++];
      uint8_t g = buf[k++];
      uint8_t b = buf[k++];
      if (invert) {
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
      }
      //This is the proper way of converting True Color (24 Bit RGB) bitmap file into grayscale, but it takes waaay too much time (full size picture takes about 17s to decode!)
      //float px = (0.2126 * (readByteFromSD(&file) / 255.0)) + (0.7152 * (readByteFromSD(&file) / 255.0)) + (0.0722 * (readByteFromSD(&file) / 255.0));
      //px = pow(px, 1.5);
      //display.drawPixel(i + x, h - j + y, (uint8_t)(px*7));

      //So then, we are convertng it to grayscale using good old average and gamma correction (from LUT). With this metod, it is still slow (full size image takes 4 seconds), but much beter than prev mentioned method.
      uint8_t px = (r * 2126 / 10000) + (g * 7152 / 10000) + (b * 722 / 10000);
      //drawPixel(i + x, h - j + y, gammaLUT[px]);
    drawPixel(i + x, h - j + y, px>>5);
    //drawPixel(i + x, h - j + y, px/32);
    }
    if (padding) {
      for (int p = 0; p < padding; p++) {
        k++;
      }
    }
  }

  free(buf);

  return 1;
}

void Inkplate::precalculateGamma(uint8_t* c, float gamma) {
  for (int i = 0; i < 256; i++) {
    c[i] = int(round((pow(i / 255.0, gamma)) * 15));
  }
}

