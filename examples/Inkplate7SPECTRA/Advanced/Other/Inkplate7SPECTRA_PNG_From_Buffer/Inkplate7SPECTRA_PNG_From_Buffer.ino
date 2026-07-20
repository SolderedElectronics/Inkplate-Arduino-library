/**
 **************************************************
 * @file        Inkplate7SPECTRA_PNG_From_Buffer.ino
 * @brief       Display a PNG image loaded into a RAM buffer on Soldered Inkplate 7 SPECTRA.
 *
 * @details     Demonstrates how to read a PNG file from an SD card into a RAM
 *              buffer and then display it using drawPngFromBuffer(). The same
 *              technique applies to PNG data received from any source — a network
 *              socket, a serial transfer, a flash partition, etc.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 7 SPECTRA
 * - Hardware:   Inkplate 7 SPECTRA, USB cable, microSD card
 * - Extra:      SD card containing a file named "image.png"
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate 7SPECTRA
 * - SD card format: FAT / FAT32
 * - PNG resolution should not exceed 800 x 480 pixels
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/7spectra/quick-start-guide/
 *
 * How to use:
 * 1) Copy a PNG file named "image.png" to a FAT-formatted SD card.
 * 2) Insert the SD card into the Inkplate.
 * 3) Upload the sketch to Inkplate 7 SPECTRA.
 * 4) The PNG is read into RAM and rendered on the e-paper display.
 *
 * Expected output:
 * - The PNG image is shown on the Inkplate display with color dithering.
 *
 * Notes:
 * - The entire PNG file is loaded into heap memory before decoding.
 *   Make sure the file fits in available RAM (ESP32 has ~300 KB free heap).
 * - Dithering is enabled by default; pass false as the fifth argument to disable it.
 * - Colors are mapped to the Spectra 6 color e-paper palette.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2026-04-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_ESP32S3_DEV
#error "Wrong board selection for this example, please select Soldered Inkplate 7SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(1);

    if (!display.sdCardInit())
    {
        display.println("SD card error!");
        display.display();
        return;
    }

    SdFile file;
    if (!file.open("image.png", O_RDONLY))
    {
        display.println("Cannot open image.png");
        display.display();
        display.sdCardSleep();
        return;
    }

    uint32_t fileSize = file.fileSize();

    // Allocate a buffer large enough for the whole file
    uint8_t *buf = (uint8_t *)malloc(fileSize);
    if (!buf)
    {
        display.println("Not enough RAM for image buffer!");
        display.display();
        file.close();
        display.sdCardSleep();
        return;
    }

    // Read the entire file into the buffer
    file.read(buf, fileSize);
    file.close();
    display.sdCardSleep();

    // Draw the PNG from the in-memory buffer
    if (!display.image.drawPngFromBuffer(buf, fileSize, 0, 0, true, false))
    {
        display.println("PNG decode error");
    }

    free(buf);

    display.display();
}

void loop()
{
    // Nothing...
}
