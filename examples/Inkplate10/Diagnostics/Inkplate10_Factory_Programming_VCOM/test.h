#ifndef __TEST_H__
#define __TEST_H__

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.h
#include "Inkplate.h"
#include <Arduino.h>

// WiFi timeout in seconds.
#define WTIMEOUT 10
=======
#include <Arduino.h>
#include "Inkplate.h"

// WiFi timeout in seconds.
#define WTIMEOUT  10
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.h

// Timeoit for detecting touchpads in seconds (only on old Inkplates)
#define TOUCHPADS_TIMEOUT 10

// Get Inkplate object from the main file.
extern Inkplate display;

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.h
// By default, test both I/O expanders.
void testPeripheral();
int checkI2C(int address);
=======
void testPeripheral(uint8_t _skipSecondIO = 0);
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.h
double getVCOMFromSerial(double *_vcom);
int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout);
int checkMicroSDCard();
int rtcCheck();
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.h
int checkBatteryAndTemp(float *temp, float *batVoltage);
int touchPads(uint8_t _timeoutTouchpads);
void failHandler(bool printErrorOnSerial = false);
=======
int touchPads(uint8_t _timeoutTouchpads);
void failHandler();
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.h

#endif
