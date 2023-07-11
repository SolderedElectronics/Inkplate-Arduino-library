/**
 **************************************************
 *
 * @file        BME680-SOLDERED.cpp
 * @brief       Simplified BME680 board for soldered board
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/


#include "BME680-SOLDERED.h"


/**
 * @brief  BME680_Soldered begin method, should be called before using the class
 */
bool BME680::begin()
{
    // for (int i = 0; i < 10 && !BME680_Class::begin(); ++i)
    //     delay(1000);

    bool returnValue = BME680_Class::begin();

    if (returnValue)
    {
        // Usually default settings are fine.
        BME680_Class::setOversampling(TemperatureSensor, Oversample16); // Use enumerated type values
        BME680_Class::setOversampling(HumiditySensor, Oversample16);    // Use enumerated type values
        BME680_Class::setOversampling(PressureSensor, Oversample16);    // Use enumerated type values
        BME680_Class::setIIRFilter(IIR4);                               // Use enumerated type values
        BME680_Class::setGas(320, 150);
    }

    return returnValue;
}

/**
 * @brief         BME680 temperature method
 *
 * @returns float Tempearture in degree C
 */
float BME680::readTemperature()
{
    int32_t temp, humidity, pressure, gas;
    BME680_Class::getSensorData(temp, humidity, pressure, gas);

    return temp / 100.0;
}

/**
 * @brief         BME680 Pressure method
 *
 * @returns float Pressure in hPa
 */
float BME680::readPressure()
{
    int32_t temp, humidity, pressure, gas;
    BME680_Class::getSensorData(temp, humidity, pressure, gas);

    return pressure / 100.0;
}

/**
 * @brief         BME680 Humidity method
 *
 * @returns float Humidity in %
 */
float BME680::readHumidity()
{
    int32_t temp, humidity, pressure, gas;
    BME680_Class::getSensorData(temp, humidity, pressure, gas);

    return humidity / 1000.0;
}

/**
 * @brief         BME680 Altiude method
 *
 * @returns float Altitude in m
 */
float BME680::readAltitude()
{
    int32_t temp, humidity, pressure, gas;
    BME680_Class::getSensorData(temp, humidity, pressure, gas);

    float seaLevel = 1013.25;
    return 44330.0 * (1.0 - pow(((float)pressure / 100.0) / seaLevel, 0.1903)); // Convert into meters
}

/**
 * @brief         BME680 Gas resistance method, default at 320 degrees for 150ms
 *
 * @returns float Gas resistance in mOhms
 */
float BME680::readGasResistance()
{
    int32_t temp, humidity, pressure, gas;
    BME680_Class::getSensorData(temp, humidity, pressure, gas);

    return gas / 100.0;
}

/**
 * @brief                    BME680 all sensor data method
 *
 * @param float& temp        Temperature in degree C
 * @param float& humidity    Humidity in %
 * @param float& pressure    Pressure in Pa
 * @param float& gas         Gas resistance in mOhms
 */
void BME680::readSensorData(float &temp, float &humidity, float &pressure, float &gas)
{
    int32_t _temp, _humidity, _pressure, _gas;
    BME680_Class::getSensorData(_temp, _humidity, _pressure, _gas);

    temp = _temp / 100.0;
    humidity = _humidity / 1000.0;
    pressure = _pressure / 100.0;
    gas = _gas / 100.0;
}

/**
 * @brief                BME680 altitude calculation method
 *
 * @param float pressure Pressure in Pa
 *
 * @returns float        Altiude in m
 */
float BME680::calculateAltitude(float pressure)
{
    float seaLevel = 1013.25;
    return 44330.0 * (1.0 - pow(pressure / seaLevel, 0.1903)); // Convert into meters
}
