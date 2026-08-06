/**
 **************************************************
 * @file        Inkplate13SPECTRA_Read_Battery_Voltage.ino
 * @brief       Battery voltage reading example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to read the voltage of the connected battery.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, Lithium battery (3.6 V) with a
 *               two pin JST connector
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Connect the battery to the Inkplate battery connector.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The measured battery voltage is shown on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        In order to convert your images into a format compatible with
 *              Inkplate, use the Soldered Image Converter available at
 *              https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
 *
 * @author      Soldered
 * @date        2023-01-23
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
// It is in same folder as this sketch. You can even open it (read it) by clicking on battSymbol.h tab in Arduino IDE
Inkplate display; // Create an object on Inkplate library

void setup()
{
    display.begin(); // Init Inkplate library (you should call this function ONLY ONCE)
    Serial.begin(115200);
}

void loop()
{
    float voltage = display.readBattery(); // Read battery voltage
    /*display.clearDisplay();                // Clear everything in frame buffer of e-paper display
    display.drawBitmap(100, 100, battSymbol, battSymbol_w, battSymbol_h,
                       INKPLATE_BLUE); // Draw battery symbol at position X=100 Y=100
    display.setCursor(210, 120);
    display.setTextColor(INKPLATE_BLUE);
    display.setTextSize(3);
    display.print(voltage, 2); // Print battery voltage
    display.print('V');
    display.display(); // Send everything to display (refresh the screen)*/
    Serial.printf("Voltage: %dV\n", voltage);
    delay(10000);      // Wait 10 seconds before new measurement
}