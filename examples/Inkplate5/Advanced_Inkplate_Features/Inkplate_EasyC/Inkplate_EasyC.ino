/*
   Inkplate_easyC example for Soldered Inkplate 5
   For this example you will need a micro USB cable, Inkplate 5,
   BME680 sensor with easyC connector on it: https://e-radionica.com/en/bme680-breakout-made-by-e-radionica.html
   and a easyC cable: https://e-radionica.com/en/easyc-cable-20cm.html
   Select "Inkplate 5(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 5(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can read temperature, humidity, air pressure and gas data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library
   If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   2 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Inkplate 5 in the boards menu."
#endif

#include "BME680-SOLDERED.h" //Soldered library for BME680 Sensor
#include "Inkplate.h"        //Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
BME680 bme680; // Create an object on Soldered BME680 library (with no arguments sent to constructor, that means we are
               // using I2C communication for BME680 sensor)

int n = 0; // Variable that keep track on how many times screen has been partially updated

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)

    if (!bme680.begin())
    { // Init. BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but you
      // don't need to specify that
        display.println("Sensor init failed!");
        display.println("Check sensor wiring/connection!");
        display.partialUpdate();
        while (1)
            ;
    }
}

void loop()
{
    // Print measured data on the screen
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Air temperature: ");
    display.print(bme680.readTemperature());
    display.println(" *C");

    display.print("Air pressure: ");
    display.print(bme680.readPressure());
    display.println(" hPa");

    display.print("Air humidity: ");
    display.print(bme680.readHumidity());
    display.println(" %");

    display.print("Gas sensor resistance: ");
    display.print(bme680.readGasResistance());
    display.println(" Ohms");

    if (n > 20)
    { // If display has been partially updated more than 20 times, do a full refresh, otherwise, perform a partial
      // update.
        display.display();
        n = 0;
    }
    else
    {
        display.partialUpdate();
        n++;
    }

    delay(2000); // Wait a little bit between readings
}
