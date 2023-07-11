/**
 **************************************************
 *
 * @file        BME680-SOLDERED.h
 * @brief       Header file for BME680-SOLDERED board
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/

#ifndef __BME680_SOLDERED__
#define __BME680_SOLDERED__

#include "Arduino.h"
#include "libs/BME680/src/Zanshin_BME680.h"

class BME680 : public BME680_Class
{
  public:
    bool begin();

    float readTemperature();
    float readPressure();
    float readHumidity();
    float readAltitude();
    float readGasResistance();
    void readSensorData(float &temp, float &humidity, float &pressure, float &gas);

    float calculateAltitude(float pressure);

  protected:
    void initializeNative(){};

  private:
};

#endif
