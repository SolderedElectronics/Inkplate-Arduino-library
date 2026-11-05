/**
 **************************************************
 * @file        Inkplate10_Read_Battery_Voltage.ino
 * @brief       Battery voltage and temperature reading example for
 *              Soldered Inkplate 10.
 *
 * @details     Shows the battery voltage and the temperature of the
 *              Inkplate 10 in big text. The battery voltage is
 *              measured by the onboard measurement circuitry, and the
 *              temperature comes from the sensor inside the TPS65186
 *              e-paper PMIC.
 *
 *              The temperature sensor is a basic one meant for
 *              internal compensation, so it is not highly accurate. A
 *              battery is optional; without one the voltage reading
 *              is not meaningful. Expected output is the two readings
 *              on the Inkplate display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, 3.6-4.2 V Li-ion/Li-Po battery (optional)
 * - Extra:      SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) The battery voltage and the temperature appear on the display.
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
 * @author      Josip Šimun Kuči @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Partial update works in black and white mode only
Inkplate display(INKPLATE_1BIT);

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();

  // Start from a clean, fully refreshed screen
  display.clearDisplay();
  display.display();

  // Black text on a white background, in the font from T1, four times its normal size
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(4);

  // Make every tenth update a full refresh, to clear the leftovers
  display.setFullUpdateThreshold(9);
}

// loop() runs over and over
void loop() {
  // Take both measurements
  float voltage = display.readBattery();
  int temperature = display.readTemperature();

  // Redraw the screen
  display.clearDisplay();

  display.setCursor(120, 280);
  display.print("Battery: ");
  display.print(voltage, 2);  // two decimals
  display.print(" V");

  display.setCursor(120, 420);
  display.print("Temperature: ");
  display.print(temperature);
  display.print(" C");

  // Send only what changed
  display.partialUpdate(false, true);

  // Wait 10 seconds before the next measurement
  delay(10000);
}
