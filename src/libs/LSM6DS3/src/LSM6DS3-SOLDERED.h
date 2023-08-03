/**
 **************************************************
 *
 * @file        LSM6DS3-SOLDERED.h
 * @brief       Functions for working with LSM6DS3.
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Karlo Leksic for soldered.com
 ***************************************************/

#ifndef __LSM6DS3__
#define __LSM6DS3__

#include "Arduino.h"
#include "libs/SparkFun_LSM6DS3_Arduino_Library/src/SparkFunLSM6DS3.h"

class Soldered_LSM6DS3 : public LSM6DS3
{
  public:
    Soldered_LSM6DS3(uint8_t inputArg = 0x6B) : LSM6DS3(I2C_MODE, inputArg)
    {
    }
};

#endif
