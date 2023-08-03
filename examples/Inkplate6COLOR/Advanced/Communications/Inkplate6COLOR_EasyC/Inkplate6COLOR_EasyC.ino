/*
   Inkplate6COLOR_EasyC example for Soldered Inkplate 6COLOR
   For this example you will need a micro USB cable, Inkplate 6COLOR,
   BME680 sensor with easyC connector on it: https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/
   and a easyC cable: https://soldered.com/product/easyc-cable-20cm/
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read temperature, humidity, and air pressure data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library
   If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   2 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include <BME680-SOLDERED.h> // Soldered library for BME680 Sensor
#include "Inkplate.h"        // Include Inkplate library to the sketch
#include "icons.h"

Inkplate display; // Create an object on Inkplate library
BME680 bme680;    // Create an object on Soldered BME680 library (with no arguments sent to constructor, that means we are
                  // using I2C or easyC communication for BME680 sensor)

// Add temperature offset to calibrate the sensor
const float temperatureOffset = 0.0;

void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)
    display.setTextColor(INKPLATE_ORANGE);
    display.setCursor(0, 0);

    // Init. BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but doesn't need to
    // specify it
    if (!bme680.begin())
    {
        display.println("Sensor init failed!");
        display.println("Check sensor wiring/connection!");
        display.display();
        while (1)
            ;
    }
}

void loop()
{
    // Clear frame buffer of the display
    display.clearDisplay();

    // Set text size to print big numbers for temperature and humidity
    display.setTextSize(6);

    // Display the temperature icon and measured value
    display.setCursor(200, 45);
    display.print(bme680.readTemperature() + temperatureOffset);
    display.print(" *C");
    display.drawBitmap(50, 5, temperature_icon, temperature_icon_w, temperature_icon_h, INKPLATE_RED); // Arguments are: start X, start Y, array variable name, size X, size Y, color
    

    // Display humidity icon and measured value
    display.setCursor(200, 180);
    display.print(bme680.readHumidity() / 10);
    display.print(" %");
    display.drawBitmap(43, 140, humidity_icon, humidity_icon_w, humidity_icon_h, INKPLATE_GREEN); // Arguments are: start X, start Y, array variable name, size X, size Y, color


    // Display the pressure icon and measured value
    display.setTextSize(5);
    display.setCursor(200, 315);
    display.print(bme680.readPressure() * 10);
    display.print(" hPa");
    display.drawBitmap(50, 270, pressure_icon, pressure_icon_w, pressure_icon_h, INKPLATE_BLUE); // Arguments are: start X, start Y, array variable name, size X, size Y, color

    // Display Soldered logo
    display.drawBitmap(425, 401, logo, logo_w, logo_h, INKPLATE_YELLOW); // Arguments are: start X, start Y, array variable name, size X, size Y, color

    // This line actually drawing on the Inkplate screen, previous lines just drawing into the frame buffer
    display.display();

    // Wait a minute bit between readings due to slow refresh rate
    delay(60000);

    // If you want to save energy, instead of the delay function, you can use deep sleep as we used in DeepSleep
    // examples
}
