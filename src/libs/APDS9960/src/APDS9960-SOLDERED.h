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
#include "libs/Arduino_APDS9960/src/Arduino_APDS9960.h"


class APDS_9960 : public APDS9960
{
  public:
    APDS_9960(int _int) : APDS9960(Wire, _int){};
    APDS_9960() : APDS9960(Wire, -1){};
};

#undef APDS9960

#endif
