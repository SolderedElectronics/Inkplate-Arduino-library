/**
 **************************************************
 *
 * @file        APDS9960-SOLDERED.h
 * @brief       Header file for APDS9960 driver.
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Zvonimir Haramustek for soldered.com
 ***************************************************/

#ifndef __APDS9960_SOLDERED__
#define __APDS9960_SOLDERED__

#include "Arduino.h"
#include "Wire.h"
#include "libs/SparkFun_APDS9960/src/SparkFun_APDS9960.h"


class APDS_9960 : public SparkFun_APDS9960
{
  public:
    APDS_9960() : SparkFun_APDS9960(){};
};

#endif
