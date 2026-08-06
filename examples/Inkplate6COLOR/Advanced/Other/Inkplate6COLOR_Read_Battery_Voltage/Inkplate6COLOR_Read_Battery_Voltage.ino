/**
 **************************************************
 * @file        Inkplate6COLOR_Read_Battery_Voltage.ino
 * @brief       Reads and displays the connected battery voltage on Inkplate
 *              6COLOR.
 *
 * @details     This example demonstrates how to measure the voltage of a
 *              lithium battery connected to the Inkplate 6COLOR battery
 *              connector. The board includes a battery monitoring circuit
 *              that allows the ESP32 to measure the battery voltage using
 *              the Inkplate library.
 *
 *              The sketch periodically reads the battery voltage using
 *              display.readBattery(), clears the display buffer, draws a
 *              battery icon, and prints the measured voltage value beside it.
 *              The screen is refreshed every 10 seconds with a new reading.
 *              The example includes a bitmap image (battSymbol.h) used to render
 *              the battery icon.
 *
 *              This example is useful for battery-powered projects where it
 *              is important to monitor charge level and display the current
 *              supply voltage on the screen. Each update performs a full display
 *              refresh, which is slower and consumes more energy than monochrome
 *              partial-update workflows on supported boards. Battery voltage
 *              measurement accuracy depends on the onboard ADC and voltage
 *              divider tolerances, so readings should be treated as approximate.
 *
 *              Expected output: a battery symbol and the measured voltage value
 *              (for example 3.85 V) on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable, 3.7 V Li-Po/Li-Ion battery with JST
 *               connector
 * - Extra:      none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Connect a compatible lithium battery to the onboard JST battery connector.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) After startup, the display shows a battery icon and the measured voltage.
 * 5) The measurement updates every 10 seconds.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
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
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "battSymbol.h" // Include .h file that contains byte array for battery symbol.
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
}

void loop()
{
    float voltage = display.readBattery(); // Read battery voltage
    display.clearDisplay();                // Clear everything in frame buffer of e-paper display
    display.drawBitmap(100, 100, battSymbol, battSymbol_w, battSymbol_h,
                       INKPLATE_BLUE); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.setTextColor(INKPLATE_BLUE);
    display.setTextSize(3);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)
    delay(10000);      // Wait 10 seconds before new measurement
}
