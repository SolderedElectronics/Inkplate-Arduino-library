/**
 **************************************************
 * @file        Inkplate5V2_SetVCOM.ino
 * @brief       Reads and programs the Inkplate 5v2 EPD VCOM voltage via Serial.
 *
 * @details     This example shows how to read the currently stored VCOM value
 *              from the display power IC/EEPROM and (optionally) program a new
 *              VCOM value entered in the Serial Monitor. After reading or
 *              programming, a simple grayscale test pattern is drawn and the
 *              stored VCOM value is shown on the e-paper display.
 *
 *              VCOM is stored in EEPROM and can only be programmed a limited
 *              number of times. Do NOT run this sketch repeatedly or "tune"
 *              VCOM by trial-and-error. Program it once (only if needed) and
 *              leave it unchanged to avoid prematurely wearing out EEPROM.
 *
 *              Display mode is 3-bit grayscale (INKPLATE_3BIT) and a full
 *              refresh is used. If programming fails, verify you selected the
 *              correct Inkplate 5v2 board and use a stable USB connection/power
 *              source. Expected output is "Stored VCOM: <value> V" plus an
 *              8-step 3-bit grayscale pattern on the display, and the prompt,
 *              requested VCOM value and success/failure message on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      none
 * - Serial:     115200 baud, Newline (LF)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5v2.
 * 3) Open Serial Monitor at 115200 baud and set line ending to Newline (LF).
 * 4) Read the prompt and enter a VCOM value in the range [-5.0, 0.0]
 *    (example: -1.23), then press Send.
 * 5) The sketch validates the range, programs VCOM (if valid), and refreshes
 *    the display to show the stored VCOM and a grayscale bar pattern.
 *
 * @warning     VCOM is written to EEPROM with limited write endurance. Avoid
 *              repeated programming to prevent permanent wear/damage.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-07-29
 * @license     GNU GPL V3
 **************************************************/

#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include <Inkplate.h>

Inkplate display(INKPLATE_3BIT);

void displayTestImage()
{
    display.clearDisplay();

    double vcom = display.getVCOMValue();

    display.setTextColor(BLACK);
    display.setTextSize(2);
    display.setCursor(5, 5);
    display.print("Stored VCOM: ");
    display.print(vcom, 2);
    display.print(" V");

    for (int i = 0; i < 8; i++)
    {
        int x = (display.width() / 8) * i;

        display.drawRect(x, 40, display.width() / 8, display.height(), i);
        display.fillRect(x, 40, display.width() / 8, display.height(), i);
    }

    display.display(true);
}

void setup()
{
    Serial.begin(115200);
    display.begin();

    Serial.println("Enter VCOM value [-5.0, 0.0]");
    Serial.println("Example: -1.23");

    displayTestImage();
}

void loop()
{
    if (!Serial.available())
        return;

    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() == 0)
        return;

    double v = line.toFloat();

    Serial.print("Requested VCOM: ");
    Serial.println(v, 2);

    if (v < -5.0 || v > 0.0)
    {
        Serial.println("VCOM out of range!");
        return;
    }

    if (display.setVCOM(v))
        Serial.println("VCOM programmed OK");
    else
        Serial.println("VCOM programming failed");

    displayTestImage();
}