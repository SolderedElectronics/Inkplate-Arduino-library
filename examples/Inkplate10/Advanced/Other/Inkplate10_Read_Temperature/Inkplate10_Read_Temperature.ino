/**
 **************************************************
 * @file        Inkplate10_Read_Temperature.ino
 * @brief       On-board temperature sensor reading example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to read temperature data from the on-board
 *              temperature sensor integrated inside the TPS65186 e-paper PMIC.
 *              This sensor is intended primarily for internal compensation
 *              and basic monitoring. It is a simple (basic) temperature sensor
 *              and should not be considered highly accurate or suitable for
 *              precise temperature measurements.
 *
 *              The sensor is not a replacement for a dedicated external
 *              temperature sensor; its intended use is system monitoring and
 *              waveform compensation. Expected output is an approximate
 *              temperature reading reported by the TPS65186 sensor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) The program reads the temperature from the onboard PMIC sensor.
 * 4) The measured value can be displayed or printed to Serial.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-01-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include "tempSymbol.h" //Include .h file that contains byte array for temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on tempSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();             // Clear frame buffer of display
    display.display();                  // Put clear image on display
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    int temperature = display.readTemperature();            // Read temperature from on-board temperature sensor
    display.clearDisplay();                                 // Clear frame buffer of display
    display.image.draw(tempSymbol, 100, 100, 38, 79, BLACK); // Draw temperature symbol at position X=100, Y=100
    display.setCursor(150, 125);
    display.print(temperature, DEC); // Print temperature
    display.print('C');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
