#ifndef __TEST_H__
#define __TEST_H__

#include <Arduino.h>
#include "Inkplate.h"

// WiFi timeout in seconds.
#define WTIMEOUT  10

// Timeout for detecting touchscreen in seconds
#define TOUCHSCREEN_TIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

void testPeripheral(uint8_t _skipSecondIO = 0);
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
int checkTouch(uint8_t _tsTimeout);
void failHandler();

#endif
