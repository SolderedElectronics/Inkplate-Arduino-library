#ifndef __TEST_H__
#define __TEST_H__

#include "Inkplate.h"
#include <Arduino.h>

// WiFi timeout in seconds.
#define WTIMEOUT 10

// Timeoit for detecting touchpads in seconds (only on old Inkplates)
#define TOUCHPADS_TIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

// By default, test both I/O expanders.
void testPeripheral();
int checkI2C(int address);
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
int checkBatteryAndTemp(float *temp, float *batVoltage);
int touchPads(uint8_t _timeoutTouchpads);
void failHandler(bool printErrorOnSerial = false);

#endif
