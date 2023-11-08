/*
   Inkplate4TEMPERA_BME688_Read example for Soldered Inkplate 4 TEMPERA
   For this example you will need only a USB-C cable and Inkplate 4 TEMPERA.
   Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read temperature, pressure and humidity data
   from the built-in BME688 sensor and show the data on the display as well.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   9 Aug 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
#include "icons.h"    // Include header file with icons

// The sensor needs to be calibrated only with a single temperature offset in degrees Celsius
const float offset = -4;

// Variable that keeps count on how many times the screen has been partially updated
int n = 0;

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text

    // Enable the BME688 sensor.
    display.wakePeripheral(INKPLATE_BME688);

    display.bme688.begin(); // Init the sensor
}

void loop()
{
    // Read all the values from the sensor
    float temperature = display.bme688.readTemperature() + offset;
    float humidity = display.bme688.readHumidity();
    float pressure = display.bme688.readPressure();
    float gasResistance = display.bme688.readGasResistance();
    float altitude = display.bme688.readAltitude();

    // Print the values to the display
    display.clearDisplay(); // Clear what was previously written;

    // Print temperature
    display.drawImage(temperature_icon, 93, 100, temperature_icon_w, temperature_icon_h, BLACK); // Draw the icon
    display.setCursor(68, 69);                                                                   // Set position of text
    display.print("Temperature: ");
    display.setCursor(100, 241);   // Set position of text
    display.print(temperature, 2); // Print float with two decimal places
    display.print("C");

    // Print humidity
    display.drawImage(humidity_icon, 378, 100, humidity_icon_w, humidity_icon_h, BLACK); // Draw the icon
    display.setCursor(378, 69);                                                          // Set position of text
    display.print("Humidity: ");
    display.setCursor(386, 241); // Set position of text
    display.print(humidity, 2);  // Print float with two decimal places
    display.print("%");

    // Print pressure
    display.drawImage(pressure_icon, 93, 368, pressure_icon_w, pressure_icon_h, BLACK); // Draw the icon
    display.setCursor(89, 337);                                                         // Set position of text
    display.print("Pressure: ");
    display.setCursor(85, 509); // Set position of text
    display.print(pressure, 2); // Print float with two decimal places
    display.print(" hPa");

    // Also print gas resistance...
    display.setCursor(312, 342); // Set position of text
    display.print("Gas resistance:");
    display.setCursor(312, 393);     // Set position of text
    display.print(gasResistance, 2); // Print float with two decimal places
    display.print(" mOhm");

    // ...and altitude!
    display.setCursor(312, 450); // Set position of text
    display.print("Altitude:");
    display.setCursor(312, 501); // Set position of text
    display.print(altitude, 2);  // Print float with two decimal places
    display.print("m");

    // Update the display
    // Check if you need to do full refresh or you can do partial update
    if (n > 9)
    {
        display.display(); // Do a full refresh
        n = 0;
    }
    else
    {
        display.partialUpdate(false, true); // Do partial update
        n++;                                // Keep track on how many times screen has been partially updated
    }

    // Wait a bit
    delay(1000);
}
