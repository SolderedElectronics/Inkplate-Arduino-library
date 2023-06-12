/*
   Inkplate5_easyC example for Soldered Inkplate 5
   For this example you will need a USB-C cable, Inkplate 5,
   BME680 sensor with easyC connector on it: https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/ 
   and a easyC cable: https://soldered.com/product/easyc-cable-20cm/ 
   Select "Soldered Inkplate5" from Tools -> Board menu. 
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read temperature, humidity, and air pressure data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library
   If you don't know how to install library you can read our tutorial https://soldered.com/learn/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

#include "BME680-SOLDERED.h" // Soldered library for BME680 Sensor
#include "Inkplate.h"        // Include Inkplate library to the sketch
#include "icons.h"           // Include header file with icons

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
BME680 bme680; // Create an object on Soldered BME680 library (with no arguments sent to constructor, that means we are
               // using I2C or easyC communication for BME680 sensor)

int n = 0; // Variable that keep track on how many times screen has been partially updated

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    // Init BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but doesn't need to
    // specify it
    if (!bme680.begin())
    {
        display.setTextSize(4); // Set text scaling to two (text will be two times bigger than normal (5x7 px))
        display.println("Sensor init failed!");
        display.println("Check sensor wiring/connection!");
        display.partialUpdate();
        while (1)
            ;
    }
}

void loop()
{
    // Clear frame buffer of the display
    display.clearDisplay();

    // Set text size to print big numbers for temperature and humidity
    display.setTextSize(8);

    // Display the temperature icon and measured value
    display.setCursor(350, 40);
    display.print(bme680.readTemperature());
    display.print(" *C");
    display.drawImage(temperature_icon, 158, 8, temperature_icon_w, temperature_icon_h,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display humidity icon and measured value
    display.setCursor(350, 245);
    display.print(bme680.readHumidity() / 10);
    display.print(" %");
    display.drawImage(humidity_icon, 150, 200, humidity_icon_w, humidity_icon_h,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display the pressure icon and measured value
    display.setTextSize(5);
    display.setCursor(350, 449);
    display.print(bme680.readPressure() * 10);
    display.print(" hPa");
    display.drawImage(pressure_icon, 150, 400, pressure_icon_w, pressure_icon_h,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display Soldered logo
    display.drawImage(logo, 755, 495, logo_w, logo_h,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // This part of code actually drawing on the Inkplate screen, previous lines just drawing into the frame buffer
    // Check if you need to do full refresh or you can do partial update
    if (n > 20)
    {
        // Do a full refresh
        display.display();
        n = 0;
    }
    else
    {
        display.partialUpdate(); // Do partial update
        n++;                     // Keep track on how many times screen has been partially updated
    }

    // Wait a little bit between readings
    delay(10000);
}
