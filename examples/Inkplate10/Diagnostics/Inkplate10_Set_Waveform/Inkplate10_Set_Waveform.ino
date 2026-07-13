/**
 **************************************************
 * @file        Inkplate10_Waveform_Select.ino
 * @brief       Selects and (optionally) burns an Inkplate 10 waveform via Serial.
 *
 * @details     This example lets you preview Inkplate 10 EPD waveforms (1–5) and
 *              choose the best-looking one for your panel. Type a number (1–5)
 *              in Serial Monitor to preview that waveform on an 8-step grayscale
 *              gradient. Type "test" to show a demo image. Type "ok" to burn the
 *              currently selected waveform into ESP32 EEPROM using:
 *                  display.setWaveform(waveformNumber, burnToEEPROM)
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10 / Inkplate 10 V2
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      image.h (demo image) for "test" command
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate10
 * - Serial Monitor: 115200 baud, Newline (LF)
 *
 * How to use:
 * 1) Upload the sketch to Inkplate 10.
 * 2) Open Serial Monitor at 115200 baud and set line ending to Newline (LF).
 * 3) Send a number 1–5 to preview waveform on gradient.
 * 4) Send "test" to show demo image.
 * 5) Send "ok" to burn the currently selected waveform to EEPROM.
 *
 * Notes:
 * - EEPROM stores waveform data (limited endurance). Only burn when needed.
 * - Display mode: 3-bit grayscale (INKPLATE_3BIT).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2026-03-05
 * @license     GNU GPL V3
 **************************************************/
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Select Inkplate10 board"
#endif

#include <Inkplate.h>
#include "image.h"

Inkplate display(INKPLATE_3BIT);

static int currentWaveform = 1;

static void showGradient(int selected)
{
    display.clearDisplay();

    int w = display.width() / 8;
    int h = display.height() - 100;

    display.fillRect(0, 725, 1200, 100, 7);

    display.setTextSize(4);
    display.setTextColor(0);
    display.setCursor(10, 743);
    display.print("Waveform select: ");

    display.setCursor(432, 743);
    for (int i = 1; i <= 5; i++)
    {
        display.print(i);
        display.print(' ');
    }

    display.drawRect(((selected-1) * 6 * 4 * 2) + 432 - 3, 740, (6 * 4) + 2, (8 * 4) + 2, 0);

    for (int i = 0; i < 8; i++)
        display.fillRect(i * w, 0, w, h, i);

    display.setTextSize(3);
    display.setCursor(10, 792);
    display.print("Send 1-5, \"ok\" to burn, \"test\" for image");

    display.display();
}

static void showTestImage()
{
    display.clearDisplay();
    display.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.display();
}

static int readCommand()
{
    if (!Serial.available())
        return 0;

    String s = Serial.readStringUntil('\n');
    s.trim();
    s.toLowerCase();

    if (s == "ok")
        return 255;

    if (s == "test")
        return 254;

    int n = s.toInt();
    if (n >= 1 && n <= 5)
        return n;

    return 0;
}

void setup()
{
    Serial.begin(115200);
    display.begin();

    Serial.println("Waveform select: send 1-5 to preview, \"test\" for image, \"ok\" to burn.");

    display.setWaveform(currentWaveform, false);
    showGradient(currentWaveform);

    while (true)
    {
        int cmd = readCommand();

        if (cmd >= 1 && cmd <= 5)
        {
            currentWaveform = cmd;
            display.setWaveform(currentWaveform, false);
            showGradient(currentWaveform);
        }
        else if (cmd == 254)
        {
            showTestImage();
        }
        else if (cmd == 255)
        {
            display.setWaveform(currentWaveform, true);

            display.clearDisplay();
            display.setCursor(10, 100);
            display.print("Waveform ");
            display.print(currentWaveform);
            display.print(" programmed!");
            display.display();

            Serial.print("Waveform ");
            Serial.print(currentWaveform);
            Serial.println(" burned to EEPROM.");
            break;
        }
    }
}

void loop()
{
}