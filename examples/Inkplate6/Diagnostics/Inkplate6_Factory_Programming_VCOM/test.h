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
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/test.h
void testPeripheral();
void checkScreenBorder();
=======
void testPeripheral(uint8_t _oldInkplate = 0);
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/test.h
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/test.h
int checkI2C(int address);
int checkBatteryAndTemp(float *temp, float *batVoltage);
int touchPads(uint8_t _timeoutTouchpads);
void failHandler(bool printErrorOnSerial = false);
=======
int touchPads(uint8_t _timeoutTouchpads);
void failHandler();
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/test.h

#endif
