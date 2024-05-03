#ifndef __TEST_H__
#define __TEST_H__

#include <Arduino.h>
#include "Inkplate.h"

// WiFi timeout in seconds.
#define WTIMEOUT  10

// Timeoit for detecting touchpads in seconds (only on old Inkplates)
#define TOUCHPADS_TIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

// By default, test both I/O expanders.
void testPeripheral();
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
int checkI2C(int address);
int checkTemp(float *temp);
int touchPads(uint8_t _timeoutTouchpads);
int checkTouch(uint8_t _tsTimeout);
int checkBME(float * bmeTemp, float * bmeHumidity, float * bmePres);
int checkFuelGauge(int *_soc, int *_volts);
int checkGestureSensor(int _gestTimeout, String * gesture);
int checkGyroscope(float * acX, float * acY, float * acZ);
void checkBuzzer();
void failHandler(bool printErrorOnSerial = false);
void IRAM_ATTR ISR();

#endif
