/*
   InkplatePLUS2_EasyC example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2,
   SI1142 sensor with easyC connector on it:
   https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/ and an easyC cable:
   https://soldered.com/product/easyc-cable-20cm/ Select "Soldered Inkplate PLUS2" from Tools -> Board menu. Don't have
   "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read temperature, humidity, and air pressure data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library
   If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   13 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Soldered InkplatePLUS2 in the boards menu."
#endif

#include "Inkplate.h"                           // Include Inkplate library to the sketch
#include "SI114X-light-sensor-easyc-SOLDERED.h" // Include Hall Effect library
#include "icons.h"                              // Include used icons to the sketch

// Create an object on Inkplate library and also set library into 1-bit mode (BW)
Inkplate display(INKPLATE_1BIT);

SI1142 lightSensor; // Create SI1142 sensor object

int numPartialUpdates = 0; // Variable to keep track of the number of partial updates

void setup()
{
    Serial.begin(112500); // So we can see the output
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)

    if (!lightSensor.begin(MEASUREMENT_MODE_AUTO)) // Initialize sensor in auto mode
    {
        Serial.println("Didn't find SI1142!");
        while (1)
        {
            // Loop forever if sensor is not found
            delay(100);
        }
    }

    // Set how often the sensor will wake up and make a new reading
    lightSensor.setAutoMeasurementRate(AUTO_500_MS);
}

void loop()
{
    // Periodically print sensor readings to Serial
    uint16_t visLight;                    // Variable to store visible light intensity
    visLight = lightSensor.readVisible(); // Get visible light intensity in lux
    Serial.print("Light intensity: ");
    Serial.print(visLight); // Print visible light intensity to Serial
    Serial.println(" lux.");

    uint16_t IRLight;               // Variable to store IR (infrared) light intensity
    IRLight = lightSensor.readIR(); // Get IR light intensity in lux
    Serial.print("IR light intensity: ");
    Serial.print(IRLight); // Print lIR ight intensity to Serial
    Serial.println(" lux.");

    // Also print the data on Inkplate
    display.setTextSize(3); // Set text size for printing
    display.clearDisplay(); // Clear what was previously in the buffer

    // Print light intensity
    display.setCursor(30, 30);
    display.print("Light intensity: ");
    display.print(visLight); // Print visible light intensity to Serial
    display.println(" lux.");

    // Print IR light
    display.setCursor(30, 70);
    display.print("IR light: ");
    display.print(visLight); // Print visible light intensity to Serial
    display.println(" lux.");

    // Every 5th refresh, do a full update
    if (numPartialUpdates == 5)
    {
        display.display();
        numPartialUpdates = 0;
    }
    // Otherwise, do a partial update, it's faster
    else
    {
        display.partialUpdate();
        numPartialUpdates++;
    }

    delay(2000); // wait a bit so the output isn't too fast
}