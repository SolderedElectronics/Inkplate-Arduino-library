/**
 **************************************************
 *
 * @file        Read_All_Values.ino
 * @brief       Example of reading all values learn more at solde.red/333035
 *
 *
 *
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/

#include "BME280-SOLDERED.h"

// Declare your sensors here
BME280 bme280;

void setup()
{
    // Initialize the sensors
    Serial.begin(115200);

    // Initialize the sensors
    bme280.begin();
}

void loop() // Temperature is not factory calibrated, you can add offset to it
{
    float offset = 0; // offset in °C

    Serial.println("Reading one by one: ");

    float temperature = bme280.readTemperature() + offset;
    Serial.print("Temperature: ");
    Serial.println(temperature);

    float humidity = bme280.readHumidity();
    Serial.print("Humidity: ");
    Serial.println(humidity);

    float pressure = bme280.readPressure();
    Serial.print("Pressure: ");
    Serial.println(pressure);

    float altitude = bme280.readAltitude();
    Serial.print("Altitude: ");
    Serial.println(altitude);

    delay(1000);

    Serial.println("\nReading all at once: ");

    float temperature2, humidity2, pressure2;
    bme280.readSensorData(temperature2, humidity2, pressure2);

    Serial.print("Temperature: " + String(temperature2 + offset) + " C");
    Serial.println();

    Serial.print("Humidity: " + String(humidity2) + " %");
    Serial.println();

    Serial.print("Pressure: " + String(pressure2) + " hPa");
    Serial.println();

    // We need to caluclate altitude from pressure and sea level pressure
    float altitude2 = bme280.calculateAltitude(pressure2);

    Serial.print("Altitude: " + String(altitude2) + " m");
    Serial.println();
    Serial.println();
}
