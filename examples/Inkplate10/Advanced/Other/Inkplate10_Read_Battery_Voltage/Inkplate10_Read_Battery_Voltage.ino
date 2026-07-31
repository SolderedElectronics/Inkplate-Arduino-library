/**
 **************************************************
 * @file        Inkplate10_Read_Battery_Voltage.ino
 * @brief       Battery voltage reading example for Soldered Inkplate 10.
 *
 * @details     Demonstrates how to read the connected Li-ion/Li-Po battery
 *              voltage using Inkplate's built-in battery measurement circuitry.
 *              The example shows how to obtain the battery voltage value in
 *              software and display or process it as needed.
 *
 *              Battery voltage reading is enabled through the onboard
 *              circuitry, and typically requires enabling the battery
 *              measurement path in hardware (see Inkplate documentation).
 *              Accuracy depends on battery condition and load. Expected output
 *              is the measured battery voltage value reported by the program.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, 3.6-4.2 V Li-ion/Li-Po battery (JST connector)
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Connect a supported Li-ion/Li-Po battery to the Inkplate battery
 *    connector.
 * 3) Upload the sketch to Inkplate 10.
 * 4) The battery voltage is read and can be displayed or logged by the sketch.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        Image converter tool:
 *              https://tools.soldered.com/tools/image-converter/
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
