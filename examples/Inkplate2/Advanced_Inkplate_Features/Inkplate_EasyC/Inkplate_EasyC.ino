/*
   Inkplate_easyC example for e-radionica.com Inkplate 2
   For this example you will need a micro USB cable, Inkplate 2,
   BMP180 sensor with easyC connector on it: https://e-radionica.com/en/bme680-breakout-made-by-e-radionica.html
   and a easyC cable: https://e-radionica.com/en/easyc-cable-20cm.html
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can read temperature, humidity, air pressure and gas data from BME680.
   In order to compile this example successfuly, you will also need to download and install
   BMP1880 library: https://github.com/e-radionicacom/Soldered-BMP180-Temperature-Pressure-Sensor-Arduino-Library.
   If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "BMP180-SOLDERED.h" //Adafruit library for BME680 Sensor
#include "Inkplate.h"        //Include Inkplate library to the sketch

#include "thermometer.h"
#include "pressure.h"
#include "altitude.h"

Inkplate display; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
Bmp_180 myBmp; // Create an object on Adafruit BME680 library
//(with no arguments sent to constructor, that means we are using I2C communication for BME680 sensor)

void setup()
{
  display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); // Clear frame buffer of display
  display.setTextColor(BLACK, WHITE);

  if (!myBmp.begin())
  { // Init. BME680 library. e-radionica.com BME680 sensor board uses 0x76 I2C address for sensor
    display.println("Sensor init failed!");
    display.println("Check sensor wiring/connection!");
    display.display();
    while (1)
      ;
  }
}

void loop()
{
  double temp, pressure, p0, a;
  int status = myBmp.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = myBmp.getTemperature(temp);
    status = myBmp.startPressure(3);
    delay(status);
    status = myBmp.getPressure(pressure, temp);
    // The pressure sensor returns abolute pressure, which varies with altitude.
    // To remove the effects of altitude, use the sealevel function and your current altitude.
    // This number is commonly used in weather reports.
    // Parameters: pressure = absolute pressure in mb, ALTITUDE = current altitude in m.
    // Result: p0 = sea-level compensated pressure in mb
    double altitude = 90;
    p0 = myBmp.sealevel(pressure, altitude); // we're at 90 meters (Osijek, CRO)
    // On the other hand, if you want to determine your altitude from the pressure reading,
    // use the altitude function along with a baseline pressure (sea-level or other).
    // Parameters: pressure = absolute pressure in mb, p0 = baseline pressure in mb.
    // Result: a = altitude in m.

    a = myBmp.altitude(pressure, p0);
  }

  display.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)
  display.setTextColor(BLACK);
  display.clearDisplay(); // Print out new data
  display.drawBitmap(20, 10, (uint8_t*)thermometer, 18, 40, RED);
  display.setCursor(60, 20);
  display.print(temp, 2); // Print air temperature
  display.println(" C");

  display.drawBitmap(10, 60, (uint8_t*)pressure_icon, 40, 40, RED);
  display.setCursor(60, 70);
  display.print(pressure, 2); //Convert to air pressure hPa and print on display
  display.println(" hPa");

  display.display();
  delay(60000);
}
