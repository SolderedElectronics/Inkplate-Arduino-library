/**
 **************************************************
 * @file        Inkplate4TEMPERA_BME688_Read.ino
 * @brief       Reads environmental data from the on-board BME688 sensor and
 *              displays the values with simple icons in 1-bit BW mode.
 *
 * @details     This example demonstrates how to use the built-in Bosch BME688
 *              sensor on Inkplate 4 TEMPERA. After enabling the peripheral
 *              power domain and initializing the sensor, the sketch repeatedly
 *              reads:
 *              - Temperature (with a user-defined offset calibration)
 *              - Relative humidity
 *              - Barometric pressure
 *              - Gas resistance
 *              - Estimated altitude (derived from pressure)
 *
 *              The values are rendered to the e-paper display alongside small
 *              bitmap icons for temperature, humidity, and pressure. The screen
 *              is updated once per second. To reduce flicker and improve update
 *              speed, the sketch performs partial updates most of the time and
 *              forces a full refresh after ~10 partial updates to limit
 *              ghosting. Panel power is kept enabled during partial updates for
 *              stability and speed (higher power usage).
 *
 *              The BME688 is enabled via wakePeripheral(INKPLATE_BME688);
 *              disable or deep-sleep peripherals when optimizing for low power.
 *              Altitude is an estimate derived from pressure and depends on
 *              reference sea level assumptions, so it is not a precision
 *              altitude measurement.
 *
 *              Expected output: temperature (degrees C), humidity (%) and
 *              pressure (hPa) shown with icons, gas resistance and altitude
 *              values printed as text on the lower half of the screen, with
 *              values refreshing about once per second.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Set the temperature calibration offset (offset, in degrees C) if needed.
 * 3) Upload the sketch to Inkplate 4 TEMPERA.
 * 4) The device powers the BME688, reads sensor values, and updates the display
 *    continuously once per second.
 * 5) If temperature reads consistently high/low, adjust the offset constant.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-08-09
 * @license     GNU GPL V3
 **************************************************/

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
    display.image.draw(temperature_icon, 93, 100, temperature_icon_w, temperature_icon_h, BLACK); // Draw the icon
    display.setCursor(68, 69);                                                                   // Set position of text
    display.print("Temperature: ");
    display.setCursor(100, 241);   // Set position of text
    display.print(temperature, 2); // Print float with two decimal places
    display.print("C");

    // Print humidity
    display.image.draw(humidity_icon, 378, 100, humidity_icon_w, humidity_icon_h, BLACK); // Draw the icon
    display.setCursor(378, 69);                                                          // Set position of text
    display.print("Humidity: ");
    display.setCursor(386, 241); // Set position of text
    display.print(humidity, 2);  // Print float with two decimal places
    display.print("%");

    // Print pressure
    display.image.draw(pressure_icon, 93, 368, pressure_icon_w, pressure_icon_h, BLACK); // Draw the icon
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
