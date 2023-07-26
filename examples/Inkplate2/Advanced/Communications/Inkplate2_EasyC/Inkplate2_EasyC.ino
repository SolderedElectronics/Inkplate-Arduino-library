/*
    Inkplate2_EasyC example for Soldered Inkplate 2
    For this example you will need a micro USB cable, Inkplate 2,
    BME680 sensor with easyC connector on it:
   https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/ and a easyC cable:
   https://soldered.com/product/easyc-cable-20cm/ Select "Soldered Inkplate2" from Tools -> Board menu. Don't have
   "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can read temperature and humidity from BME680.
    In order to compile this example successfuly, you will also need to download and install
    BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library.
    If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    2 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"                        // Include Inkplate library to the sketch
#include "libs/BME680/src/BME680-SOLDERED.h" // Soldered library for BME680 Sensor

// Bitmaps for the icons
#include "humidity.h"
#include "thermometer.h"

Inkplate display; // Create an object on Inkplate library
BME680 bme680;    // Create an object on BME680 library

void setup()
{
    Serial.begin(115200);   // Initialize Serial communication with PC
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Init. BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but you
    // don't need to specify that
    if (!bme680.begin())
    {
        Serial.println("Sensor init failed");   // Print message on serial monitor
        display.println("Sensor init failed!"); // Print message on display
        display.println("Check sensor wiring/connection!");
        display.display();
        while (1)
            ;
    }
}

void loop()
{
    // Variables for storing measured data
    double temp, hum;

    // Read values from the sensor
    temp = bme680.readTemperature();
    hum = bme680.readHumidity() / 10;

    display.setTextSize(2);                // Set text scaling to two (text will be two times bigger than normal)
    display.setTextColor(INKPLATE2_BLACK); // Set text color to black
    display.clearDisplay();                // Print out new data
    display.drawImage(thermometer, 20, 8, 18, 45); // Draw thermometer icon
    display.setCursor(60, 22);                     // Set text cursor position at X = 60, Y = 22
    display.print(temp, 2);                        // Print air temperature
    display.println(" C");

    display.drawImage(humidity_icon, 5, 58, 45, 45); // Draw pressure icon
    display.setCursor(60, 72);                       // Set text cursor position at X = 60, Y = 72
    display.print(hum, 2);                           // Convert to air pressure hPa and print on display
    display.println(" %");

    display.display(); // Refresh the display
    delay(60000);      // Wait 60000 miliseconds or 1 minute

    // If you want to save energy, instead of the delay function, you can use deep sleep as we used in DeepSleep
    // examples
}
