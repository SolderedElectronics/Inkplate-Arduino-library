/*
   Inkplate4_EasyC example for Soldered Inkplate 4
   For this example you will need a USB-C cable, Inkplate 4,
   BME680 sensor with easyC connector on it: https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/ 
   and a easyC cable: https://soldered.com/product/easyc-cable-20cm/ 
   Select "Soldered Inkplate4" from Tools -> Board menu. 
   Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read temperature, humidity, and air pressure data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library
   If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   6 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include <BME680-SOLDERED.h> // Soldered library for BME680 Sensor
#include "Inkplate.h"        // Include Inkplate library to the sketch
#include "icons.h"           // Include used icons to the sketch

// Create an object on Inkplate library
Inkplate display;

// Create an object on Soldered BME680 library
BME680 bme680;

// Add temperature offset to calibrate the sensor
const float temperatureOffset = 0.0;

void setup()
{
    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setTextSize(3);      // Set text scaling to 3 (text will be 3 times bigger than normal)
    display.setTextColor(BLACK); // Set text color to black

    // Init. BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but doesn't need to
    // specify it
    if (!bme680.begin())
    {
        display.println("Sensor init failed!");
        display.println("Check sensor wiring/\nconnection!");
        display.display();
        while (1)
            ;
    }
}

void loop()
{
    // Clear frame buffer of the display
    display.clearDisplay();

    // Display the temperature icon and measured value
    display.setCursor(155, 35); // Arguments are: X coordinate, Y coordinate
    display.print(bme680.readTemperature() + temperatureOffset);
    display.print(" *C");
    // Draw thermometer icon
    // Arguments are: array variable name, start X, start Y, size X, size Y
    display.drawImage(temperature_icon, 80, 10, 70, 70);

    // Display humidity icon and measured value
    display.setCursor(155, 120); // Arguments are: X coordinate, Y coordinate
    display.print(bme680.readHumidity() / 10);
    display.print(" %");
    // Draw humidity icon
    // Arguments are: array variable name, start X, start Y, size X, size Y
    display.drawImage(humidity_icon, 75, 95, 70, 70);

    // Display the pressure icon and measured value
    display.setTextSize(3);
    display.setCursor(155, 205); // Arguments are: X coordinate, Y coordinate
    display.print(bme680.readPressure() * 10);
    display.print(" hPa");
    // Draw pressure icon
    // Arguments are: array variable name, start X, start Y, size X, size Y
    display.drawImage(pressure_icon, 70, 185, 70, 70);

    // Draw Soldered logo
    // Arguments are: array variable name, start X, start Y, size X, size Y
    display.drawImage(logo, 240, 268, 160, 32);

    // This line actually drawing on the Inkplate screen, previous lines just drawing into the frame buffer
    display.display();

    // Wait a minute between readings
    delay(60000);

    // If you want to save energy, instead of the delay function, you can use deep sleep as we used in DeepSleep
    // examples
}
