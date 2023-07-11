/**
 **************************************************
 *
 * @file        BME280-SOLDERED.cpp
 * @brief       Simplified BME280 board for soldered board
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/

#include "BME280-SOLDERED.h"


/**
 * @brief  BME280_Soldered begin method, should be called before using the class
 */
void BME280::begin()
{
    for (int i = 0; i < 10 && !BME280_Class::begin(); ++i)
        delay(1000);

    mode(NormalMode);

    // Usually default settings are fine.
    BME280_Class::setOversampling(TemperatureSensor, Oversample16); // Use enumerated type values
    BME280_Class::setOversampling(HumiditySensor, Oversample16);    // Use enumerated type values
    BME280_Class::setOversampling(PressureSensor, Oversample16);    // Use enumerated type values

    BME280_Class::iirFilter(IIR16); // Use enumerated type values
    BME280_Class::inactiveTime(inactive1000ms);
}

/**
 * @brief         BME280 temperature method
 *
 * @returns float Tempearture in degree C
 */
float BME280::readTemperature()
{
    int32_t temp, humidity, pressure;
    BME280_Class::getSensorData(temp, humidity, pressure);

    return temp / 100.0;
}

/**
 * @brief         BME280 Pressure method
 *
 * @returns float Pressure in hPa
 */
float BME280::readPressure()
{
    int32_t temp, humidity, pressure;
    BME280_Class::getSensorData(temp, humidity, pressure);

    return pressure / 100.0;
}

/**
 * @brief         BME280 Humidity method
 *
 * @returns float Humidity in %
 */
float BME280::readHumidity()
{
    int32_t temp, humidity, pressure;
    BME280_Class::getSensorData(temp, humidity, pressure);

    return humidity / 100.0;
}

/**
 * @brief         BME280 Altiude method
 *
 * @returns float Altitude in m
 */
float BME280::readAltitude()
{
    int32_t temp, humidity, pressure;
    BME280_Class::getSensorData(temp, humidity, pressure);

    float seaLevel = 1013.25;
    return 44330.0 * (1.0 - pow(((float)pressure / 100.0) / seaLevel, 0.1903));
}

/**
 * @brief                    BME280 all sensor readings at once
 *
 * @param float& temp        Temperature in degree C
 * @param float& humidity    Humidity in %
 * @param float& pressure    Pressure in Pa
 */
void BME280::readSensorData(float &temp, float &humidity, float &pressure)
{
    int32_t _temp, _humidity, _pressure, _gas;
    BME280_Class::getSensorData(_temp, _humidity, _pressure);

    temp = _temp / 100.0;
    humidity = _humidity / 100.0;
    pressure = _pressure / 100.0;
}

/**
 * @brief                BME280 Altitude method caluclation
 *
 * @param float pressure Pressure in Pa
 *
 * @returns float        Altiude in m
 */
float BME280::calculateAltitude(float pressure)
{
    float seaLevel = 1013.25;
    return 44330.0 * (1.0 - pow(pressure / seaLevel, 0.1903));
}
