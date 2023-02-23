#ifndef __TEST_H__
#define __TEST_H__

#include "Arduino.h"
#include "Inkplate.h"

// WiFi timeout in seconds
#define WTIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

// Functions for testing
void testPeripheral();
int checkI2C(int address);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
void failHandler();

#endif
