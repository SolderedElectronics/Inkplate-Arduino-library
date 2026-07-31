/*
  Inkplate13SPECTRA_EasyC example for Soldered Inkplate 13SPECTRA
  For this example you will need a micro USB cable, Inkplate 13SPECTRA,
  BME680 sensor with easyC connector on it: https://soldered.com/product/enviromental-air-quality-sensor-bme680-breakout/
  and a easyC cable: https://soldered.com/product/easyc-cable-20cm/
  Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
  Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

  https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

  This example will show you how you can read temperature, humidity, and air pressure data from BME680.
  In order to compile this example successfuly, you will also need to download and install
  Soldered BME680 library: https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include <BME680-SOLDERED.h> // Soldered library for BME680 Sensor
#include "Inkplate.h"        // Include Inkplate library to the sketch
#include "icons.h"

Inkplate inkplate; // Create an object on Inkplate library
BME680 bme680;    // Create an object on Soldered BME680 library (with no arguments sent to constructor, that means we are
                  // using I2C or easyC communication for BME680 sensor)

// Add temperature offset to calibrate the sensor
const float temperatureOffset = 0.0;

void setup()
{
  Serial.begin(115200);
  inkplate.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.clearDisplay(); // Clear frame buffer of display
  inkplate.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)
  inkplate.setTextColor(INKPLATE_BLACK);
  inkplate.setCursor(0, 0);

  // Init. BME680 library. Soldered BME680 sensor board uses 0x76 I2C address for the sensor but doesn't need to
  // specify it
  if (!bme680.begin())
  {
    inkplate.println("Sensor init failed!");
    inkplate.println("Check sensor wiring/connection!");
    inkplate.display();
    while (1)
        ;
  }
}

void loop()
{
  // Clear frame buffer of the display
  inkplate.clearDisplay();

  // Set text size to print big numbers for temperature and humidity
  inkplate.setTextSize(6);

  // Display the temperature icon and measured value
  inkplate.setCursor(200, 45);
  inkplate.print(bme680.readTemperature() + temperatureOffset);
  inkplate.print(" *C");
  inkplate.drawBitmap(50, 5, temperature_icon, temperature_icon_w, temperature_icon_h, INKPLATE_RED); // Arguments are: start X, start Y, array variable name, size X, size Y, color
  

  // Display humidity icon and measured value
  inkplate.setCursor(200, 180);
  inkplate.print(bme680.readHumidity() / 10);
  inkplate.print(" %");
  inkplate.drawBitmap(43, 140, humidity_icon, humidity_icon_w, humidity_icon_h, INKPLATE_GREEN); // Arguments are: start X, start Y, array variable name, size X, size Y, color


  // Display the pressure icon and measured value
  inkplate.setTextSize(5);
  inkplate.setCursor(200, 315);
  inkplate.print(bme680.readPressure() * 10);
  inkplate.print(" hPa");
  inkplate.drawBitmap(50, 270, pressure_icon, pressure_icon_w, pressure_icon_h, INKPLATE_BLUE); // Arguments are: start X, start Y, array variable name, size X, size Y, color

  // Display Soldered logo
  inkplate.drawBitmap(425, 401, logo, logo_w, logo_h, INKPLATE_YELLOW); // Arguments are: start X, start Y, array variable name, size X, size Y, color

  // This line actually drawing on the Inkplate screen, previous lines just drawing into the frame buffer
  inkplate.display();

  // Wait a minute bit between readings due to slow refresh rate
  delay(60000);

  // If you want to save energy, instead of the delay function, you can use deep sleep as we used in DeepSleep
  // examples
}