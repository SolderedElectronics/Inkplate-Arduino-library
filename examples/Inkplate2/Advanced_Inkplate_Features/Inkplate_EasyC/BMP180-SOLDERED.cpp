/**
 **************************************************
 *
 * @file        BMP180-SOLDERED.cpp
 * @brief       Arduino library wraper for BMP180 Temperature and Pressure Sensor.
 *
 * @copyright   GNU General Public License v3.0
 * @authors     @ soldered.com, Robert Peric
 ***************************************************/


#include "BMP180-SOLDERED.h"


/**
 * @brief                   Overloaded function for virtual in base class to initialize sensor specific.
 */
void Bmp_180::initializeNative()
{
    pinMode(pin, INPUT);
}
