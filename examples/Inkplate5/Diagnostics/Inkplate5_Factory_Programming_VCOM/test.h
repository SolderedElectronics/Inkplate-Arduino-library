#ifndef __TEST_H__
#define __TEST_H__

#include "Arduino.h"
#include "Inkplate.h"

// WiFi timeout in seconds.
#define WTIMEOUT 10

// Timeoit for detecting touchpads in seconds (only on old Inkplates)
#define TOUCHPADS_TIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;


// Timeout for the operator's answer on visual/audible checks, in seconds.
#define VISUAL_CHECK_TIMEOUT 30

// Serial test reporting helpers.
void testResult(const char *_testName, bool _passed);
int askOperator(const char *_testName, const char *_question, uint8_t _timeout = VISUAL_CHECK_TIMEOUT);

void testPeripheral();
int checkScreenBorder();
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
int checkI2C(int address);
int checkBatteryAndTemp(float *temp, float *batVoltage);
void failHandler(bool printErrorOnSerial = false);

#endif
