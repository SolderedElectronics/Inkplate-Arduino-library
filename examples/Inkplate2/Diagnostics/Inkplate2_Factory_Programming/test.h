#ifndef __TEST_H__
#define __TEST_H__

#include "Arduino.h"
#include "Inkplate.h"

// WiFi timeout in seconds
#define WTIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

// Functions for testing

// Timeout for the operator's answer on visual/audible checks, in seconds.
#define VISUAL_CHECK_TIMEOUT 30

// Serial test reporting helpers.
void testResult(const char *_testName, bool _passed);
int askOperator(const char *_testName, const char *_question, uint8_t _timeout = VISUAL_CHECK_TIMEOUT);

void testPeripheral();
int checkI2C(int address);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
void failHandler();

#endif
