/**
 **************************************************
 *
 * @file        BMP180-SOLDERED.h
 * @brief       Header file for BMP180 temperature and pressure sensor.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     @ soldered.com, Robert Peric
 ***************************************************/

#ifndef _BMP_180_H
#define _BMP_180_H

#include "Arduino.h"

#include "libs/BMP180_Breakout_Arduino_Library/src/SFE_BMP180.h"

class Bmp_180 : public SFE_BMP180
{
  public:
  protected:
    void initializeNative();

  private:
    int pin;
};

#endif
