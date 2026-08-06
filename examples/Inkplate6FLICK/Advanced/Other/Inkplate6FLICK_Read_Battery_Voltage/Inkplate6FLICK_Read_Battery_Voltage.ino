/**
 **************************************************
 * @file        Inkplate6FLICK_Read_Battery_Voltage.ino
 * @brief       Battery voltage reading demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to measure and display the connected Li-ion
 *              battery voltage using the built-in battery measurement circuit
 *              on Inkplate 6FLICK. The measured voltage is shown on the
 *              e-paper display together with a battery icon.
 *
 *              The sketch uses display.readBattery() to read the battery
 *              voltage and runs in 1-bit (black & white) mode. Ensure correct
 *              battery polarity before connecting. Expected output is a battery
 *              icon rendered on screen with the measured battery voltage
 *              displayed in volts (e.g. 3.92 V), updated every 10 seconds.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable,
 *               3.6-3.7 V Li-ion/LiPo battery with 2-pin JST connector
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Connect a compatible Li-ion/LiPo battery to the JST connector.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) The screen displays the measured battery voltage, updating every
 *    10 seconds.
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
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

void setup()
{
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
    display.setTextSize(2);             // Scale text to be two times bigger then original (5x7 px)
    display.setTextColor(BLACK, WHITE); // Set text color to black and background color to white
}

void loop()
{
    float voltage = display.readBattery();                   // Read battery voltage
    display.clearDisplay();                                  // Clear everything in frame buffer of e-paper display
    display.image.draw(battSymbol, 100, 100, 106, 45, BLACK); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
