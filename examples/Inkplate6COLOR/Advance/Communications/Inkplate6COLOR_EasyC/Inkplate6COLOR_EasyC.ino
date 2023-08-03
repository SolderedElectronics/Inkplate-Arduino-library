/*
   Inkplate6COLOR_EasyC example for Soldered Inkplate 6COLOR
   For this example you will need a micro USB cable, Inkplate 6COLOR,
   BME680 sensor with easyC connector on it: https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/
   and a easyC cable: https://soldered.com/product/easyc-cable-20cm/
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can read temperature, humidity, air pressure and gas data from BME680.
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

#include "BME680-SOLDERED.h" //Soldered library for BME680 Sensor
#include "Inkplate.h"        //Include Inkplate library to the sketch
#include "icons.h"

Inkplate display; // Create an object on Inkplate library
BME680 bme680; // Create an object on Soldered BME680 library (with no arguments sent to constructor, that means we are
               // using I2C or easyC communication for BME680 sensor)

int n = 0; // Variable that keep track on how many times screen has been partially updated
void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)
    display.setTextColor(INKPLATE_BLACK);
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
    display.setTextSize(7);

    // Display the temperature icon and measured value
    display.setCursor(250, 45);
    display.print(bme680.readTemperature());
    display.print(" *C");
    display.drawImage(temperature_icon, 90, 15, 110, 110,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display humidity icon and measured value
    display.setCursor(250, 180);
    display.print(bme680.readHumidity() / 10);
    display.print(" %");
    display.drawImage(humidity_icon, 90, 150, 110, 110,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display the pressure icon and measured value
    display.setTextSize(5);
    display.setCursor(250, 315);
    display.print(bme680.readPressure() * 10);
    display.print(" hPa");
    display.drawImage(pressure_icon, 90, 285, 110, 110,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // Display Soldered logo
    display.drawImage(logo, 425, 401, 160, 32,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color

    // This line actually drawing on the Inkplate screen, previous lines just drawing into the frame buffer
    display.display();

    // Wait a little bit between readings
    delay(15000);
}
