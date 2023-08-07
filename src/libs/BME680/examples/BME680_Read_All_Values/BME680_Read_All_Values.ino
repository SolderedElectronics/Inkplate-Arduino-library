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

#include "BME680-SOLDERED.h"

// Declare your sensors here
BME680 bme680;

void setup()
{
    // Initialize the sensors
    Serial.begin(115200);

    // Initialize the sensors
    bme680.begin();
}

void loop() // Temperature is not factory calibrated, you can add offset to it
{
    float offset = -4; // offset in °C

    Serial.println("Reading one by one: ");

    float temperature = bme680.readTemperature() + offset;
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    float humidity = bme680.readHumidity();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    float pressure = bme680.readPressure();
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    float gas = bme680.readGasResistance();
    Serial.print("Gas: ");
    Serial.print(gas);
    Serial.println(" mOhm");

    float altitude = bme680.readAltitude();
    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println(" m");

    delay(1000);

    Serial.println("\nReading all at once: ");

    float temperature2, humidity2, pressure2, gas2;
    bme680.readSensorData(temperature2, humidity2, pressure2, gas2);

    Serial.print("Temperature: " + String(temperature2 + offset) + " C");
    Serial.println();

    Serial.print("Humidity: " + String(humidity2) + " %");
    Serial.println();

    Serial.print("Pressure: " + String(pressure2) + " hPa");
    Serial.println();

    Serial.print("Gas: " + String(gas2) + " mOhm");
    Serial.println();

    // We need to caluclate altitude from pressure and sea level pressure
    float altitude2 = bme680.calculateAltitude(pressure2);

    Serial.print("Altitude: " + String(altitude2) + " m");
    Serial.println();
    Serial.println();

    delay(1000);
}
