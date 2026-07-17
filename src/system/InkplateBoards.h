/**
 **************************************************
 *
 * @file        InkplateBoards.h
 * @brief       File used for the Inkplate board selection. It selects
 *              correct Inkplate Driver code as well as correct peripherals,
 *              defines etc.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#ifndef __INKPLATE_BOARDS_H__
#define __INKPLATE_BOARDS_H__

// Board selector. It only includes files for selected board.
#if defined(ARDUINO_INKPLATE13SPECTRA)
#include "../boards/Inkplate13SPECTRA/Inkplate13SPECTRADriver.h"
#elif defined(ARDUINO_ESP32S3_DEV)
#include "../boards/Inkplate7SPECTRA/Inkplate7SPECTRADriver.h"
#elif defined(ARDUINO_INKPLATE10V2) || defined(ARDUINO_INKPLATE10)
#include "../boards/Inkplate10/Inkplate10Driver.h"
#elif defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
#include "../boards/Inkplate6PLUS/Inkplate6PLUSDriver.h"
#elif defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE6)
#include "../boards/Inkplate6/Inkplate6Driver.h"
#elif defined(ARDUINO_INKPLATE5V2)
#include "../boards/Inkplate5V2/Inkplate5V2Driver.h"
#elif defined(ARDUINO_INKPLATE5)
#include "../boards/Inkplate5/Inkplate5Driver.h"
#elif defined(ARDUINO_INKPLATE4TEMPERA)
#include "../boards/Inkplate4TEMPERA/Inkplate4TEMPERADriver.h"
#elif defined(ARDUINO_INKPLATE6FLICK)
#include "boards/Inkplate6FLICK/Inkplate6FLICKDriver.h"
#elif defined(ARDUINO_INKPLATECOLOR)
#include "boards/Inkplate6COLOR/Inkplate6COLORDriver.h"
#elif defined(ARDUINO_INKPLATE2)
#include "boards/Inkplate2/Inkplate2Driver.h"
#endif
#endif