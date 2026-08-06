/**
 **************************************************
 * @file        Inkplate6FLICK_Read_Temperature.ino
 * @brief       On-board temperature sensor reading demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to read temperature from the on-board
 *              temperature sensor integrated in the TPS65186 e-paper
 *              PMIC (Power Management IC) on Inkplate 6FLICK.
 *              The measured temperature is displayed on the e-paper screen.
 *
 *              The temperature reflects the PMIC/chip temperature, not the
 *              ambient air temperature, so the readings are not 100% accurate.
 *              Expected output is the temperature value (degrees C) shown on the
 *              e-paper display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Libraries:  Inkplate library
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6FLICK.
 * 3) After initialization, the measured temperature is displayed.
 * 4) Screen refresh behaviour depends on the implementation (full or partial
 *    update).
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "tempSymbol.h" // Include .h file that contains byte array for temperature symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on tempSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    int temperature = display.readTemperature();            // Read temperature from on-board temperature sensor
                                                            // NOTE: The temperature readings are not 100% accurate!
                                                            // See header comment for more details.

    display.clearDisplay();                                 // Clear everything in frame buffer of e-paper display
    display.image.draw(tempSymbol, 100, 100, 38, 79, BLACK); // Draw temperature symbol at position X=100, Y=100
    display.setCursor(150, 120);
    display.print(temperature, DEC); // Print temperature
    display.print('C');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
