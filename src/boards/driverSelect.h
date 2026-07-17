/**
 **************************************************
 *
 * @file        driverSelect.h
 * @brief       Main header file of the Inkpate board driver selector used to
 *              include proper files for the selected Inkplate board.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#ifndef __BOARD_SELECT_H__
#define __BOARD_SELECT_H__

#include <Arduino.h>
#if defined(ARDUINO_INKPLATE13SPECTRA)
#define USE_COLOR_IMAGE
#include "boards/Inkplate13SPECTRA/Inkplate13SPECTRADriver.h"
#elif defined(ARDUINO_ESP32S3_DEV)
#define USE_COLOR_IMAGE
#include "boards/Inkplate7SPECTRA/Inkplate7SPECTRADriver.h"
#elif defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE10)
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate10/Inkplate10Driver.h"
#elif defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate6PLUS/Inkplate6PLUSDriver.h"
#elif defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE6)
#define USES_I2S
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate6/Inkplate6Driver.h"
#elif defined(ARDUINO_INKPLATE5V2)
#define MULTIPLE_DISPLAY_MODES
#define USES_I2S
#include "boards/Inkplate5V2/Inkplate5V2Driver.h"
#elif defined(ARDUINO_INKPLATE5)
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate5/Inkplate5Driver.h"
#elif defined(ARDUINO_INKPLATE4TEMPERA)
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate4TEMPERA/Inkplate4TEMPERADriver.h"
#elif defined(ARDUINO_INKPLATE6FLICK)
#define USES_I2S
#define MULTIPLE_DISPLAY_MODES
#include "boards/Inkplate6FLICK/Inkplate6FLICKDriver.h"
#elif defined(ARDUINO_INKPLATECOLOR)
#define USE_COLOR_IMAGE
#include "boards/Inkplate6COLOR/Inkplate6COLORDriver.h"
#elif defined(ARDUINO_INKPLATE2)
#define USE_COLOR_IMAGE
#include "boards/Inkplate2/Inkplate2Driver.h"
#else
#error "Board not selected!"
#endif

#endif